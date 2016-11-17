//
// Created by dawehr on 11/16/2016.
//

#include <android/log.h>
#include "audio_uart.h"

#define MAXIMUM_AMPLITUDE_VALUE 32767

namespace AudioUART {

    short *outBuffer;
    long stream_id;

    static void createWaveTable(
            unsigned int frames,
            unsigned int channels,
            struct PlayerState *state) {

        // First figure out how many samples we need and allocate memory for the tables
        unsigned int numSamples = frames * channels;
        state->sineWaveBuffer = (short *) malloc(sizeof(*state->sineWaveBuffer) * numSamples);
        size_t bufferSizeInBytes = numSamples * 2;

        // Now create the sine wave - we'll just create a single cycle which fills the entire table
        float phaseIncrement = (float) (3.14158265 * 2) / frames;
        float currentPhase = 0.0;

        unsigned int i;
        unsigned int j;

        for (i = 0; i < frames; i++) {

            short sampleValue = (short) (sin(currentPhase) * MAXIMUM_AMPLITUDE_VALUE);

            for (j = 0; j < channels; j++) {
                state->sineWaveBuffer[(i * channels) + j] = sampleValue;
            }

            currentPhase += phaseIncrement;
        }
    }

    static HowieError onDeviceChanged(
            const HowieDeviceCharacteristics *pHDC,
            const HowieBuffer *state,
            const HowieBuffer *params) {

        struct PlayerState *playerState = (struct PlayerState *) state->data;
        struct PlayerParams *playerParams = (struct PlayerParams *) params->data;

        createWaveTable(pHDC->framesPerPeriod, pHDC->channelCount, playerState);

        playerParams->playing = 0;
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

        if (playerParams->playing) {
            memcpy(out->data, playerState->sineWaveBuffer, out->byteCount);
        } else {
            memset(out->data, 0, out->byteCount);
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
        struct PlayerParams parms = {1};
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_PLAYING);
        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
    }

    void stop_output() {
        struct PlayerParams parms;
        parms.playing = 0;
        HowieStream *pstream = (HowieStream *) (void *) stream_id;
        HowieStreamSendParameters(pstream, &parms, sizeof(parms), 1);
        HowieStreamSetState(pstream, HOWIE_STREAM_STATE_STOPPED);
    }

}