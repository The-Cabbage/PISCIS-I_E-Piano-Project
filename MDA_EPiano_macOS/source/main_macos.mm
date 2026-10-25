#import <Cocoa/Cocoa.h>
#import <AudioToolbox/AudioToolbox.h>
#include "MdaEPianoEngine.h"
#include "MacAudioModel.hpp"
#include <array>
#include <atomic>
#include <cstring>
#include <exception>

class MacAudio {
    MdaEPianoEngine engine;
    piano::Events events;
    std::atomic<bool> panic{false},live{false};
    AudioQueueRef queue=nullptr;
    std::array<AudioQueueBufferRef,4> buffers{};
    static void callback(void* ctx,AudioQueueRef,AudioQueueBufferRef buffer){static_cast<MacAudio*>(ctx)->fill(buffer);}
    void fill(AudioQueueBufferRef buffer) noexcept {
        if(!live.load(std::memory_order_acquire))return;
        try{
            if(panic.exchange(false,std::memory_order_acq_rel)){events.discard();engine.sustainPedal(false);engine.allNotesOff();}
            piano::Event e;
            while(events.pop(e)){
                if(e.kind==1)engine.noteOn(e.note,e.value);
                else if(e.kind==2)engine.noteOff(e.note);
                else if(e.kind==3)engine.sustainPedal(e.value!=0);
                else if(e.kind==4){engine.sustainPedal(false);engine.allNotesOff();}
            }
            std::array<int16_t,piano::Frames> left{},right{};
            engine.render(left.data(),right.data(),piano::Frames);
            auto* pcm=static_cast<int16_t*>(buffer->mAudioData);
            for(unsigned i=0;i<piano::Frames;++i){pcm[i*2]=left[i];pcm[i*2+1]=right[i];}
            buffer->mAudioDataByteSize=piano::Frames*4;
            const OSStatus result=AudioQueueEnqueueBuffer(queue,buffer,0,nullptr);
            if(result!=noErr)error.store(result);
        }catch(...){error.store(-1);}
    }
public:
    std::atomic<OSStatus> error{noErr};
    MacAudio(){engine.setVolume(1.0f);engine.setParameter(MDA_EP_PAN_TREM,.5f);engine.setParameter(MDA_EP_DETUNE,0);engine.setParameter(MDA_EP_TREBLE,.2f);engine.setParameter(MDA_EP_HARDNESS,.4f);}
    bool send(piano::Event e){if(events.push(e))return true;panic.store(true,std::memory_order_release);return false;}
    void silence(){send(piano::Event{4,0,0});}
    OSStatus start(){
        AudioStreamBasicDescription format{};
        format.mSampleRate=44100;format.mFormatID=kAudioFormatLinearPCM;
        format.mFormatFlags=kLinearPCMFormatFlagIsSignedInteger|kLinearPCMFormatFlagIsPacked;
        format.mBytesPerPacket=4;format.mFramesPerPacket=1;format.mBytesPerFrame=4;format.mChannelsPerFrame=2;format.mBitsPerChannel=16;
        OSStatus result=AudioQueueNewOutput(&format,callback,this,nullptr,nullptr,0,&queue);
        if(result!=noErr)return result;
        for(auto& buffer:buffers){result=AudioQueueAllocateBuffer(queue,piano::Frames*4,&buffer);if(result!=noErr){stop();return result;}}
        live.store(true,std::memory_order_release);
        for(auto buffer:buffers)fill(buffer);
        if(error.load()!=noErr){result=error.load();stop();return result;}
        result=AudioQueueStart(queue,nullptr);if(result!=noErr)stop();return result;
    }
    void stop(){live.store(false,std::memory_order_release);if(queue){AudioQueueStop(queue,true);AudioQueueDispose(queue,true);queue=nullptr;}}
    ~MacAudio(){stop();}
};

