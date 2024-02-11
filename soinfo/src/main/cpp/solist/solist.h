//
// Created by chic on 2023/6/2.
//

#ifndef THEPTAVPN_SOLIST_H
#define THEPTAVPN_SOLIST_H

#include <elf.h>
#include <link.h>

#include <memory>
#include <string>
#include <vector>
#include <regex>
#include "sotype.h"



void text();

typedef void (*linker_dtor_function_t)();
typedef void (*linker_ctor_function_t)(int, char**, char**);
#define FLAG_GNU_HASH    0x00000040
constexpr ElfW(Versym) kVersymNotNeeded = 0;
static constexpr ElfW(Versym) kVersymHiddenBit = 0x8000;
constexpr ElfW(Versym) kVersymGlobal = 1;

bool inline has_min_version(uint32_t min_version __unused)  {
#if defined(__work_around_b_24465209__)
    return (flags_ & FLAG_NEW_SOINFO) != 0 && version_ >= min_version;
#else
    return true;
#endif
}


struct soinfo_tls {
//    TlsSegment segment;
//    size_t module_id = kTlsUninitializedModuleId;
};



#if defined(__work_around_b_24465209__)
#define SOINFO_NAME_LEN 128
#endif

struct _soinfo {
#if defined(__work_around_b_24465209__)
    private:
  char old_name_[SOINFO_NAME_LEN];
#endif
public:
    const ElfW(Phdr)* phdr;
    size_t phnum;
#if defined(__work_around_b_24465209__)
    ElfW(Addr) unused0; // DO NOT USE, maintained for compatibility.
#endif
    ElfW(Addr) base;
    size_t size;

#if defined(__work_around_b_24465209__)
    uint32_t unused1;  // DO NOT USE, maintained for compatibility.
#endif

    ElfW(Dyn)* dynamic;

#if defined(__work_around_b_24465209__)
    uint32_t unused2; // DO NOT USE, maintained for compatibility
  uint32_t unused3; // DO NOT USE, maintained for compatibility
#endif

    _soinfo* next;
private:
    uint32_t flags_;

    const char* strtab_;
    ElfW(Sym)* symtab_;

    size_t nbucket_;
    size_t nchain_;
    uint32_t* bucket_;
    uint32_t* chain_;

#if !defined(__LP64__)
    ElfW(Addr)** unused4; // DO NOT USE, maintained for compatibility
#endif

#if defined(USE_RELA)
    ElfW(Rela)* plt_rela_;
    size_t plt_rela_count_;

    ElfW(Rela)* rela_;
    size_t rela_count_;
#else
    ElfW(Rel)* plt_rel_;
    size_t plt_rel_count_;

    ElfW(Rel)* rel_;
    size_t rel_count_;
#endif

    linker_ctor_function_t* preinit_array_;
    size_t preinit_array_count_;

    linker_ctor_function_t* init_array_;
    size_t init_array_count_;
    linker_dtor_function_t* fini_array_;
    size_t fini_array_count_;

    linker_ctor_function_t init_func_;
    linker_dtor_function_t fini_func_;

//#if defined(__arm__)     这里定义的arm是arm  架构，不是32位 还是64位，所以去掉
public:
    // ARM EABI section used for stack unwinding.
    uint32_t* ARM_exidx;
    size_t ARM_exidx_count;
private:
//#endif
    size_t ref_count_;
public:
    link_map link_map_head;

    bool constructors_called;

    // When you read a virtual address from the ELF file, add this
    // value to get the corresponding address in the process' address space.
    ElfW(Addr) load_bias;

#if !defined(__LP64__)
    bool has_text_relocations;
#endif
    bool has_DT_SYMBOLIC;


    uint32_t version_;

    // version >= 0
    dev_t st_dev_;
    ino_t st_ino_;

    // dependency graph
    soinfo_list_t children_;
    soinfo_list_t parents_;

    // version >= 1
    off64_t file_offset_;
    uint32_t rtld_flags_;
    uint32_t dt_flags_1_;
    size_t strtab_size_;

    // version >= 2

    size_t gnu_nbucket_;
    uint32_t* gnu_bucket_;
    uint32_t* gnu_chain_;
    uint32_t gnu_maskwords_;
    uint32_t gnu_shift2_;
    ElfW(Addr)* gnu_bloom_filter_;

    _soinfo* local_group_root_;

    uint8_t* android_relocs_;
    size_t android_relocs_size_;

    const char* soname_;
    std::string realpath_;

    const ElfW(Versym)* versym_;

    ElfW(Addr) verdef_ptr_;
    size_t verdef_cnt_;

    ElfW(Addr) verneed_ptr_;
    size_t verneed_cnt_;

    int target_sdk_version_;

    // version >= 3
    std::vector<std::string> dt_runpath_;
    android_namespace_t* primary_namespace_;
    android_namespace_list_t secondary_namespaces_;
    uintptr_t handle_;


    // version >= 4
    ElfW(Relr)* relr_;
    size_t relr_count_;

    // version >= 5
    std::unique_ptr<soinfo_tls> tls_;
//    std::vector<TlsDynamicResolverArg> tlsdesc_args_;


    const char* get_realpath() {
        if(realpath_.empty()){
            return "null";
        }
        return realpath_.c_str();
    }
    bool is_gnu_hash() const {
        return (flags_ & FLAG_GNU_HASH) != 0;
    }


    const char* get_string(ElfW(Word) index) const {
        if (index >= strtab_size_ && 0 >= strtab_size_ ) {
            return nullptr;
        }



        return strtab_ + index;
    }


    std::vector<std::string> gnu_lookupList();

    std::vector<std::string> elf_lookupList() {
        std::vector<std::string> list ;
        list.push_back("elf_lookupList");
//        uint32_t n = 1;
//        do {
//            ElfW(Sym)* s = symtab_ + n;
//            list.push_back(get_string(s->st_name));
//        } while ((gnu_chain_[n++] & 1) == 0);

        return list;
    }

    std::vector<std::string>  getFunctionList(){

        return is_gnu_hash() ? gnu_lookupList() : elf_lookupList();
    }
//    const ElfW(Sym)* find_symbol_by_name(SymbolName& symbol_name,
//                                                   const version_info* vi) const {
//            return is_gnu_hash() ? gnu_lookup(symbol_name, vi) : elf_lookup(symbol_name, vi);
//          }

    const ElfW(Versym)* get_versym_table() const {
        return has_min_version(2) ? versym_ : nullptr;
    }

};















#endif //THEPTAVPN_SOLIST_H
