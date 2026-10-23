//
// Created by CabbageAdmin on 2026/8/26.
//
#include "midievents.h"
#include <iostream>

// struct MidiEvents
// {
//     daisy::MidiMessageType type;
//     int channel;
//     uint8_t data[2];
//
// };

struct AObj
{
    int param0;
    int param1;
};

AObj ReturnPtr(AObj** a)
{
    *a = new AObj{10,15};
    return a;
}

int* main()
{
    AObj* b = nullptr;
    ReturnPtr(&b);
    std::cout << b->param0 << std::endl;
}