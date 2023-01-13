//
// Created by os on 1/12/23.
//

#include "../h/SlabK.h"

void SlabK::init(void* space, int size_, int obj_sz) // space je isti pokazivac kao this
{
    size = size_;
    next = nullptr;
    cnt = 0;
    objects = (SlabK*)space + 1;
    kap = (size - sizeof(SlabK)) / obj_sz;
    object_size = obj_sz;
    void* curr = objects, *nxt;
    for(int i = 0; i < kap - 1; ++i) {
        nxt = (char*)curr + obj_sz;
        *(void **) curr = nxt;
        curr = nxt;
    }
    *(void **) curr = nullptr;
}

void* SlabK::alloc()
{
    if(objects == nullptr)
        return nullptr;
    void* tmp = objects;
    objects = *(void**)objects;
    ++cnt;
    return tmp;
}

bool SlabK::try_free(void *obj, void (*dtor)(void*)) {
    if(obj < (void*) this || obj >= ((char*) this + size))
        return false;
   if(dtor)
        dtor(obj);
    *(void**)obj = objects;
    objects = obj;
    --cnt;
    return true;
}