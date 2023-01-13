//
// Created by os on 1/12/23.
//

#include "../h/Cache.h"
#include "../h/BuddyAllocator.h"
#include "../h/KConsole.h"

#define MIN_OBJECTS_IN_CACHE 8

void Cache::init(const char* na, int o_sz, void (*ct)(void*), void (*dt)(void*))
{
    next = nullptr;
    free=full=partly=nullptr;
    err = 0;
    for(int i = 0; na[i - 1] != 0; ++i)
        if(i < MAX_LEN_NAME)
            name[i] = na[i];
        else {
            err = 1;
            break;
        }
    name[MAX_LEN_NAME - 1] = 0;
    object_size = o_sz;
    size = 0;
    num_of_slabs = 0;
    int l_o_sz = BuddyAllocator::getLogSize(8 * o_sz/BLOCK_SIZE);
    size_n = l_o_sz;
    ctor = nullptr;
    dtor = nullptr;
    last_shrink_num_slabs = 10000;
    num_of_objects = 0;
}

void Cache::add_slab(SlabK* sk)
{
    ++num_of_slabs;
    size += sk->size;
    if(sk->get_free_cnt() == 0)
        full = full->next = sk;
    else
        if(sk->get_cnt() == 0)
            free = free->next = sk;
        else
            partly = partly->next = sk;
}

void* Cache::alloc()
{
    void *ret = nullptr;
    if(partly)
    {
        ret = partly->alloc();
        if(partly->get_free_cnt() == 0)
        {
            SlabK *tmp = partly->next;
            partly->next = full;
            full = partly;
            partly = tmp;
        }
    }
    else {
        if(!free) {
            free = (SlabK *) BuddyAllocator::getAllocator()->allocate(size_n);
            if(free)
                free->init(free, 1 << size_n, object_size);
        }
        if (free) {
            ret = free->alloc();

            SlabK *tmp = free->next;
            free->next = full;
            partly = free;
            free = tmp;
        }
        else
        {
            err = 2;
        }
    }
    if(ret && ctor)
        ctor(ret);
    if(ret)
        num_of_objects++;
    return ret;
}

void Cache::shrink()
{
    if(num_of_slabs <= last_shrink_num_slabs) // moze biti samo jednako ili vece
    {
        for(SlabK* sk = free; sk != nullptr;) {
            SlabK *tmp = sk->next;
            size -= sk->size;
            BuddyAllocator::getAllocator()->free(sk, size_n);
            sk = tmp;
            num_of_slabs--;
        }
        free = nullptr;
    }
}

bool Cache::try_free(void * obj)
{
    SlabK *t, *p = nullptr;
    for(t = full; t != nullptr; p = t, t = t->next)
    {
        if(t->try_free(obj, dtor))
            break;
    }
    if(t != nullptr)
    {
        if(p)
            p->next = t->next;
        else
            full = t->next;
        t->next = partly;
        partly = t;
        num_of_objects--;
        return true;
    }

    p = nullptr;
    for(t = partly; t != nullptr; p = t, t = t->next)
        if(t->try_free(obj, dtor))
            break;
    if(t != nullptr)
    {
        if(p)
            p->next = t->next;
        else
            partly = t->next;
        if(t->get_cnt() == 0) {
            t->next = free;
            free = t;
        }
        num_of_objects--;
        return true;
    }
    return false;
}

void Cache::print_info()
{
    KConsole::println("---Cache---");
    KConsole::print("Name: ");
    KConsole::println(name);
    KConsole::print("Object size [Byte]: ");
    KConsole::println(object_size);
    KConsole::print("Size [BLOCKS]:");
    KConsole::println(size/BLOCK_SIZE);
    KConsole::print("Num of slabs: ");
    KConsole::println(num_of_slabs);
    KConsole::print("Num of objects in slab: ");
    SlabK *p = full;
    if(!p)
        p = partly;
    if(!p)
        p = free;
    if(!p)
        KConsole::println("not calculated yet");
    else
        KConsole::println(p->kap);
    KConsole::print("Percentage of used: ");
    if(!p)
        KConsole::println("not calculated yet");
    else
    {
        KConsole::print(100 * num_of_objects / p->kap / num_of_slabs);
        KConsole::putc('.');
        int tmp = (100 * num_of_objects) % (p->kap * num_of_slabs) * 100 / (p->kap * num_of_slabs);
        if(tmp < 10)
            KConsole::putc(0);
        KConsole::println(tmp);
    }
    KConsole::println("-----------");
    KConsole::putc('\n');

    /*KConsole::print("Num of objects in slabs: ");
    for(SlabK* curr = full; curr; curr = curr->next) {
        KConsole::print(curr->cnt);
    }*/
}

int Cache::print_error()
{
    switch(err)
    {
        case 1: KConsole::println("Name is too long"); break;
        case 2: KConsole::println("There are no enough memory"); break;
    }
    return -err;
}
