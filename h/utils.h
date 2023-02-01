//
// Created by os on 1/15/23.
//

#ifndef PROJEKAT_UTILS_H
#define PROJEKAT_UTILS_H

//#define DEBUG

#ifdef DEBUG

//for debug
//#define kmem_alloc(x) MemoryAllocator::getAllocator()->allocate(x/64 + 1)
//#define kmem_free(x) MemoryAllocator::getAllocator()->free(x)

#endif

#ifdef __cplusplus
extern "C" {
#endif

    class Cache;
    using size_t = decltype(sizeof(int));
    using kmem_cache_t = Cache;

#ifdef __cplusplus
}
#endif


#endif //PROJEKAT_UTILS_H
