//
// Created by os on 1/4/23.
//

#include "../h/SlabAllocator.h"

SlabAllocator *SlabAllocator::getAllocator() {
    static SlabAllocator *ma = nullptr;
    if(!ma)
        ma = (SlabAllocator*) MemoryAllocator::getAllocator()->allocate(sizeof(SlabAllocator));
    return ma;
}

void SlabAllocator::init(void* space, int block_num)
{
    BuddyAllocator::getAllocator()->init(space, block_num);

    SlabK* slab = (SlabK*) BuddyAllocator::getAllocator()->allocate(size_n_caches);
    slab->init(slab, (1 << size_n_caches) * BLOCK_SIZE, sizeof(Cache));

    cache_for_caches = (Cache*) slab->alloc();
    cache_for_caches->init("cache_for_caches", sizeof(Cache));
    cache_for_caches->add_slab(slab);
    cache_for_caches->num_of_objects = 1;
    last_cache = cache_for_caches;
}

int SlabAllocator::strcmp(const char* c1, const char* c2)
{
    int i;
    for(i = 0; c1[i] && c2[i];++i)
        if(c1[i] < c2[i])
            return -1;
        else
            if(c1[i] > c2[i])
                return 1;
    if(!c1[i] && !c2[i])
        return 0;
    if(!c1[i])
        return -1;
    return 1;
}

Cache* SlabAllocator::create_cache(const char* name, int size, void (*ctor)(void*), void (*dtor)(void *)){
    Cache *curr;
    for(curr = cache_for_caches; curr != nullptr; curr = curr->next)
        if(!strcmp(curr->name, name))
            break;
    if(curr != nullptr)
        return curr;
    Cache* cache = (Cache*) cache_for_caches->alloc();
    cache->init(name, size, ctor, dtor);
    return cache;
}

void SlabAllocator::shrink_cache(Cache* cache){
    cache->shrink();
}

void* SlabAllocator::alloc(Cache* cache){
    return cache->alloc();
}

void SlabAllocator::free(Cache* cache, void* ptr){
    cache->try_free(ptr);
}

int SlabAllocator::itos(int n, char* buff){
    if(n == 0)
    {
        buff[0] = '0';
        buff[1] = 0;
        return 1;
    }
    int cnt = 0;
    while(n)
    {
        buff[cnt++] = '0' + n % 10;
        n /= 10;
    }
    buff[cnt] = 0;
    return cnt;
}

void SlabAllocator::strcpy(char* dst, const char* src)
{
    for(int i = 0; src[i] != 0; ++i)
        dst[i] = src[i];
}

void* SlabAllocator::kmalloc(size_t size){
    int log_size = BuddyAllocator::getLogSize(size);
    size_t rs = 1 << log_size;

    char *name = (char*) MemoryAllocator::getAllocator()->allocate(20);
    strcpy(name, "size-");
    itos(rs, name + 5);
    Cache *cache = create_cache(name, rs, nullptr, nullptr);
    if(cache == nullptr)
        return nullptr;
    return cache->alloc();
}

void SlabAllocator::kfree(void* ptr){
    for(Cache* cache = cache_for_caches; cache != nullptr; cache=cache->next)
    {
        if(cache->name[0] == 's' &&
            cache->name[1] == 'i' &&
            cache->name[2] == 'z' &&
            cache->name[3] == 'e' &&
            cache->name[4] == '-')
            if(cache->try_free(ptr))
                return;
    }
    //error
}

void SlabAllocator::delete_cache(Cache* cache){
//TODO
}

void SlabAllocator::print_cache_info(Cache* cache){
//TODO
}

int SlabAllocator::print_error_cache(Cache* cache){
//TODO
    return 0;
}

