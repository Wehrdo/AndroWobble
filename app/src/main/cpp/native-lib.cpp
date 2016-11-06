#include "native-lib.h"

#include <jni.h>
#include <string>
#include <android/sensor.h>
#include <android/looper.h>
#include <android/log.h>
#include <assert.h>
#include <cmath>

#define GYRO_RATE 100
#define ACCEL_RATE GYRO_RATE
#define CF_RATIO 0.98

ASensorManager* sensorManager;
const ASensor* accelSensor;
const ASensor* gyroSensor;
ASensorEventQueue* sensorEventQueue;
ALooper* looper;

bool gyroFresh = false;
bool accelFresh = false;
double accel_angle, gyro_d_angle;
long gyro_dt_ns;
double deviceAngle = 0;
long lastGyroTime = -1;
int c = 0;

static double accelToAngle(float x, float y, float z) {
    return -atan2(z, y);
}

static int sensorCallback(int fd, int events, void* data) {
    ASensorEvent event;

    while (ASensorEventQueue_getEvents(sensorEventQueue, &event, 1)) {
        switch (event.type) {
            case ASENSOR_TYPE_ACCELEROMETER:
                accel_angle = accelToAngle(event.acceleration.x, event.acceleration.y, event.acceleration.z);
                accelFresh = true;
                break;
//                LOGV("Accel: %f, %f, %f\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
            case ASENSOR_TYPE_GYROSCOPE:
                gyro_dt_ns = event.timestamp - lastGyroTime;
                lastGyroTime = event.timestamp;
                gyro_d_angle = event.acceleration.x;
                gyroFresh = true;

                LOGV("raw: %f, filteded: %f", event.uncalibrated_gyro.z_uncalib, event.acceleration.z);
//                LOGV("Gyro: %f, %f, %f\n", event.acceleration.x, event.acceleration.y, event.acceleration.z);
                break;
            default:
                assert(false);
                break;
        }
    }
    if (gyroFresh && accelFresh) {
        double gyro_dt_s = gyro_dt_ns / 1000000000.0;
        deviceAngle = CF_RATIO * (deviceAngle + (gyro_d_angle * gyro_dt_s)) +
                      (1 - CF_RATIO) * accel_angle;
        c++;
        gyroFresh = false;
        accelFresh = false;
    }
//        if (c % 10 == 0) {
//    LOGV("smoothed: %.5f, gyro: %.5f, accel: %.5f", deviceAngle, gyro_d_angle, accel_angle);
//        }


    // return 1 to continue receiving callbacks, 0 cancels callback
    return 1;
}

static void initSensors() {
    // Get accelerometer and gyroscope sensors
    sensorManager = ASensorManager_getInstance();
    accelSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    gyroSensor = ASensorManager_getDefaultSensor(sensorManager, ASENSOR_TYPE_GYROSCOPE);

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
Java_com_davidawehr_androwobble_NativeCalls_registerSensors(JNIEnv *env, jclass type) {
    registerSensors();
}


JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_unregisterSensors(JNIEnv *env, jclass type) {
    unregisterSensors();
}

JNIEXPORT void JNICALL
Java_com_davidawehr_androwobble_NativeCalls_initSensors(JNIEnv *env, jclass type) {
    initSensors();
}


jstring
Java_com_davidawehr_androwobble_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

}