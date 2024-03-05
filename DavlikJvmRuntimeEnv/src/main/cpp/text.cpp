//
// Created by thehepta on 2024/2/21.
//


#include "jni.h"


#include "android/log.h"

#define LOGV(...)  ((void)__android_log_print(ANDROID_LOG_INFO, "JVMTI", __VA_ARGS__))


void NativeLoadJvmTI(JNIEnv *env, jclass thiz) {
    LOGV("NativeLoadJvmTI");
}



JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    JNIEnv* env;
    if (vm->GetEnv( (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass classTest = env->FindClass("com/hepta/davlik/JvmRuntime");
    JNINativeMethod methods[]= {
            {"nativeLoadJvmTI", "()V", (void*)NativeLoadJvmTI},
    };
    env->RegisterNatives(classTest, methods, sizeof(methods)/sizeof(JNINativeMethod));

    return JNI_VERSION_1_6;
}
