#include <jni.h>
#include <string>
#include "RuntimePart.h"
#include "jvmti.h"
#include "runtime/dex_file.h"
#include "vector"

jobjectArray getClassLoaderList(JNIEnv *env, jclass thiz) {

    return getClassLoaders(env,android_get_application_target_sdk_version());

}

void NativeLoadJvmTI(JNIEnv *env, jclass thiz) {
    LoadJvmTI(env);
}

template <typename Dest, typename Source>
inline Dest reinterpret_cast64(Source source) {

    return reinterpret_cast<Dest>(static_cast<uintptr_t>(source));
}

constexpr size_t kOatFileIndex = 0;
constexpr size_t kDexFileIndexStart = 1;
static bool ConvertJavaArrayToDexFiles(
        JNIEnv* env,
        jobject arrayObject,
        /*out*/ std::vector<const DexFile*>& dex_files,
        /*out*/ const OatFile*& oat_file) {
    jarray array = reinterpret_cast<jarray>(arrayObject);


    jsize array_size = env->GetArrayLength(array);
    if (env->ExceptionCheck() == JNI_TRUE) {
        return false;
    }

    // TODO: Optimize. On 32bit we can use an int array.
    jboolean is_long_data_copied;
    jlong* long_data = env->GetLongArrayElements(reinterpret_cast<jlongArray>(arrayObject),
                                                 &is_long_data_copied);
    if (env->ExceptionCheck() == JNI_TRUE) {
        return false;
    }

    oat_file = reinterpret_cast64<const OatFile*>(long_data[kOatFileIndex]);
    dex_files.reserve(array_size - 1);
    for (jsize i = kDexFileIndexStart; i < array_size; ++i) {
        dex_files.push_back(reinterpret_cast64<const DexFile*>(long_data[i]));
    }
    //b4000079b3af9740
    env->ReleaseLongArrayElements(reinterpret_cast<jlongArray>(array), long_data, JNI_ABORT);
    return env->ExceptionCheck() != JNI_TRUE;
}


 jobjectArray dumpDexByCookie(JNIEnv *env, jclass thiz, jlongArray cookie) {
    // TODO: implement dumpDexByCookie()
    std::vector<const DexFile*> dex_files;

    const OatFile* oat_file;
    bool re = ConvertJavaArrayToDexFiles(env,cookie,dex_files,oat_file);
    jobjectArray dex_byteArray = env->NewObjectArray(dex_files.size(),env->FindClass("[B"), nullptr);

//    for (const DexFile* dex_file : dex_files) {
    for(int i=0;i<dex_files.size();i++){
        const DexFile* dex_file = dex_files[i];
        if (dex_file != nullptr) {
            jbyteArray dex_byte = env->NewByteArray(dex_file->data_size_);
            env->SetByteArrayRegion(dex_byte, 0, dex_file->data_size_,
                                    reinterpret_cast<const jbyte *>(dex_file->begin_));
            env->SetObjectArrayElement(dex_byteArray, i, dex_byte);
        }
    }
    return dex_byteArray;
}

jclass GlobalFindClass(JNIEnv *env, jclass thiz, jstring className){

    return nullptr;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    JNIEnv* env;
    if (vm->GetEnv( (void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass classTest = env->FindClass("com/hepta/davlik/JvmRuntime");
    JNINativeMethod methods[]= {
            {"getClassLoaderList", "()[Ljava/lang/ClassLoader;",(void*)getClassLoaderList},
            {"GlobalFindClass", "(Ljava/lang/String;)Ljava/lang/Class;",(void*)GlobalFindClass},
            {"nativeLoadJvmTI", "()V", (void*)NativeLoadJvmTI},
            {"dumpDexByCookie", "([J)[Ljava/lang/Object;", (void*)dumpDexByCookie},
    };
    env->RegisterNatives(classTest, methods, sizeof(methods)/sizeof(JNINativeMethod));

    return JNI_VERSION_1_6;
}








