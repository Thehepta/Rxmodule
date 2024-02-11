//
// Created by canyie on 2020/3/18.
//

#include "jni_bridge.h"
#include "utils/macros.h"
#include "utils/scoped_local_ref.h"

#include "jni.h"
#include "android/log.h"
#define LOG_TAG "Native"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGE("JNI_OnLoad");
    JNIEnv* env;

    if (UNLIKELY(vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)) {
        return JNI_ERR;
    }

    {
        ScopedLocalClassRef Pine(env, "top/custom/hepta/Pine");
        if (UNLIKELY(Pine.IsNull())) {
            return JNI_ERR;
        }
        if (UNLIKELY(!register_Pine(env, Pine.Get()))) {
            return JNI_ERR;
        }
    }

    {
        ScopedLocalClassRef Ruler(env, "top/custom/hepta/Ruler");
        if (UNLIKELY(Ruler.IsNull())) {
            return JNI_ERR;
        }
        if (UNLIKELY(!register_Ruler(env, Ruler.Get()))) {
            return JNI_ERR;
        }
    }

    return JNI_VERSION_1_6;
}