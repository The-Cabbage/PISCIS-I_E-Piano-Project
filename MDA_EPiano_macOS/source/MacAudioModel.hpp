#ifndef MAC_AUDIO_MODEL_HPP
#define MAC_AUDIO_MODEL_HPP
#include <array>
#include <atomic>
#include <cstdint>
namespace piano {
constexpr unsigned Frames=441;
struct Event {int kind=0,note=0,value=0;};
// Single UI producer, single audio consumer; no blocking locks in audio callback.
class Events {
    std::array<Event,256> events{};
    std::atomic<unsigned> head{0},tail{0};
public:
    bool push(Event e){auto h=head.load(std::memory_order_relaxed);auto next=(h+1)%256;if(next==tail.load(std::memory_order_acquire))return false;events[h]=e;head.store(next,std::memory_order_release);return true;}
    bool pop(Event& e){auto t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))return false;e=events[t];tail.store((t+1)%256,std::memory_order_release);return true;}
    void discard(){tail.store(head.load(std::memory_order_acquire),std::memory_order_release);}
};
inline bool black(int n){const int k=n%12;return k==1||k==3||k==6||k==8||k==10;}
struct Rect {double x,y,w,h;};
inline Rect rect(int n){int white=0;for(int i=0;i<n;++i)if(!black(i))++white;return black(n)?Rect{20+white*39.0-12,215,24,85}:Rect{20+white*39.0,215,39,140};}
inline int hit(double x,double y){for(int pass=0;pass<2;++pass)for(int n=0;n<37;++n)if(black(n)==(pass==0)){auto r=rect(n);if(x>=r.x&&x<r.x+r.w&&y>=r.y&&y<r.y+r.h)return n;}return -1;}
inline int keyIndex(unsigned code){constexpr unsigned codes[]={0,13,1,14,2,3,17,5,16,4,32,38,40};for(int i=0;i<13;++i)if(code==codes[i])return i;return -1;}
}
#endif
