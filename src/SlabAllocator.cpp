//
// Created by os on 1/4/23.
//

#include "../h/SlabAllocator.h"
#include "../h/KConsole.h"
#include "../h/utils.h"

const int SlabAllocator::size_n_caches = 0;
SlabAllocator* SlabAllocator::ma = nullptr;

SlabAllocator *SlabAllocator::getAllocator() {
    //static SlabAllocator *ma = nullptr;
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
    last_cache->next = cache;
    last_cache = cache;
    if(size < 32)
        size = 32;
    if(cache)
        cache->init(name, size, ctor, dtor);
    return cache;
}

int SlabAllocator::shrink_cache(Cache* cache){
    return cache->shrink();
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
    for(int i = 0; i < cnt / 2; ++i)
    {
        char tmp = buff[i];
        buff[i] = buff[cnt - 1 - i];
        buff[cnt - 1 - i] = tmp;
    }
    buff[cnt] = 0;
    return cnt;
}

void SlabAllocator::strcpy(char* dst, const char* src)
{
    for(int i = 0; i == 0 || src[i - 1] != 0; ++i)
        dst[i] = src[i];
}

void* SlabAllocator::kmalloc(size_t size){
    int log_size = BuddyAllocator::getLogSize(size);
    size_t rs = 1 << log_size;
    if(rs < 32)
        rs = 32;

    static Cache *cache_for_names = SlabAllocator::create_cache("for_size-n_names", 20, nullptr, nullptr);
    char *name = (char*) alloc(cache_for_names);
            //MemoryAllocator::getAllocator()->allocate(20);
    if(name == nullptr)
        return nullptr;
    strcpy(name, "size-");
    itos(rs, name + 5);
    Cache *cache = create_cache(name, rs, nullptr, nullptr);
    free(cache_for_names, name);
    if(cache == nullptr) {
        KConsole::print("ERROR: SlabAllocator failed to allocate new cache\n");
        KConsole::console_handler();
        return nullptr;
    }
    void *tmp = cache->alloc();
    if(!tmp) {
        KConsole::print("ERROR: SlabAllocator failed to allocate from cache\n");
        KConsole::console_handler();
    }
    return tmp;
}

int SlabAllocator::kfree(const void* ptr){
    for(Cache* cache = cache_for_caches; cache != nullptr; cache=cache->next)
    {
        if(cache->name[0] == 's' &&
            cache->name[1] == 'i' &&
            cache->name[2] == 'z' &&
            cache->name[3] == 'e' &&
            cache->name[4] == '-')
            if(cache->try_free(ptr))
                return 0;
    }
    //error but can't signal in cache, only in cache for caches but that's not great solution
    return -1;
}

void SlabAllocator::delete_cache(Cache* cache){
//TODO
}

void SlabAllocator::print_cache_info(Cache* cache){
    cache->print_info();
}

int SlabAllocator::print_error_cache(Cache* cache){
    return cache->print_error();
}

void* kmem_alloc(size_t sz)
{
#ifdef DEBUG
    return MemoryAllocator::getAllocator()->allocate(sz);
#endif
    return SlabAllocator::getAllocator()->kmalloc(sz);
}

int kmem_free(void* ptr)
{
#ifdef DEBUG
    return MemoryAllocator::getAllocator()->free(ptr);
#endif
    return SlabAllocator::getAllocator()->kfree(ptr);
}

int SlabAllocator::shrink_all_caches() {
    int cnt = 0;
    for(Cache* curr = cache_for_caches; curr; curr = curr->next)
        cnt += curr->shrink();
    return cnt;
}
