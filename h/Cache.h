//
// Created by os on 1/12/23.
//

#ifndef PROJEKAT_CACHE_H
#define PROJEKAT_CACHE_H
#include "SlabK.h"

#define MAX_LEN_NAME 20

class Cache {
    friend class SlabAllocator;
    Cache *next;
    SlabK* free, *full, *partly;
    char name[MAX_LEN_NAME];
    int object_size;
    int size;
    int num_of_slabs;
    int size_n; // size_n je promeljiva koja kaze koliko je veliki slab 2 ^ slab_k
    void (*ctor)(void*);
    void (*dtor)(void*);
    int err;
    int last_shrink_num_slabs;
    int num_of_objects;
public:
    void init(const char* na, int o_sz, void (*ctor)(void*) = nullptr, void (*dtor)(void*) = nullptr);

    void add_slab(SlabK* sk);

    void* alloc();

    void shrink();

    bool try_free(void * obj);

    void print_info();

    int print_error();
};

typedef Cache kmem_cache_s;


#endif //PROJEKAT_CACHE_H
