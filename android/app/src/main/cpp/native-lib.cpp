#include "native-lib.h"
#include "audio_uart.h"
#include "pid.h"

#include <jni.h>
#include <string>
#include <android/sensor.h>
#include <assert.h>
#include <algorithm>

#define GYRO_RATE 100
#define ACCEL_RATE GYRO_RATE
#define CF_RATIO 0.98

ASensorManager* sensorManager;
const ASensor* accelSensor;
const ASensor* gyroSensor;
ASensorEventQueue* sensorEventQueue;
ALooper* looper;

static bool streamExists = false;

// Whether the gyroscope and accelerometer values are fresh
bool gyroFresh = false;
bool accelFresh = false;

// Angle computed from accelerometer
double accel_angle;
// Angle difference from last gyroscope update in rad/s
double gyro_d_angle;
// Difference in time from last gyroscope value, in nanoseconds
long gyro_dt_ns;
// Filtered angle of devie
double deviceAngle = 0;
// timestanp of last gyroscope event
long lastGyroTime = -1;

int c = 0;

/*
 * Computes angle around the x-axis from accelerometer data
 * Consider phone standing upright as 0 angle
 */
static double accelToAngle(float x, float y, float z) {
    return -atan2(z, y);
}

/*
 * Called by the system whenever there are new sensor values
 */
static int sensorCallback(int fd, int events, void* data) {
    ASensorEvent event;
    // Loop over all updates in event queue
    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1)) {
        switch (event.type) {
            case ASENSOR_TYPE_ACCELEROMETER:
                accel_angle = accelToAngle(event.acceleration.x, event.acceleration.y, event.acceleration.z);
                accelFresh = true;
                break;
            case ASENSOR_TYPE_GYROSCOPE:
                gyro_dt_ns = event.timestamp - lastGyroTime;
                lastGyroTime = event.timestamp;
                gyro_d_angle = event.acceleration.x;
                gyroFresh = true;
//                LOGV("Gyro: %f, %f, %f\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
                break;
            default:
                assert(false);
                break;
        }

        if (gyroFresh && accelFresh) {
            // Convert nanoseconds to seconds
            double gyro_dt_s = gyro_dt_ns / 1000000000.0;
            // Complementary filter: use CF_RATIO of the angle from the integrated gyro, and the (1-CF_RATIO) from the accelerometer
            // Using a small value of the accelerometer eliminates drift from the gyroscope
            deviceAngle = CF_RATIO * (deviceAngle + (gyro_d_angle * gyro_dt_s)) +
                          (1 - CF_RATIO) * accel_angle;
            gyroFresh = false;
            accelFresh = false;
            c++;
        }
    }

//    if (c % 100 == 0) {
//        LOGV("smoothed: %.5f, gyro: %.5f, accel: %.5f", deviceAngle, gyro_d_angle, accel_angle);
//    }
    double pid_actuate = pidUpdate(deviceAngle, 0);
    pid_actuate = std::max(-255.0, std::min(255.0, pid_actuate));
    AudioUART::set_motors(pid_actuate, pid_actuate);


    // return 1 to continue receiving callbacks, 0 cancels callback
    return 1;
}

static void initSensors() {
    // Get accelerometer and gyroscope sensors
    sensorManager = ASensorManager_getInstance();
    accelSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    gyroSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);

    // Get looper for this thread, or create if it doesn't exist
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);

    // Create event queue and set callback
    sensorEventQueue = ASensorManager_createEventQueue(sensorManager, looper, ALOOPER_POLL_CALLBACK, sensorCallback, NULL);
}

static void registerSensors() {
    // Enable sensors
    ASensorEventQueue_enableSensor(sensorEventQueue, accelSensor);
    ASensorEventQueue_enableSensor(sensorEventQueue, gyroSensor);

    // Set update rate
    ASensorEventQueue_setEventRate(sensorEventQueue, accelSensor, ACCEL_RATE);
    ASensorEventQueue_setEventRate(sensorEventQueue, gyroSensor, GYRO_RATE);
}

static void unregisterSensors() {
    // Disable sensors
    ASensorEventQueue_disableSensor(sensorEventQueue, accelSensor);
    ASensorEventQueue_disableSensor(sensorEventQueue, gyroSensor);
}


extern "C" {

JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_beginBalancing(JNIEnv *env, jclass type) {
    initSensors();
    registerSensors();
    if (!streamExists) {
        AudioUART::createStream();
        streamExists = true;
    }
    AudioUART::begin_output();
}

JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_stopBalancing(JNIEnv *env, jclass type) {
    unregisterSensors();
    AudioUART::stop_output();
//    AudioUART::destroyStream();
}


JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_destroyStream(JNIEnv *env, jclass type) {
    if (streamExists) {
        AudioUART::destroyStream();
        streamExists = false;
    }
}


JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_setConstants(JNIEnv *env, jclass type, jdouble p,
                                                         jdouble i, jdouble d) {

    updateConstants(p, i, d);
}

}