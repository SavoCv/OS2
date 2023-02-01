//
// Created by os on 1/12/23.
//

#include "../h/Cache.h"
#include "../h/BuddyAllocator.h"
#include "../h/KConsole.h"

#define MIN_OBJECTS_IN_CACHE 8

#define println(X) (KConsole::print(X), KConsole::print("\n"))

void Cache::init(const char* na, int o_sz, void (*ct)(void*), void (*dt)(void*))
{
    next = nullptr;
    free=full=partly=nullptr;
    err = 0;
    for(int i = 0; i == 0 || na[i - 1] != 0; ++i)
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
    ctor = ct;
    dtor = dt;
    last_shrink_num_slabs = 10000;
    num_of_objects = 0;
}

void Cache::add_slab(SlabK* sk)
{
    ++num_of_slabs;
    size += sk->size;
    SlabK **tmp;
    if(sk->get_free_cnt() == 0)
        tmp = &full;
        //full = full->next = sk;
    else
        if(sk->get_cnt() == 0)
            tmp = &free;
            //free = free->next = sk;
        else
            tmp = &partly;
            //partly = partly->next = sk;
    if(*tmp == nullptr)
    {
        *tmp = sk;
        sk->next = nullptr;
    }
    else
    {
        sk->next = *tmp;
        *tmp = sk;
    }
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
            if(free) {
                free->init(free, (1 << size_n) * BLOCK_SIZE, object_size);
                num_of_slabs++;
                size += (1 << size_n) * BLOCK_SIZE;
            }

        }
        if (free) {
            ret = free->alloc();

            SlabK *tmp = free->next;
            free->next = partly;
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

int Cache::shrink()
{
    int cnt = 0;
    if(num_of_slabs <= last_shrink_num_slabs) // moze biti samo jednako ili vece
    {
        for(SlabK* sk = free; sk != nullptr;) {
            SlabK *tmp = sk->next;
            size -= sk->size;
            BuddyAllocator::getAllocator()->free(sk, size_n);
            sk = tmp;
            num_of_slabs--;
            cnt++;
        }
        free = nullptr;
    }
    return cnt;
}

bool Cache::try_free(const void *obj)
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
        if(t->get_cnt() == 0){
            if(p)
                p->next = t->next;
            else
                partly = t->next;
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
    println("---Cache---");
    KConsole::print("Name: ");
    println(name);
    KConsole::print("Object size [Byte]: ");
    println(object_size);
    KConsole::print("Size [BLOCKS]:");
    println(size/BLOCK_SIZE);
    KConsole::print("Num of slabs: ");
    println(num_of_slabs);
    KConsole::print("Num of objects: ");
    KConsole::print(num_of_objects);
    KConsole::print("\n");
    KConsole::print("Capacity of slab: ");
    SlabK *p = full;
    if(!p)
        p = partly;
    if(!p)
        p = free;
    if(!p)
        println("not calculated yet");
    else
        println(p->kap);
    KConsole::print("Percentage of used: ");
    if(!p)
        println("not calculated yet");
    else
    {
        KConsole::print(100 * num_of_objects / p->kap / num_of_slabs);
        KConsole::putc('.');
        int tmp = (100 * num_of_objects) % (p->kap * num_of_slabs) * 100 / (p->kap * num_of_slabs);
        if(tmp < 10)
            KConsole::putc(0);
        println(tmp);
    }
    println("-----------");
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
        case 1: println("Name is too long"); break;
        case 2: println("There are no enough memory"); break;
    }
    return -err;
}
