#pragma once
#include "AudioEngineInterface.h"
#include "mdaEPiano.h"
#include "protocol.h"
#include <windows.h>
#include <initguid.h>
#include <devguid.h>
#include <setupapi.h>
#include <array>
#include <stdexcept>
#include <string>
#include <algorithm>
class RemoteEngine:public AudioEngineInterface {
    HANDLE serial=INVALID_HANDLE_VALUE;
    std::array<uint8_t,128> notes{};
    bool sustain=false,resetPending=true;
    uint32_t sequence=0;
    int voices=0;
    void transfer(const uint8_t *request,uint8_t *reply){
        DWORD n=0;
        if(!WriteFile(serial,request,RT_REQUEST,&n,nullptr)||n!=RT_REQUEST)throw std::runtime_error("USB write failed");
        unsigned have=0;auto deadline=GetTickCount64()+500;
        while(have<RT_RESPONSE){
            if(!ReadFile(serial,reply+have,RT_RESPONSE-have,&n,nullptr)||!n||GetTickCount64()>deadline)
                throw std::runtime_error("USB read timeout or disconnect");
            have+=n;
        }
        if(!rt_valid_response(reply,sequence))throw std::runtime_error("Wrong firmware or corrupted response");
    }
    static std::wstring findPort(){
        HDEVINFO devices=SetupDiGetClassDevsW(&GUID_DEVCLASS_PORTS,nullptr,nullptr,DIGCF_PRESENT);
        if(devices==INVALID_HANDLE_VALUE)throw std::runtime_error("Port enumeration failed");
        std::wstring found;unsigned matches=0;
        for(DWORD i=0;;++i){
            SP_DEVINFO_DATA device{};device.cbSize=sizeof(device);
            if(!SetupDiEnumDeviceInfo(devices,i,&device))break;
            wchar_t ids[2048]{};
            if(!SetupDiGetDeviceRegistryPropertyW(devices,&device,SPDRP_HARDWAREID,nullptr,reinterpret_cast<PBYTE>(ids),sizeof(ids),nullptr))continue;
            bool match=false;for(auto id=ids;*id;id+=wcslen(id)+1)if(wcsstr(id,L"VID_FFFF&PID_FFFF"))match=true;
            if(!match)continue;
            HKEY key=SetupDiOpenDevRegKey(devices,&device,DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_READ);
            if(key==INVALID_HANDLE_VALUE)continue;
            wchar_t port[128]{};DWORD bytes=sizeof(port),type=0;
            if(RegQueryValueExW(key,L"PortName",nullptr,&type,reinterpret_cast<LPBYTE>(port),&bytes)==ERROR_SUCCESS && type==REG_SZ){found=port;++matches;}
            RegCloseKey(key);
        }
        SetupDiDestroyDeviceInfoList(devices);
        if(matches!=1)throw std::runtime_error("Need exactly one F101 CDC port");
        return found;
    }
public:
    std::wstring portName;
    uint32_t worstRenderUs=0;
    unsigned responses=0;
    RemoteEngine(){
        try{
            portName=findPort();std::wstring path=L"\\.\\";path=L"\\\\.\\"+portName;
            serial=CreateFileW(path.c_str(),GENERIC_READ|GENERIC_WRITE,0,nullptr,OPEN_EXISTING,0,nullptr);
            if(serial==INVALID_HANDLE_VALUE)throw std::runtime_error("COM port busy");
            DCB dcb{};dcb.DCBlength=sizeof(dcb);if(!GetCommState(serial,&dcb))throw std::runtime_error("GetCommState");
            dcb.BaudRate=115200;dcb.ByteSize=8;dcb.Parity=NOPARITY;dcb.StopBits=ONESTOPBIT;
            dcb.fBinary=TRUE;dcb.fParity=FALSE;dcb.fOutxCtsFlow=FALSE;dcb.fOutxDsrFlow=FALSE;
            dcb.fDtrControl=DTR_CONTROL_ENABLE;dcb.fRtsControl=RTS_CONTROL_DISABLE;
            dcb.fDsrSensitivity=FALSE;dcb.fOutX=FALSE;dcb.fInX=FALSE;dcb.fNull=FALSE;dcb.fAbortOnError=FALSE;
            if(!SetCommState(serial,&dcb))throw std::runtime_error("SetCommState");
            COMMTIMEOUTS timeouts{};timeouts.ReadIntervalTimeout=MAXDWORD;timeouts.ReadTotalTimeoutConstant=250;timeouts.WriteTotalTimeoutConstant=250;
            if(!SetCommTimeouts(serial,&timeouts))throw std::runtime_error("SetCommTimeouts");
            SetupComm(serial,65536,4096);Sleep(200);PurgeComm(serial,PURGE_RXCLEAR|PURGE_TXCLEAR);
            int16_t l[RT_FRAMES],r[RT_FRAMES];render(l,r,RT_FRAMES);
        }catch(...){if(serial!=INVALID_HANDLE_VALUE){CloseHandle(serial);serial=INVALID_HANDLE_VALUE;}throw;}
    }
    ~RemoteEngine()override{
        if(serial!=INVALID_HANDLE_VALUE){
            try{allNotesOff();int16_t l[RT_FRAMES],r[RT_FRAMES];render(l,r,RT_FRAMES);}catch(...){}
            EscapeCommFunction(serial,CLRDTR);CloseHandle(serial);
        }
    }
    void noteOn(int32_t n,int32_t v)override{if(n>=0&&n<128)notes[n]=static_cast<uint8_t>(std::clamp(v,0,127));}
    void noteOff(int32_t n)override{noteOn(n,0);}
    void allNotesOff()override{notes.fill(0);sustain=false;resetPending=true;}
    void sustainPedal(bool v)override{sustain=v;}
    void setVolume(float v)override{if(v!=1.0f)throw std::runtime_error("Unsupported gain");}
    void setParameter(int i,float v)override{
        if((i==MDA_EP_PAN_TREM&&v==0.5f)||(i==MDA_EP_DETUNE&&v==0.0f)||(i==MDA_EP_TREBLE&&v==0.20f)||(i==MDA_EP_HARDNESS&&v==0.4f))return;
        throw std::runtime_error("Unsupported parameter");
    }
    void render(int16_t *l,int16_t *r,uint32_t frames)override{
        if(frames!=RT_FRAMES)throw std::runtime_error("Incorrect block size");
        uint8_t request[RT_REQUEST]{},reply[RT_RESPONSE]{};
        memcpy(request,"MDRQ",4);rt_put32(request+4,++sequence);rt_put16(request+8,frames);
        request[10]=sustain;request[11]=resetPending;memcpy(request+12,notes.data(),notes.size());
        rt_put32(request+RT_REQUEST-4,rt_crc(request,RT_REQUEST-4));
        transfer(request,reply);resetPending=false;voices=rt_u16(reply+10);
        worstRenderUs=std::max(worstRenderUs,rt_u32(reply+12));++responses;
        for(unsigned n=0;n<frames;++n){l[n]=static_cast<int16_t>(rt_u16(reply+16+n*4));r[n]=static_cast<int16_t>(rt_u16(reply+18+n*4));}
    }
    int32_t getActiveVoices()const override{return voices;}
    void reset()override{allNotesOff();}
    bool isPlaying()const override{return true;}
};
