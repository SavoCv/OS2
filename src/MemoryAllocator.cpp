//
// Created by os on 5/3/22.
//

#include "../h/MemoryAllocator.h"
#include "../lib/hw.h"

void* MemoryAllocator::operator new(size_t sz) {
    return (void*) HEAP_START_ADDR;
}
MemoryAllocator *MemoryAllocator::getAllocator() {
    static MemoryAllocator *ma = nullptr;
    if(!ma)
        ma = new MemoryAllocator();
    return ma;
}

MemoryAllocator::MemoryAllocator() {
    first_free = (Fragment*) ((char*)HEAP_START_ADDR + sizeof(MemoryAllocator));
    first_free->next = nullptr;
    first_free->sz = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR;
    first_reserved = nullptr;
}

void MemoryAllocator::link(Fragment *&list, Fragment* curr, Fragment* prev, Fragment* x)
{
    if(prev)
        prev->next = x;
    else
        list = x;
    x->next = curr;
}

void MemoryAllocator::unlink(Fragment *& list, Fragment* curr, Fragment* prev){
    if(prev)
        prev->next = curr->next;
    else
        list = curr->next;
}

void *MemoryAllocator::allocate(size_t vel) {
    vel *= MEM_BLOCK_SIZE;

    //best fit - trazenje najmanjeg fragmenta veceg ili jednakoh zadatoj velicini + sizeof(Fragment)
    Fragment* curr = first_free, *prev = nullptr, *x;
    lower_bound(first_free, curr, prev, vel);
    if(!curr || curr->sz < vel)
        return nullptr;

    //Ulancavanje u listu slobodnih fragmenata
    unlink(first_free, curr, prev);
    if(curr->sz - vel >= sizeof(Fragment))
    {
        x = (Fragment*)((char*) curr + vel);
        x->sz = curr->sz - vel;

        Fragment *c, *p;
        lower_bound(first_free, c, p, x->sz);
        link(first_free, c, p, x);

        curr->sz = vel;
    }

    //Ulancavanje u listu alociranih fragmenata
    Fragment* i, * j;
    lower_bound(first_reserved, i, j, curr);
    link(first_reserved, i, j, curr);

    return (char*) curr + sizeof(Fragment);
}

int MemoryAllocator::free(void *ptr) {
    //Trazenje fragmenta u listi alociranih fragmenata
    Fragment *i = first_reserved, * j = nullptr, *curr, *prev;
    lower_bound(first_reserved, i, j, (Fragment*)ptr - 1);
    if(!i || i + 1 != ptr)
        return -1 ;//ERROR
    unlink(first_reserved, i, j);

    //Spajenje novo-slobodnog fragmentima ispred i/ili iza ako moze i izlancavanje istih iz liste slobodnih
    curr = first_free;
    prev = nullptr;
    int count = 0;
    while(curr != nullptr) {
        if ((char *) i + i->sz == (char *) curr || (char *) curr + curr->sz == (char *) i) {
            unlink(first_free, curr, prev);

            if ((char *) i + i->sz == (char *) curr) // i < curr
                i->sz += curr->sz;
            else {
                curr->sz += i->sz;
                i = curr;
            }

            if (++count == 2)
                break;

            if (prev)
                curr = prev->next;
            else
                curr = first_free;
        } else {
            prev = curr;
            curr = curr->next;
        }
    }

    //Ulancavanje novog fragmenta u listu slobodnih
    lower_bound(first_free, curr, prev, i->sz);
    link(first_free, curr, prev, i);
    return 0;
}

void MemoryAllocator::lower_bound(Fragment *list, Fragment *&curr, Fragment *&prev, Fragment* f) {
    curr = list;
    prev = nullptr;
    for(; curr; prev = curr, curr = curr->next)
        if(curr >= f)
            break;
}

void MemoryAllocator::lower_bound(Fragment *list, Fragment *&curr, Fragment *&prev, size_t sz) {
    curr = list;
    prev = nullptr;
    for(; curr; prev = curr, curr = curr->next)
        if(curr->sz >= sz)
            break;
}

void *kmem_alloc(size_t sz)
{
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    return ma->allocate((sz - 1 + ma->get_size_of_additional_space()) / MEM_BLOCK_SIZE + 1);
}

int kmem_free(void* ptr)
{
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    return ma->free(ptr);
}