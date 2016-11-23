//
// Created by dawehr on 11/5/2016.
//

#ifndef ANDROWOBBLE_NATIVE_LIB_H
#define ANDROWOBBLE_NATIVE_LIB_H

#include <android/log.h>
#define LOG_TAG "AndroWobble-native"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))


#endif //ANDROWOBBLE_NATIVE_LIB_H
