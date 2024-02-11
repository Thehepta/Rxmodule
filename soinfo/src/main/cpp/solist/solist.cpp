//
// Created by chic on 2023/6/1.
//

#include <vector>
#include <iostream>
#include "elf_symbol_resolver.h"
#include "android/log.h"
#include "solist.h"

#define LOG_TAG "solist"

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
typedef void *soinfo_t;

#undef Q
#define Q 29

std::vector<soinfo_t> linker_solist;
typedef uintptr_t addr_t;


static int get_android_system_version() {
    char os_version_str[100];
    __system_property_get("ro.build.version.sdk", os_version_str);
    int os_version_int = atoi(os_version_str);
    return os_version_int;
}

static const char *get_android_linker_path() {
#if __LP64__
    if (get_android_system_version() >= Q) {
        return (const char *)"/apex/com.android.runtime/bin/linker64";
    } else {
        return (const char *)"/system/bin/linker64";
    }
#else
    if (get_android_system_version() >= Q) {
        return (const char *)"/apex/com.android.runtime/bin/linker";
    } else {
        return (const char *)"/system/bin/linker";
    }
#endif
}


std::vector<soinfo_t> linker_get_solist() {
    if (!linker_solist.empty()) {
        linker_solist.clear();
    }

    static soinfo_t (*solist_get_head)() = NULL;
    if (!solist_get_head)
        solist_get_head =
                (soinfo_t(*)())resolve_elf_internal_symbol(get_android_linker_path(), "__dl__Z15solist_get_headv");

    static soinfo_t (*solist_get_somain)() = NULL;
    if (!solist_get_somain)
        solist_get_somain =
                (soinfo_t(*)())resolve_elf_internal_symbol(get_android_linker_path(), "__dl__Z17solist_get_somainv");

    static addr_t *solist_head = NULL;
    if (!solist_head)
        solist_head = (addr_t *)solist_get_head();

    static addr_t somain = 0;
    if (!somain)
        somain = (addr_t)solist_get_somain();

    // Generate the name for an offset.
#define PARAM_OFFSET(type_, member_) __##type_##__##member_##__offset_
#define STRUCT_OFFSET PARAM_OFFSET
    int STRUCT_OFFSET(solist, next) = 0;
    for (size_t i = 0; i < 1024 / sizeof(void *); i++) {
        if (*(addr_t *)((addr_t)solist_head + i * sizeof(void *)) == somain) {
            STRUCT_OFFSET(solist, next) = i * sizeof(void *);
            break;
        }
    }

    linker_solist.push_back(solist_head);

    addr_t sonext = 0;
    sonext = *(addr_t *)((addr_t)solist_head + STRUCT_OFFSET(solist, next));
    while (sonext) {
        linker_solist.push_back((void *)sonext);
        sonext = *(addr_t *)((addr_t)sonext + STRUCT_OFFSET(solist, next));
    }

    return linker_solist;
}

char *linker_soinfo_get_realpath(soinfo_t soinfo) {
    static char *(*_get_realpath)(soinfo_t) = NULL;
    if (!_get_realpath)
        _get_realpath =(char *(*)(soinfo_t))resolve_elf_internal_symbol(get_android_linker_path(), "__dl__ZNK6soinfo12get_realpathEv");
    return _get_realpath(soinfo);
}


uintptr_t linker_soinfo_to_handle(soinfo_t soinfo) {
    static uintptr_t (*_linker_soinfo_to_handle)(soinfo_t) = NULL;
    if (!_linker_soinfo_to_handle)
        _linker_soinfo_to_handle =
                (uintptr_t(*)(soinfo_t))resolve_elf_internal_symbol(get_android_linker_path(), "__dl__ZN6soinfo9to_handleEv");
    return _linker_soinfo_to_handle(soinfo);
}



static inline bool check_symbol_version(const ElfW(Versym)* ver_table, uint32_t sym_idx,
                                        const ElfW(Versym) verneed) {
    if (ver_table == nullptr) return true;
    const uint32_t verdef = ver_table[sym_idx];
    return (verneed == kVersymNotNeeded) ?
           !(verdef & kVersymHiddenBit) :
           verneed == (verdef & ~kVersymHiddenBit);
}




void text(){
    LOGE("%s","");

    std::vector<soinfo_t> solist = linker_get_solist();
    for (auto soinfo : solist) {
        _soinfo * _si = (_soinfo *)soinfo;
        LOGE("%s",_si->get_realpath());
        LOGE("version_: %d",_si->version_);

        std::vector<std::string> list;
        if(_si->is_gnu_hash()){
            list = _si->gnu_lookupList();
        }else{
            continue;
        }
        for (std::string element : list) {
            LOGE("symbol: %s",element.c_str());
        }
        list.clear();
//        const ElfW(Sym)* s = nullptr;
//        if (image_name == NULL || strstr(linker_soinfo_get_realpath(soinfo), image_name) != 0) {
//            result = dlsym((void *)handle, symbol_name_pattern);
//            if (result)
//                return result;
//        }
    }

}
uint32_t calculate_elf_hash(const char* name) {
    const uint8_t* name_bytes = reinterpret_cast<const uint8_t*>(name);
    uint32_t h = 0, g;

    while (*name_bytes) {
        h = (h << 4) + *name_bytes++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }

    return h;
}


static std::pair<uint32_t, uint32_t> calculate_gnu_hash_simple(const char* name) {
    uint32_t h = 5381;
    const uint8_t* name_bytes = reinterpret_cast<const uint8_t*>(name);
#pragma unroll 8
    while (*name_bytes != 0) {
        h += (h << 5) + *name_bytes++; // h*33 + c = h + h * 32 + c = h + h << 5 + c
    }
    return { h, reinterpret_cast<const char*>(name_bytes) - name };
}


static inline std::pair<uint32_t, uint32_t> calculate_gnu_hash(const char* name) {
#if USE_GNU_HASH_NEON
    return calculate_gnu_hash_neon(name);
#else
    return calculate_gnu_hash_simple(name);
#endif
}





std::vector<std::string> _soinfo::gnu_lookupList() {

    std::vector<std::string> list ;
    uint32_t n = 1;

    int image_end = load_bias + size;

    do {
        ElfW(Sym) *s = symtab_ + n;
        const char *syn_name = get_string(s->st_name);
        if(syn_name == nullptr){
            LOGE("syn_name: null");
            continue;
        }
        bool is_symbol = true;
        for (int i=0;;i++) {
            if (!std::isalnum(syn_name[i]) && syn_name[i] != '_') {
                is_symbol = false;
            }
            if(syn_name[i] != '\0'){
                break;
            }
        }

        if(!is_symbol){
            continue;
        }
        LOGE("syn_name: %s",syn_name);
        list.push_back(syn_name);
    } while ((gnu_chain_[n++] & 1) == 0);
    return list;
}