@interface PianoView : NSView {
    MacAudio* audio;
    std::array<bool,13> down;
    std::array<int,128> owners;
    int base,mouse;
    bool pedal;
}
- (instancetype)initWithAudio:(MacAudio*)host;
- (void)releaseAll;
- (void)changeRange:(id)sender;
@end
@implementation PianoView
- (instancetype)initWithAudio:(MacAudio*)host {
    self=[super initWithFrame:NSMakeRect(0,0,900,390)];
    if(self){audio=host;base=60;mouse=-1;pedal=false;down.fill(false);owners.fill(0);
        NSArray<NSString*>* names=@[@"低音区（Z）",@"中音区（X）",@"高音区（C）"];
        for(NSInteger i=0;i<3;++i){NSButton* button=[NSButton buttonWithTitle:names[i] target:self action:@selector(changeRange:)];button.frame=NSMakeRect(520+i*120,40,110,34);button.tag=i;[button setRefusesFirstResponder:YES];[self addSubview:button];}
    }return self;
}
- (BOOL)isFlipped{return YES;}
- (BOOL)acceptsFirstResponder{return YES;}
- (BOOL)acceptsFirstMouse:(NSEvent*)event{(void)event;return YES;}
- (void)emit:(piano::Event)e {if(!audio->send(e)){down.fill(false);owners.fill(0);mouse=-1;pedal=false;} }
- (void)press:(int)note {if(note<0||note>127)return;if(owners[note]++==0)[self emit:piano::Event{1,note,70}];}
- (void)releaseNote:(int)note {if(note<0||note>127)return;if(owners[note]>0 && --owners[note]==0)[self emit:piano::Event{2,note,0}];}
- (void)releaseAll {audio->silence();down.fill(false);owners.fill(0);mouse=-1;pedal=false;self.needsDisplay=YES;}
- (void)setRange:(int)value {[self releaseAll];base=value;[self.window makeFirstResponder:self];self.needsDisplay=YES;}
- (void)changeRange:(id)sender {[self setRange:48+12*int([sender tag])];}
- (void)keyDown:(NSEvent*)event {
    if(event.isARepeat)return;
    if(event.modifierFlags & NSEventModifierFlagCommand)return;
    unsigned code=event.keyCode;
    if(code==53){[self.window performClose:self];return;}
    if(code==6||code==7||code==8){[self setRange:code==6?48:(code==7?60:72)];return;}
    if(code==49){pedal=true;[self emit:piano::Event{3,0,1}];return;}
    int index=piano::keyIndex(code);if(index>=0&&!down[index]){down[index]=true;[self press:base+index];self.needsDisplay=YES;}
}
- (void)keyUp:(NSEvent*)event {
    if(event.keyCode==49){pedal=false;[self emit:piano::Event{3,0,0}];return;}
    int index=piano::keyIndex(event.keyCode);if(index>=0&&down[index]){down[index]=false;[self releaseNote:base+index];self.needsDisplay=YES;}
}
- (void)mouseDown:(NSEvent*)event {
    [self.window makeFirstResponder:self];NSPoint p=[self convertPoint:event.locationInWindow fromView:nil];int n=piano::hit(p.x,p.y);
    if(n>=0){if(mouse>=0)[self releaseNote:mouse];mouse=base-12+n;[self press:mouse];self.needsDisplay=YES;}
}
- (void)mouseUp:(NSEvent*)event {(void)event;if(mouse>=0){[self releaseNote:mouse];mouse=-1;self.needsDisplay=YES;}}
- (void)drawRect:(NSRect)dirty {
    (void)dirty;[[NSColor windowBackgroundColor]setFill];NSRectFill(self.bounds);
    NSDictionary* normal=@{NSFontAttributeName:[NSFont systemFontOfSize:16],NSForegroundColorAttributeName:NSColor.labelColor};
    NSDictionary* title=@{NSFontAttributeName:[NSFont boldSystemFontOfSize:24],NSForegroundColorAttributeName:NSColor.labelColor};
    [@"三十七键 MDA 电钢琴" drawAtPoint:NSMakePoint(20,18) withAttributes:title];
    [@"白键：A S D F G H J K　黑键：W E T Y U\n空格：延音踏板　退出键：关闭窗口\n鼠标按住琴键演奏；低／中／高音区快捷键：Z／X／C" drawInRect:NSMakeRect(20,75,860,95) withAttributes:normal];
    [[NSString stringWithFormat:@"当前音区：音符编号 %d—%d　｜　共三十七键",base-12,base+24]drawAtPoint:NSMakePoint(20,181) withAttributes:normal];
    for(int pass=0;pass<2;++pass)for(int n=0;n<37;++n)if(piano::black(n)==(pass==1)){
        auto r=piano::rect(n);NSRect box=NSMakeRect(r.x,r.y,r.w,r.h);int note=base-12+n;
        NSColor* color=owners[note]>0?[NSColor colorWithRed:.32 green:.65 blue:.90 alpha:1]:(piano::black(n)?NSColor.blackColor:NSColor.whiteColor);
        [color setFill];NSRectFill(box);[NSColor.blackColor setStroke];[NSBezierPath strokeRect:box];
        if(n%12==0){NSDictionary* label=@{NSFontAttributeName:[NSFont systemFontOfSize:12],NSForegroundColorAttributeName:NSColor.blackColor};[[NSString stringWithFormat:@"哆%d",note/12-1]drawAtPoint:NSMakePoint(r.x+5,333) withAttributes:label];}
    }
}
@end

