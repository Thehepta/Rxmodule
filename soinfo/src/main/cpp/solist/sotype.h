//
// Created by chic on 2023/6/2.
//

#ifndef THEPTAVPN_SOTYPE_H
#define THEPTAVPN_SOTYPE_H



#include <string>
#include <vector>
#include <unordered_set>



template<typename T>
struct LinkedListEntry {
    LinkedListEntry<T>* next;
    T* element;
};


#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&) = delete;      \
    void operator=(const TypeName&) = delete


#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName() = delete;                           \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

// Android uses RELA for LP64.
#if defined(__LP64__)
#define USE_RELA 1
#endif


template<typename T, typename Allocator>
class LinkedList {

    struct LinkedListHeader {
        LinkedListEntry<T>* head;
        LinkedListEntry<T>* tail;
    };
    LinkedListHeader* header_;
    DISALLOW_COPY_AND_ASSIGN(LinkedList);
};


struct _soinfo;
struct android_namespace_t;
class SoinfoListAllocator {
public:
    static LinkedListEntry<_soinfo>* alloc();
    static void free(LinkedListEntry<_soinfo>* entry);

private:
    // unconstructable
    DISALLOW_IMPLICIT_CONSTRUCTORS(SoinfoListAllocator);
};

class NamespaceListAllocator {
public:
    static LinkedListEntry<android_namespace_t>* alloc();
    static void free(LinkedListEntry<android_namespace_t>* entry);

private:
    // unconstructable
    DISALLOW_IMPLICIT_CONSTRUCTORS(NamespaceListAllocator);
};

typedef LinkedList<_soinfo, SoinfoListAllocator> soinfo_list_t;
typedef LinkedList<android_namespace_t, NamespaceListAllocator> android_namespace_list_t;


struct android_namespace_link_t {

private:
    android_namespace_t* const linked_namespace_;
    const std::unordered_set<std::string> shared_lib_sonames_;
    bool allow_all_shared_libs_;
};



struct android_namespace_t {
private:
    std::string name_;
    bool is_isolated_;
    bool is_greylist_enabled_;
    bool is_also_used_as_anonymous_;
    std::vector<std::string> ld_library_paths_;
    std::vector<std::string> default_library_paths_;
    std::vector<std::string> permitted_paths_;
    std::vector<std::string> whitelisted_libs_;
    // Loader looks into linked namespace if it was not able
    // to find a library in this namespace. Note that library
    // lookup in linked namespaces are limited by the list of
    // shared sonames.
    std::vector<android_namespace_link_t> linked_namespaces_;
    soinfo_list_t soinfo_list_;

    DISALLOW_COPY_AND_ASSIGN(android_namespace_t);
};


#endif //THEPTAVPN_SOTYPE_H
