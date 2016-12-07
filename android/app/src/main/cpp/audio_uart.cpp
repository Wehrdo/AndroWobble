//
// Created by dawehr on 11/16/2016.
//

#include <android/log.h>
#include "audio_uart.h"
#include <stdio.h>
#include <assert.h>
#include <cstdlib>

#define MAXIMUM_AMPLITUDE_VALUE 32767
#define BAUD_RATE 24000
#define STOP_BITS 10
#define START_BITS 1

namespace AudioUART {

    short *outBuffer;
    long stream_id;

    static void createWaveTable(
            int frames,
            int channels,
            int sampleRate,
            struct PlayerState *state) {
        assert(channels == 1);

        // Create bytes wavetable

        // First figure out how many samples we need and allocate memory for the tables
        state->frames = frames;
        int byte_frame_bits = 8 + START_BITS + STOP_BITS;
        // Number of samples per UART bit
        float bit_width = sampleRate / BAUD_RATE;
        // Number of samples per byte frame
        int frame_width = ceil(byte_frame_bits * bit_width);
        state->frame_width = frame_width;
        state->bytes_wavetable = (short *) malloc(sizeof(*state->bytes_wavetable) * 256 * frame_width);

        // Fill out bytes wavetable
        for (int count = -128; count < 128; count++) {
            char b = (char) count;
            short *wave = &state->bytes_wavetable[frame_width * (unsigned char) b];
            for (int i = 0; i < frame_width; i++) {
                int bit_id = floor((float) i / bit_width - START_BITS);
                short to_send;
                // start bits
                if (bit_id < 0) {
                    to_send = -MAXIMUM_AMPLITUDE_VALUE;
                }
                    // data bits
                else if (bit_id < 8 && bit_id >= 0) {
                    char bit_send = (b >> bit_id) & (0x01);
                    to_send = ((2 * bit_send) - 1) * MAXIMUM_AMPLITUDE_VALUE;
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
        LOGV("Sample rate: %d", pHDC->sampleRate);
        createWaveTable(pHDC->framesPerPeriod, pHDC->channelCount, pHDC->sampleRate, playerState);

        playerParams->motor_updated = 0;
        return HOWIE_SUCCESS;
    }

    static HowieError OnCleanupCallback(
            const HowieStream *stream,
            const HowieBuffer *state) {
        struct PlayerState* playerState = (struct PlayerState *) state->data;
        free(playerState->bytes_wavetable);
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
            // Start byte
            short* start_byte = &playerState->bytes_wavetable[playerState->frame_width * 0xBE];
            // Motor value bytes
            unsigned char l_abs_val = abs(playerParams->motorL_val);
            unsigned char r_abs_val = abs(playerParams->motorR_val);
            short* left_byte = &playerState->bytes_wavetable[playerState->frame_width * l_abs_val];
            short* right_byte = &playerState->bytes_wavetable[playerState->frame_width * r_abs_val];
            // Direction byte. bit 0 is left sign, bit 1 is right sign
            unsigned char dir = ((playerParams->motorL_val >> 7) & 0x01) |
                                     ((playerParams->motorR_val >> 6) & 0x02);
            short* dir_byte = &playerState->bytes_wavetable[playerState->frame_width * dir];


            size_t copy_size = playerState->frame_width * sizeof(short);
//            // Copy start byte
//            memcpy(out->data, start_byte, copy_size);
//            // Copy direction byte
//            memcpy(out->data + copy_size, dir_byte, copy_size);
//            // Copy left value byte
//            memcpy(out->data + (2 * copy_size), left_byte, copy_size);
//            // Copy right value byte
//            memcpy(out->data + (3 * copy_size),right_byte, copy_size);
//
//            // Fill out remaining buffer with high
//            size_t left_to_copy = out->byteCount - 4*copy_size;
//            assert(left_to_copy >= 0);
//            if (left_to_copy) {
//                memset(out->data + 4*copy_size, MAXIMUM_AMPLITUDE_VALUE, left_to_copy);
//            }


            /////// Testing /////////
            memcpy(out->data, left_byte, copy_size);
            size_t left_to_copy = out->byteCount - copy_size;
            assert(left_to_copy >= 0);
            if (left_to_copy) {
                memset(out->data + 4*copy_size, MAXIMUM_AMPLITUDE_VALUE, left_to_copy);
            }
            ///////// End testing ////////

            playerParams->motor_updated = false;
        } else {
            // Hold line high if no data
            memset(out->data, MAXIMUM_AMPLITUDE_VALUE, out->byteCount);
        }

        return HOWIE_SUCCESS;
    }

    void createStream() {
        LOGV("createStream");
        // Creates stream
        HowieStreamCreationParams hscp = {
                sizeof(HowieStreamCreationParams),
                HOWIE_STREAM_DIRECTION_PLAYBACK,
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
        LOGV("destroyStream");
        // Destroys stream
        HowieStream *pstream = reinterpret_cast<HowieStream *>(stream_id);
        HowieStreamDestroy(pstream);
    }

    void begin_output() {
        LOGV("begin_output");
        struct PlayerParams parms = {0, 0, true};
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_PLAYING);
        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
    }

    void stop_output() {
        LOGV("stop_output");
//        struct PlayerParams parms;
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
//        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_STOPPED);
    }

    void set_motors(int motorL, int motorR) {
        struct PlayerParams params = {
                motorL,
                motorR,
                true
        };
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSendParameters(pstream, &params, sizeof(params), 1);
    }

}