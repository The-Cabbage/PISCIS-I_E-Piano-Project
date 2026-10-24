#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <array>
#include <algorithm>
#include <cstring>
#include <cstdio>
#include "RemoteEngine.h"
#include <memory>



#include <thread>
#include <mutex>
#include <atomic>

namespace {
constexpr unsigned frames=441, blocks=4;
constexpr char keys[]="AWSEDFTGYHUJK";
AudioEngineInterface* synth=nullptr;


HFONT chineseFont=nullptr;
std::array<bool,13> down{};
bool pedal=false;
std::atomic<bool> running{true};
std::recursive_mutex synthMutex;
int baseNote=60;
int mouseNote=-1;
bool black(int n){int p=n%12;return p==1||p==3||p==6||p==8||p==10;}
RECT pianoRect(int n){int whites=0;for(int i=0;i<n;++i)if(!black(i))++whites;
    if(black(n))return RECT{20+whites*39-12,220,20+whites*39+12,305};
    return RECT{20+whites*39,220,20+(whites+1)*39,355};}
int pianoHit(int x,int y){POINT pt{x,y};for(int pass=0;pass<2;++pass)for(int n=0;n<37;++n)if(black(n)==(pass==0)){auto r=pianoRect(n);if(PtInRect(&r,pt))return n;}return -1;}
void releaseKeys() {
    if(!synth) return;
    synth->sustainPedal(false); pedal=false;
    if(mouseNote>=0){synth->noteOff(mouseNote);mouseNote=-1;}
    for(unsigned i=0;i<down.size();++i) if(down[i]) synth->noteOff(baseNote+i);
    down.fill(false);
}
LRESULT CALLBACK windowProc(HWND w,UINT msg,WPARAM key,LPARAM data) {
    std::unique_lock<std::recursive_mutex> stateLock(synthMutex,std::defer_lock);
    if(msg==WM_COMMAND || msg==WM_KEYDOWN || msg==WM_KEYUP || msg==WM_KILLFOCUS || msg==WM_DESTROY || msg==WM_LBUTTONDOWN || msg==WM_LBUTTONUP || msg==WM_CAPTURECHANGED)stateLock.lock();
    if(msg==WM_COMMAND && LOWORD(key)>=101 && LOWORD(key)<=103){releaseKeys();baseNote=48+(LOWORD(key)-101)*12;SetFocus(w);InvalidateRect(w,nullptr,FALSE);return 0;}
    if(msg==WM_LBUTTONDOWN){int n=pianoHit(LOWORD(data),HIWORD(data));if(n>=0){SetFocus(w);SetCapture(w);mouseNote=baseNote-12+n;synth->noteOn(mouseNote,70);InvalidateRect(w,nullptr,FALSE);}return 0;}
    if(msg==WM_LBUTTONUP || msg==WM_CAPTURECHANGED){if(mouseNote>=0){synth->noteOff(mouseNote);mouseNote=-1;InvalidateRect(w,nullptr,FALSE);}if(msg==WM_LBUTTONUP)ReleaseCapture();return 0;}
    if(msg==WM_KEYDOWN || msg==WM_KEYUP) {
        const bool pressed=msg==WM_KEYDOWN;
        if(pressed && (data & (1LL<<30))) return 0;
        if(pressed && (key=='Z' || key=='X' || key=='C')) {releaseKeys();baseNote=key=='Z'?48:(key=='X'?60:72);InvalidateRect(w,nullptr,FALSE);return 0;}
        if(key==VK_ESCAPE && pressed) {DestroyWindow(w);return 0;}
        if(key==VK_SPACE && synth) {pedal=pressed;synth->sustainPedal(pressed);return 0;}
        for(unsigned i=0;i<down.size();++i) if(key==static_cast<unsigned>(keys[i]) && synth) {
            if(pressed && !down[i]) synth->noteOn(baseNote+i,70);
            if(!pressed && down[i] && mouseNote!=baseNote+int(i)) synth->noteOff(baseNote+i);
            down[i]=pressed;InvalidateRect(w,nullptr,FALSE);return 0;
        }
    }
    if(msg==WM_KILLFOCUS) {releaseKeys();InvalidateRect(w,nullptr,FALSE);}
    if(msg==WM_PAINT) {
        PAINTSTRUCT ps;HDC dc=BeginPaint(w,&ps); RECT r;GetClientRect(w,&r);
        FillRect(dc,&r,static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
        SetBkMode(dc,TRANSPARENT);
        HGDIOBJ oldFont=SelectObject(dc,chineseFont);
        const wchar_t* help=L"F101 实时电钢琴 · 三十七键\n\n白键：A S D F G H J K　黑键：W E T Y U\n空格：延音踏板　退出键：关闭程序\n声音由 F101 计算，电脑播放；鼠标可演奏\n低／中／高音区快捷键：Z／X／C";
        RECT text{20,12,505,180};DrawTextW(dc,help,-1,&text,DT_LEFT);
        wchar_t range[160];swprintf(range,160,L"音色：MDA 电钢琴　｜　三十七键，音符编号 %d—%d",baseNote-12,baseNote+24);
        TextOutW(dc,20,190,range,int(wcslen(range)));
        for(int pass=0;pass<2;++pass)for(int n=0;n<37;++n)if(black(n)==(pass==1)){
            RECT box=pianoRect(n);bool active=mouseNote==baseNote-12+n;
            for(int k=0;k<13;++k)if(down[k] && baseNote+k==baseNote-12+n)active=true;
            FillRect(dc,&box,static_cast<HBRUSH>(GetStockObject(active?GRAY_BRUSH:(black(n)?BLACK_BRUSH:WHITE_BRUSH))));
            FrameRect(dc,&box,static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            if(n%12==0){wchar_t label[12];swprintf(label,12,L"哆%d",(baseNote-12+n)/12-1);RECT t=box;t.top=325;DrawTextW(dc,label,-1,&t,DT_CENTER|DT_SINGLELINE);}
        }
        SelectObject(dc,oldFont);EndPaint(w,&ps);return 0;
    }
    if(msg==WM_DESTROY){releaseKeys();running=false;PostQuitMessage(0);return 0;}
    return DefWindowProcW(w,msg,key,data);
}
struct Audio {
    HWAVEOUT device=nullptr;
    HANDLE ready=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    std::array<WAVEHDR,blocks> headers{};
    std::array<std::array<int16_t,frames*2>,blocks> pcm{};
    ~Audio(){if(device){waveOutReset(device);for(auto& h:headers)if(h.dwFlags&WHDR_PREPARED)waveOutUnprepareHeader(device,&h,sizeof(h));waveOutClose(device);}if(ready)CloseHandle(ready);}
};
}
int main(int argc,char** argv) {
    if(argc>1 && std::strcmp(argv[1],"--protocol-test")==0){
        uint8_t q[RT_REQUEST]{},r[RT_RESPONSE]{};
        memcpy(q,"MDRQ",4);rt_put32(q+4,123);rt_put16(q+8,RT_FRAMES);q[12+60]=70;
        rt_put32(q+RT_REQUEST-4,rt_crc(q,RT_REQUEST-4));if(!rt_valid_request(q))return 20;
        q[12+60]^=1;if(rt_valid_request(q))return 21;
        memcpy(r,"MDRS",4);rt_put32(r+4,123);rt_put16(r+8,RT_FRAMES);
        rt_put32(r+RT_RESPONSE-4,rt_crc(r,RT_RESPONSE-4));if(!rt_valid_response(r,123)||rt_valid_response(r,124))return 22;
        r[24]^=1;if(rt_valid_response(r,123))return 23;
        for(int n=0;n<37;++n){RECT key=pianoRect(n);if(pianoHit((key.left+key.right)/2,black(n)?250:340)!=n)return 24;}
        std::puts("PASS: protocol CRC, sequence checks and 37-key hit regions");return 0;
    }
    bool smoke=argc>1 && std::strcmp(argv[1],"--smoke-test")==0;
    if(smoke){
        for(int n=0;n<37;++n){RECT r=pianoRect(n);int y=black(n)?250:340;if(pianoHit((r.left+r.right)/2,y)!=n)return 7;}
        if(pianoHit(0,0)!=-1)return 8;
        std::puts("PASS: all 37 piano key hit regions");
    }
    std::unique_ptr<RemoteEngine> connection;
    try{connection=std::make_unique<RemoteEngine>();}
    catch(const std::exception& e){std::fprintf(stderr,"Connection: %s\n",e.what());if(!smoke)MessageBoxW(nullptr,L"未连接到实时演奏固件。请先加载实时版板端程序，关闭串口工具，再重新打开本程序。",L"F101 连接失败",MB_ICONERROR);return 30;}
    RemoteEngine& engine=*connection;synth=&engine;engine.setVolume(1.0f);
    engine.setParameter(MDA_EP_PAN_TREM,0.5f);
    engine.setParameter(MDA_EP_DETUNE,0.0f);
    engine.setParameter(MDA_EP_TREBLE,0.20f);
    engine.setParameter(MDA_EP_HARDNESS,0.4f);
    Audio audio;
    WAVEFORMATEX format{};format.wFormatTag=WAVE_FORMAT_PCM;format.nChannels=2;
    format.nSamplesPerSec=44100;format.wBitsPerSample=16;format.nBlockAlign=4;format.nAvgBytesPerSec=176400;
    if(!audio.ready)return 16;
    auto result=waveOutOpen(&audio.device,WAVE_MAPPER,&format,reinterpret_cast<DWORD_PTR>(audio.ready),0,CALLBACK_EVENT);
    if(result!=MMSYSERR_NOERROR){char err[256];waveOutGetErrorTextA(result,err,sizeof(err));MessageBoxW(nullptr,L"无法打开音频输出，请检查系统的默认播放设备。",L"音频输出失败",MB_ICONERROR);return 1;}
    for(unsigned i=0;i<blocks;++i){auto& h=audio.headers[i];h.lpData=reinterpret_cast<LPSTR>(audio.pcm[i].data());h.dwBufferLength=frames*4;
        if(waveOutPrepareHeader(audio.device,&h,sizeof(h))!=MMSYSERR_NOERROR)return 2;}
    HINSTANCE instance=GetModuleHandle(nullptr);WNDCLASSW wc{};wc.lpfnWndProc=windowProc;wc.hInstance=instance;wc.lpszClassName=L"MdaF101LivePiano";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);
    if(!RegisterClassW(&wc))return 3;
    HWND window=CreateWindowW(wc.lpszClassName,L"F101 实时电钢琴 · 三十七键",WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,CW_USEDEFAULT,CW_USEDEFAULT,925,435,nullptr,nullptr,instance,nullptr);
    if(!window)return 4;
    chineseFont=CreateFontW(-19,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH,L"Microsoft YaHei");
    const wchar_t* labels[]={L"低音区（Z）",L"中音区（X）",L"高音区（C）"};
    for(int i=0;i<3;++i){int id=101+i;
        HWND button=CreateWindowW(L"BUTTON",labels[i],WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,520+(i%3)*120,i<3?35:100,110,38,window,reinterpret_cast<HMENU>(INT_PTR(id)),instance,nullptr);
        SendMessageW(button,WM_SETFONT,reinterpret_cast<WPARAM>(chineseFont),TRUE);
    }
    if(smoke){
        for(int id=101;id<=103;++id){SendMessageW(window,WM_COMMAND,id,0);if(baseNote!=48+12*(id-101))return 13;}
        SendMessageW(window,WM_COMMAND,102,0);
        SendMessageW(window,WM_LBUTTONDOWN,0,MAKELPARAM(30,340));
        if(mouseNote!=48)return 14;
        SendMessageW(window,WM_LBUTTONUP,0,0);if(mouseNote!=-1)return 15;
        engine.allNotesOff();std::puts("PASS: octave controls and mouse release");
    }
    if(!smoke)ShowWindow(window,SW_SHOW);
    auto started=GetTickCount64();bool released=false,stalled=false;int testPhase=0;
    std::atomic<unsigned> submitted{0},underruns{0};std::atomic<int> peak{0},audioError{0};
    if(smoke)engine.noteOn(60,70);
    std::thread audioThread([&]{
        std::array<int16_t,frames> left{},right{};
        try{
            while(running){
                unsigned available=0;for(auto& h:audio.headers)if(!(h.dwFlags&WHDR_INQUEUE))++available;
                if(available==blocks && submitted>0)++underruns;
                for(unsigned i=0;i<blocks;++i){auto& h=audio.headers[i];if(h.dwFlags&WHDR_INQUEUE)continue;
                    {std::lock_guard<std::recursive_mutex> lock(synthMutex);synth->render(left.data(),right.data(),frames);}
                    int blockPeak=0;
                    for(unsigned j=0;j<frames;++j){
                        // One master gain stage; keep headroom for polyphony.
                        audio.pcm[i][j*2]=left[j];audio.pcm[i][j*2+1]=right[j];
                        blockPeak=std::max(blockPeak,std::abs(int(left[j])));
                    }
                    peak=std::max(peak.load(),blockPeak);
                    if(waveOutWrite(audio.device,&h,sizeof(h))!=MMSYSERR_NOERROR){audioError=5;running=false;break;}
                    ++submitted;
                }
                WaitForSingleObject(audio.ready,5);
            }
        }catch(...){audioError=17;running=false;}
    });
    while(running){
        MSG msg;while(PeekMessageW(&msg,nullptr,0,0,PM_REMOVE)){if(msg.message==WM_QUIT)running=false;TranslateMessage(&msg);DispatchMessageW(&msg);}
        if(!running)break;
        if(smoke && GetTickCount64()-started>300 && !stalled){Sleep(120);stalled=true;}
        if(smoke && GetTickCount64()-started>600 && !released){std::lock_guard<std::recursive_mutex> lock(synthMutex);engine.noteOff(60);released=true;}
        if(smoke && GetTickCount64()-started>2000 && testPhase==0){std::lock_guard<std::recursive_mutex> lock(synthMutex);for(int n=48;n<80;++n)engine.noteOn(n,40);testPhase=1;}
        if(smoke && GetTickCount64()-started>6000 && testPhase==1){std::lock_guard<std::recursive_mutex> lock(synthMutex);engine.allNotesOff();testPhase=2;}
        if(smoke && GetTickCount64()-started>10000)break;
        Sleep(1);
    }
    running=false;SetEvent(audio.ready);audioThread.join();
    std::printf("USB: responses=%u worst_board_render_us=%u\n",engine.responses,engine.worstRenderUs);
    bool playbackClockOk=true;
    if(smoke){MMTIME pos{};pos.wType=TIME_SAMPLES;
        const auto code=waveOutGetPosition(audio.device,&pos,sizeof(pos));
        const auto elapsed=GetTickCount64()-started;
        const double effectiveRate=elapsed?double(pos.u.sample)*1000.0/double(elapsed):0.0;
        playbackClockOk=code==MMSYSERR_NOERROR && pos.wType==TIME_SAMPLES && effectiveRate>41000.0 && effectiveRate<46000.0;
        std::printf("stream: elapsed_ms=%llu submitted_frames=%u played_frames=%u effective_rate=%.1f clock_ok=%d\n",elapsed,submitted.load()*frames,pos.u.sample,effectiveRate,playbackClockOk);
    }
    if(IsWindow(window))DestroyWindow(window);
    synth=nullptr;
    if(chineseFont)DeleteObject(chineseFont);
    if(audioError){if(!smoke)MessageBoxW(nullptr,L"USB 或音频输出中断。请检查板子连接，关闭程序后重新打开；必要时重新加载板端程序。",L"音频错误",MB_ICONERROR);return audioError.load();}
    if(smoke){std::printf("smoke: blocks=%u peak=%d released=%d ui_stall_test=%d underruns=%u\n",submitted.load(),peak.load(),released,stalled,underruns.load());return submitted>20&&peak>0&&released&&underruns==0&&playbackClockOk?0:6;}
    return 0;
}
