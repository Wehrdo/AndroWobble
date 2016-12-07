//
// Created by dawehr on 11/16/2016.
//

#ifndef ANDROWOBBLE_AUDIO_UART_H
#define ANDROWOBBLE_AUDIO_UART_H

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "howie.h"
#include "common.h"

#include <malloc.h>
#include <cmath>
#include <string.h>

namespace AudioUART {
    void createStream();
    void destroyStream();
    void begin_output();
    void stop_output();
    void set_motors(int motorL, int motorR);

struct PlayerState {
    short* bytes_wavetable;
    short* high_line;
    // The length of each byte, in samples, including all stop its
    int frame_width;
    int frames;


};

struct PlayerParams {
    int motorL_val;
    int motorR_val;
    bool motor_updated;
};

};
#endif //ANDROWOBBLE_AUDIO_UART_H
