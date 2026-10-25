#include "MacAudioModel.hpp"
#include "MdaEPianoEngine.h"
#include <array>
#include <vector>
#include <cstdio>
#include <stdexcept>
#include <algorithm>
#include <thread>
void require(bool condition,const char* text){if(!condition)throw std::runtime_error(text);}
int main(){try{
    piano::Events q;piano::Event e;
    require(!q.pop(e),"empty queue");
    for(int i=0;i<255;++i)require(q.push({1,i,70}),"queue push");
    require(!q.push({1,256,70}),"queue overflow guard");
    for(int i=0;i<255;++i){require(q.pop(e),"queue pop");require(e.note==i,"queue order");}
    require(!q.pop(e),"queue should drain");
    std::thread producer([&]{for(int i=0;i<50000;++i)while(!q.push({1,i,70}))std::this_thread::yield();});
    bool ordered=true;for(int i=0;i<50000;++i){while(!q.pop(e))std::this_thread::yield();if(e.note!=i)ordered=false;}producer.join();require(ordered,"threaded event order");
    q.push({4,0,0});q.push({1,60,70});require(q.pop(e)&&e.kind==4,"panic order");require(q.pop(e)&&e.kind==1,"new note after panic");
    for(int n=0;n<37;++n){auto r=piano::rect(n);require(piano::hit(r.x+r.w/2,piano::black(n)?250:340)==n,"piano hit mapping");}
    require(piano::hit(0,0)==-1,"outside keyboard");
    require(piano::keyIndex(0)==0&&piano::keyIndex(40)==12&&piano::keyIndex(255)==-1,"Mac physical key mapping");
    MdaEPianoEngine engine;engine.setVolume(1);engine.setParameter(MDA_EP_PAN_TREM,.5f);engine.setParameter(MDA_EP_DETUNE,0);engine.setParameter(MDA_EP_TREBLE,.2f);engine.setParameter(MDA_EP_HARDNESS,.4f);
    for(int note=36;note<=96;++note){
        engine.allNotesOff();engine.noteOn(note,70);std::array<int16_t,443> l{},r{};int peak=0;
        l.front()=l.back()=r.front()=r.back()=12345;
        for(int k=0;k<40;++k){engine.render(l.data()+1,r.data()+1,441);for(unsigned i=1;i<=441;++i)peak=std::max(peak,std::abs(int(l[i])));}
        require(l.front()==12345&&l.back()==12345&&r.front()==12345&&r.back()==12345,"audio buffer guards");require(peak>0,"silent note");require(peak<32767,"single note clipping");engine.noteOff(note);
    }
    engine.reset();engine.render(nullptr,nullptr,0);
    for(unsigned size:{1u,255u,256u,441u,512u,1000u}){std::vector<int16_t> l(size+2,12345),r(size+2,12345);engine.render(l.data()+1,r.data()+1,size);require(l.front()==12345&&l.back()==12345&&r.front()==12345&&r.back()==12345,"variable buffer guards");for(unsigned i=1;i<=size;++i)require(l[i]==0&&r[i]==0,"silence write");}
    std::puts("PASS: SPSC queue, 50000 threaded events, panic ordering, 37 keys, Mac key mapping, notes 36-96, 441-frame rendering, guards and silence");return 0;
}catch(const std::exception& e){std::fprintf(stderr,"FAIL: %s\n",e.what());return 1;}}
