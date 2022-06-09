//
// Created by os on 6/8/22.
//
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.h"

void * operator new(size_t n)
{
    return mem_alloc(n);
}

void *operator new[](size_t n)
{
    return mem_alloc(n);
}

void operator delete(void *p) noexcept
{
    mem_free(p);
}

void operator delete[](void *p) noexcept
{
    mem_free(p);
}

Thread::Thread (void (*body)(void*), void* arg) : body(body), arg(arg)
{
    myHandle = nullptr;
}

Thread::~Thread ()
{
    mem_free(myHandle);
}

int Thread::start ()
{
    return thread_create(&myHandle, body, arg);
}

void Thread::dispatch ()
{
    thread_dispatch();
}

int Thread::sleep (time_t t)
{
    return time_sleep(t);
}

Thread::Thread () {

}
