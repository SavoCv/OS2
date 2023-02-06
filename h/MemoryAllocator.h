//
// Created by os on 5/3/22.
//

#ifndef PROJECT_BASE_MEMORYALLOCATOR_H
#define PROJECT_BASE_MEMORYALLOCATOR_H
#include "./../lib/hw.h"

class MemoryAllocator {
public:
    MemoryAllocator(const MemoryAllocator&) = delete;
    MemoryAllocator(MemoryAllocator&&) = delete;
    MemoryAllocator& operator=(const MemoryAllocator&) = delete;
    MemoryAllocator& operator=(MemoryAllocator&&) = delete;

    static MemoryAllocator* getAllocator();

    void* allocate(size_t vel);

    int free(void * ptr);

    static int get_size_of_additional_space() {
        return sizeof(Fragment);
    }

private:
    MemoryAllocator();

    struct Fragment
    {
        size_t sz;
        Fragment *next, *prev;
    };

    Fragment* first_free, *first_reserved;

    void*  operator new(size_t sz);

    static void link(Fragment *&list, Fragment* curr, Fragment* prev, Fragment* x);

    static void unlink(Fragment *& list, Fragment* curr, Fragment* prev);

    static void find(Fragment* list, Fragment *&curr, Fragment*& prev, Fragment* f);

    static void lower_bound(Fragment* list, Fragment *&curr, Fragment*& prev, size_t sz);

    static MemoryAllocator* ma;
};


#endif //PROJECT_BASE_MEMORYALLOCATOR_H
