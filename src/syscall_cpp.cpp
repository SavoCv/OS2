//
// Created by os on 6/8/22.
//
#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"

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

void Thread::runWrapper(void * v) {
    ((Thread*) v)->run();
}

Thread::Thread () : Thread(runWrapper, this) {}



Semaphore::Semaphore (unsigned init)
{
    sem_open(&myHandle, init);
}

Semaphore::~Semaphore ()
{
    mem_free(myHandle);
}

int Semaphore::wait ()
{
    return sem_wait(myHandle);
}

int Semaphore::signal ()
{
    return sem_signal(myHandle);
}


/*class PeriodicThread : public Thread {
protected:*/
PeriodicThread::PeriodicThread (time_t period) : Thread(periodicActivationWrapper, this)
{
    p = period;
}

void PeriodicThread::periodicActivationWrapper(void * a) {
    while(true) {
        ((PeriodicThread *) a)->periodicActivation();
        time_sleep(((PeriodicThread *) a)->p);
    }
}


char Console::getc ()
{
    return ::getc();
}

void Console::putc (char c)
{
    ::putc(c);
}
