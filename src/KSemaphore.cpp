//
// Created by os on 6/7/22.
//
#include "../h/KSemaphore.hpp"

KSemaphore::KSemaphore(unsigned init) : val(init) {}

KSemaphore::~KSemaphore()
{
    TCB* t;
    deleted = 0;
    while( (t = blocked.removeFirst()) )
    {
        Scheduler::put(t);
        ++deleted;
    }
    while(deleted)
    {
        TCB::dispatch();
    }
}

int KSemaphore::wait()
{
    --val;
    if(val >= 0)
        return 0;
    else
    {
        blocked.addLast(TCB::running);
        TCB::dispatch_without_puting();
        if(deleted) {
            --deleted;
            return -4;
        }
        return 0;
    }
}

int KSemaphore::kwait()
{
    --val;
    if(val >= 0)
        return 0;
    else
    {
        blocked.addLast(TCB::running);
        TCB::k_dispatch_without_puting();
        if(deleted) {
            --deleted;
            return -4;
        }
        return 0;
    }
}

int KSemaphore::signal()
{
    ++val;
    if(val > 0)
        return 0;
    else
    {
        Scheduler::put(blocked.removeFirst());
        return 0;
    }
}

void *KSemaphore::operator new(size_t_ sz) {
    return kmem_alloc(sz);
}

void KSemaphore::operator delete(void * ptr)
{
    kmem_free(ptr);
}