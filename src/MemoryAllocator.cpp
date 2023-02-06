//
// Created by os on 5/3/22.
//

#include "../h/MemoryAllocator.h"
#include "../lib/hw.h"
#include "../h/KConsole.h"

MemoryAllocator* MemoryAllocator::ma = nullptr;

void* MemoryAllocator::operator new(size_t sz) {
    return (void*) HEAP_START_ADDR;
}
MemoryAllocator *MemoryAllocator::getAllocator() {
    if(!ma)
        ma = new MemoryAllocator();
    return ma;
}

MemoryAllocator::MemoryAllocator() {
    first_free = (Fragment*) ((char*)HEAP_START_ADDR + MEM_BLOCK_SIZE);
    first_free->next = nullptr;
    first_free->prev = nullptr;
    first_free->sz = (char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR - MEM_BLOCK_SIZE;
    first_reserved = nullptr;
}

void MemoryAllocator::link(Fragment *&list, Fragment* curr, Fragment* prev, Fragment* x)
{
    if(prev)
        prev->next = x;
    else
        list = x;
    x->next = curr;
    x->prev = prev;
    if(curr)
        curr->prev = x;
}

void MemoryAllocator::unlink(Fragment *& list, Fragment* curr, Fragment* prev){
    if(!curr)
        return;



    if(prev)
    {
        prev->next = curr->next;
        if(curr->next)
            prev->next->prev = prev;
    }
    else
    {
        list = curr->next;
        if(list)
            list->prev = nullptr;
    }


//    if(prev) {
//        if(curr) {
//            //curr->prev = prev;
//            prev->next = curr->next;
//        }
//        else {
//            prev->prev = nullptr;
//        }
//    }
//    else {
//        if(curr) {
//            curr->prev = prev;
//            list = curr->next;
//        }
//        else {
//            list = nullptr;
//        }
//    }
}

void *MemoryAllocator::allocate(size_t vel) {
    /*static int cnt = 1;
    ++cnt;
    if(cnt % 50 == 0) {
        int broj_frag = 0;
        for(Fragment * curr = first_free; curr; curr = curr->next)
            ++broj_frag;
        KConsole::print(" ");
        KConsole::print(broj_frag);
        KConsole::print(" ");
        broj_frag = 0;
        for(Fragment * curr = first_reserved; curr; curr = curr->next)
            ++broj_frag;
        KConsole::print(broj_frag);
        KConsole::print("\n");
    }//*/

    vel *= MEM_BLOCK_SIZE;//ne treba 2!!

    //best fit - trazenje najmanjeg fragmenta veceg ili jednakoh zadatoj velicini + sizeof(Fragment
    Fragment* curr = first_free, *prev = nullptr, *x;
    lower_bound(first_free, curr, prev, vel);
    if(!curr || curr->sz + sizeof(Fragment) < vel)
        return nullptr;

    //Ulancavanje u listu slobodnih fragmenata
    unlink(first_free, curr, prev);
    if(curr->sz - vel > sizeof(Fragment))
    {
        x = (Fragment*)((char*) curr + vel);
        x->sz = curr->sz - vel;

        Fragment *c, *p;
        lower_bound(first_free, c, p, x->sz);
        link(first_free, c, p, x);

        curr->sz = vel;
    }

    //Ulancavanje u listu alociranih fragmenata
    /*Fragment* i, * j;
    find(first_reserved, i, j, curr);
    link(first_reserved, i, j, curr);*/
    link(first_reserved, first_reserved, nullptr, curr);

    //print((void*)((char*) curr + sizeof(Fragment)));
    //printString(" ");
    //printInteger(vel);
    //printString("\n");
    return (char*) curr + sizeof(Fragment);
}

int MemoryAllocator::free(void *ptr) {
    //Trazenje fragmenta u listi alociranih fragmenata
    Fragment *i = first_reserved, * j = nullptr, *curr, *prev;
    //find(first_reserved, i, j, (Fragment *) ptr - 1);
    i = (Fragment *) ptr - 1;
    if(!i || !ptr || i + 1 != ptr || i < HEAP_START_ADDR || i + i->sz > HEAP_END_ADDR || i->sz > (uint64)HEAP_END_ADDR)
        return -1 ;//ERROR
    j = i->prev;
    unlink(first_reserved, i, j);

    //return 0; // for debug

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

void MemoryAllocator::find(Fragment *list, Fragment *&curr, Fragment *&prev, Fragment* f) {
    curr = list;
    prev = nullptr;
    for(; curr; prev = curr, curr = curr->next)
        if(curr == f)
            break;
}

void MemoryAllocator::lower_bound(Fragment *list, Fragment *&curr, Fragment *&prev, size_t sz) {
    curr = list;
    prev = nullptr;
    for(; curr; prev = curr, curr = curr->next)
        if(curr->sz >= sz)
            break;
}