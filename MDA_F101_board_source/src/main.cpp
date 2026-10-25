#include <zephyr/kernel.h>
#include "mdaEPiano.h"
#include "protocol.h"
#include "usb_transport.h"
static uint8_t request[RT_REQUEST],response[RT_RESPONSE],held[128];
static int16_t left[RT_FRAMES],right[RT_FRAMES];
int main(){
    mdaEPiano engine;
    engine.setLoudness(1.0f);engine.setDecay(0.4f);engine.setRelease(0.3f);
    engine.setHardness(0.4f);engine.setTreble(0.20f);engine.setPanTremolo(0.5f);
    engine.setDetune(0.0f);engine.setMaxPolyphony(32);
    mda_usb_init();
    for(;;){
        if(!mda_usb_request(request)){
            engine.processMidiController(64,0);engine.stop_voices();memset(held,0,sizeof(held));continue;
        }
        if(!rt_valid_request(request))continue;
        if(request[11]){engine.processMidiController(64,0);engine.stop_voices();memset(held,0,sizeof(held));}
        uint64_t begin=k_cycle_get_64();
        engine.processMidiController(64,request[10]?127:0);
        for(int n=0;n<128;++n){
            uint8_t v=request[12+n];if(v>127)v=127;
            if(v!=held[n]){engine.noteOn(n,v);held[n]=v;}
        }
        engine.process(right,left,RT_FRAMES);
        memcpy(response,"MDRS",4);rt_put32(response+4,rt_u32(request+4));
        rt_put16(response+8,RT_FRAMES);rt_put16(response+10,engine.getActiveVoices());
        rt_put32(response+12,k_cyc_to_us_floor64(k_cycle_get_64()-begin));
        for(unsigned n=0;n<RT_FRAMES;++n){rt_put16(response+16+n*4,(uint16_t)left[n]);rt_put16(response+18+n*4,(uint16_t)right[n]);}
        rt_put32(response+RT_RESPONSE-4,rt_crc(response,RT_RESPONSE-4));
        if(!mda_usb_send(response,sizeof(response))){
            engine.stop_voices();
            // Do not reuse the USB buffer after an uncertain transfer timeout.
            for(;;)k_msleep(1000);
        }
    }
}
