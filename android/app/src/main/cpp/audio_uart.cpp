//
// Created by dawehr on 11/16/2016.
//

#include <android/log.h>
#include "audio_uart.h"
#include <stdio.h>

#define MAXIMUM_AMPLITUDE_VALUE 32767
#define BAUD_RATE 24000

namespace AudioUART {

    short *outBuffer;
    long stream_id;

    static void createWaveTable(
            int frames,
            int channels,
            int sampleRate,
            struct PlayerState *state) {
        if (channels != 2) {
            __android_log_print(ANDROID_LOG_ERROR, "audio_uart.cpp", "Channels is equal to %d, not 2\n", channels);
        }
        // First figure out how many samples we need and allocate memory for the tables
        state->frames = frames;
        int numSamples = frames * 256;
        state->bytes_wavetable = (short *) malloc(sizeof(*state->bytes_wavetable) * numSamples);

        // Number of samples per UART bit
        float bit_width = sampleRate / BAUD_RATE;

//        unsigned int i;
//        unsigned int j;

        for (int count = -128; count < 128; count++) {
            char b = (char) count;
            short* wave = &state->bytes_wavetable[frames * (unsigned char)b];
            for (int i = 0; i < frames; i++) {
                int bit_id = floor((float)i / bit_width - 1);
                short to_send;
                // start bit
                if (bit_id == -1) {
                    to_send = -MAXIMUM_AMPLITUDE_VALUE;
                }
                else if (bit_id < 8 && bit_id >= 0) {
                    char bit_send = (b >> bit_id) & (0x01);
                    to_send = ((2*bit_send) - 1) * MAXIMUM_AMPLITUDE_VALUE;
                }
                // stop bit and holding line high to indicate no data
                else {
                    to_send = MAXIMUM_AMPLITUDE_VALUE;
                }

                wave[i] = to_send;
            }
    }
    }

    static HowieError onDeviceChanged(
            const HowieDeviceCharacteristics *pHDC,
            const HowieBuffer *state,
            const HowieBuffer *params) {

        struct PlayerState *playerState = (struct PlayerState *) state->data;
        struct PlayerParams *playerParams = (struct PlayerParams *) params->data;

        createWaveTable(pHDC->framesPerPeriod, pHDC->channelCount, pHDC->sampleRate, playerState);

        playerParams->motor_updated = 0;
        return HOWIE_SUCCESS;
    }

    static HowieError OnCleanupCallback(
            const HowieStream *stream,
            const HowieBuffer *state) {
        return HOWIE_SUCCESS;
    }

    static HowieError onProcess(
            const HowieStream *stream,
            const HowieBuffer *in,
            const HowieBuffer *out,
            const HowieBuffer *state,
            const HowieBuffer *params) {

        struct PlayerState *playerState = (struct PlayerState *) state->data;
        struct PlayerParams *playerParams = (struct PlayerParams *) params->data;
        if (playerParams->motor_updated) {

            short* outdata_short = (short*) out->data;
            short* left_byte = &playerState->bytes_wavetable[playerState->frames * (unsigned char)playerParams->motorL_val];

            for (int i = 0; i < playerState->frames; i += 1 ) {
                outdata_short[i] = left_byte[i];
            }

//            short* right_byte = &playerState->bytes_wavetable[(unsigned char)playerParams->motorR_val];
//            for (int i = 1; i < playerState->frames; i += 1 ) {
//                outdata_short[i] = right_byte[i];
//            }
            size_t left_to_copy = out->byteCount - (playerState->frames * sizeof(short));
            if (left_to_copy) {
                memset(out->data + (playerState->frames * sizeof(short)), MAXIMUM_AMPLITUDE_VALUE, left_to_copy);
            }
            playerParams->motor_updated = false;
        } else {
            memset(out->data, MAXIMUM_AMPLITUDE_VALUE, out->byteCount);
        }

        return HOWIE_SUCCESS;
    }

    void createStream() {
        // Creates stream
        HowieStreamCreationParams hscp = {
                sizeof(HowieStreamCreationParams), HOWIE_STREAM_DIRECTION_PLAYBACK,
                onDeviceChanged,
                onProcess,
                OnCleanupCallback,
                sizeof(struct PlayerState),
                sizeof(struct PlayerParams),
                HOWIE_STREAM_STATE_STOPPED
        };
        HowieStreamCreate(&hscp, (HowieStream **) &stream_id);
    }

    void destroyStream() {
        // Destroys stream
        HowieStream *stream = reinterpret_cast<HowieStream *>(stream_id);
        HowieStreamDestroy(stream);
    }

    void begin_output() {
        struct PlayerParams parms = {0, 0, true};
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_PLAYING);
        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
    }

    void stop_output() {
        struct PlayerParams parms;
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_STOPPED);
    }

    void set_motors(char motorL, char motorR) {
        struct PlayerParams params = {
                motorL,
                motorR,
                true
        };
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSendParameters(pstream, &params, sizeof(params), 1);
    }

}