@interface PianoDelegate : NSObject<NSApplicationDelegate,NSWindowDelegate> {
    MacAudio* audio;
    NSWindow* window;
    PianoView* piano;
    NSTimer* timer;
    BOOL reported;
}
@end
@implementation PianoDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    (void)notification;audio=new MacAudio();
    window=[[NSWindow alloc]initWithContentRect:NSMakeRect(0,0,900,390) styleMask:NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|NSWindowStyleMaskMiniaturizable backing:NSBackingStoreBuffered defer:NO];
    window.title=@"MDA 电钢琴 · 苹果电脑版";window.delegate=self;window.releasedWhenClosed=NO;
    piano=[[PianoView alloc]initWithAudio:audio];window.contentView=piano;[window center];[window makeKeyAndOrderFront:nil];[window makeFirstResponder:piano];[NSApp activateIgnoringOtherApps:YES];
    OSStatus status=audio->start();if(status!=noErr){[self showAudioError:status];return;}
    timer=[NSTimer scheduledTimerWithTimeInterval:.25 target:self selector:@selector(checkAudio:) userInfo:nil repeats:YES];
}
- (void)showAudioError:(OSStatus)status {
    if(reported)return;reported=YES;audio->stop();NSAlert* alert=[NSAlert new];alert.messageText=@"无法播放声音";
    alert.informativeText=[NSString stringWithFormat:@"请检查系统默认输出设备，再重新打开程序。音频错误码：%d",int(status)];[alert addButtonWithTitle:@"关闭程序"];[alert runModal];[NSApp terminate:self];
}
- (void)checkAudio:(NSTimer*)sender {(void)sender;OSStatus status=audio->error.load();if(status!=noErr)[self showAudioError:status];}
- (void)windowDidResignKey:(NSNotification*)notification {(void)notification;[piano releaseAll];}
- (void)applicationDidResignActive:(NSNotification*)notification {(void)notification;[piano releaseAll];}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {(void)sender;return YES;}
- (void)applicationWillTerminate:(NSNotification*)notification {(void)notification;[timer invalidate];if(audio){audio->stop();delete audio;audio=nullptr;}}
@end
int main(){@autoreleasepool {
    NSApplication* app=NSApplication.sharedApplication;[app setActivationPolicy:NSApplicationActivationPolicyRegular];
    NSMenu* menu=[NSMenu new];NSMenuItem* root=[NSMenuItem new];[menu addItem:root];NSMenu* submenu=[NSMenu new];
    [submenu addItemWithTitle:@"退出电钢琴" action:@selector(terminate:) keyEquivalent:@"q"];root.submenu=submenu;app.mainMenu=menu;
    static PianoDelegate* delegate;delegate=[PianoDelegate new];app.delegate=delegate;[app run];
}return 0;}
