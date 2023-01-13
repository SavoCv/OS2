//
// Created by os on 6/8/22.
//

#ifndef _syscall_cpp
#define _syscall_cpp
#include "syscall_c.h"

void* operator new (size_t);

void operator delete (void*) noexcept;

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);
protected:
    Thread ();
    virtual void run () {}
private:
    thread_t myHandle;
    void (*body)(void*);
    void* arg;
    static void runWrapper(void *);
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
private:
    sem_t myHandle;
};

class PeriodicThread : public Thread {
protected:
    PeriodicThread (time_t period);
    virtual void periodicActivation () {}
private:
    static void periodicActivationWrapper(void*);
    time_t p;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif //PROJEKAT_SYSCALL_CPP_H
