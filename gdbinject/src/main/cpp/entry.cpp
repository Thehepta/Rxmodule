// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("gdbinject");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("gdbinject")
//      }
//    }

#include <jni.h>
#include "gdbinject/gdbinject.h"

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    JNIEnv* env;
    if (vm->GetEnv( (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }


    return JNI_VERSION_1_6;
}



extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_gdbinject_MainActivity_startGdbServer(JNIEnv *env, jobject thiz) {
    // TODO: implement startGdbServer()

    start_gdbserver(23946);


}