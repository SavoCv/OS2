//
// Created by os on 1/4/23.
//

#ifndef PROJEKAT_SLABALLOCATOR_H
#define PROJEKAT_SLABALLOCATOR_H
#include "Cache.h"
#include "BuddyAllocator.h"

void *kmem_alloc(size_t);

int kmem_free(void* );

class SlabAllocator {
public:
    const static int block_size = 4096;
    SlabAllocator(const SlabAllocator&) = delete;
    SlabAllocator(SlabAllocator&&) = delete;
    SlabAllocator& operator=(const SlabAllocator&) = delete;
    SlabAllocator& operator=(SlabAllocator&&) = delete;

    static SlabAllocator* getAllocator();

    void init(void* space, int block_num);

    Cache* create_cache(const char* name, int size, void (*ctor)(void*), void (*dtor)(void *));

    int shrink_cache(Cache* cache);

    void* alloc(Cache* cache);

    void* kmalloc(size_t size);

    int kfree(const void* ptr);

    void free(Cache* cache, void* ptr);

    void delete_cache(Cache* cache);

    void print_cache_info(Cache* cache);

    int print_error_cache(Cache* cache);

    int shrink_all_caches();

    static int strcmp(const char* c1, const char* c2);

    /**
     *
     * @param n int number bigger or equal than 0
     * @param buff where to store string containing converted int
     * @return how many digits was
     */
    static int itos(int n, char* buff);

    static void strcpy(char* dst, const char* src);
private:
    Cache* cache_for_caches, *last_cache;
    static const int size_n_caches;
    static SlabAllocator *ma;
};


#endif //PROJEKAT_SLABALLOCATOR_H
