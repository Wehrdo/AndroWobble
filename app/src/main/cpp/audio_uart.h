//
// Created by dawehr on 11/16/2016.
//

#ifndef ANDROWOBBLE_AUDIO_UART_H
#define ANDROWOBBLE_AUDIO_UART_H

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "howie.h"

#include <malloc.h>
#include <cmath>
#include <string.h>

namespace AudioUART {
    void createStream();
    void destroyStream();
    void begin_output();
    void stop_output();

struct PlayerState {
    short* sineWaveBuffer;
};

struct PlayerParams {
    unsigned int playing;
};

};
#endif //ANDROWOBBLE_AUDIO_UART_H
