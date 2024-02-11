

#include <android/log.h>
#include <jni.h>
#include <ifaddrs.h>
#include <net/if.h>

#include "../include/dobby.h"
#include <linux/if_tun.h>
#include <linux/ioctl.h>
#include <cstring>
#include <bits/ioctl.h>
#include <dlfcn.h>
#include <link.h>
#include "vector"
#define LOG_TAG "checkVpn Native"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


//#define TUNSETIFF     _IOW('T', 202, int)



int (*ioctl_old)(int __fd, unsigned long int __request, void * arg) = 0;

int ioctl_hook(int __fd, unsigned long int __request, void * arg) {

    if(TUNSETIFF == __request){
        LOGE("ioctl_hook miss TUNSETIFF");
        struct ifreq * ifr4 = (struct ifreq *)arg;
        strncpy((*ifr4).ifr_name,"mytun",IFNAMSIZ);
    }
    return ioctl_old(__fd,__request,arg);
}







void hook_start() {

    void*ioctl_addr  = DobbySymbolResolver(nullptr, "ioctl");
    DobbyHook(ioctl_addr, (void*)ioctl_hook,reinterpret_cast<dobby_dummy_func_t *>(&ioctl_old));
    LOGE("ioctl %p",ioctl_addr);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_hepta_checkvpn_hookEntry_inject(JNIEnv *env, jobject thiz) {
    // TODO: implement inject()

    hook_start();

}