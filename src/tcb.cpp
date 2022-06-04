//
// Created by marko on 20.4.22..
//

#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::createThread(Body body, void* stack_space, void* param)
{
    return new TCB(body, TIME_SLICE, stack_space, param);
}

void TCB::yield()
{
    __asm__ volatile("addi a0, zero, 0x13");
    __asm__ volatile ("ecall");
}

void TCB::dispatch()
{
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

void *TCB::operator new(size_t sz) {
    return kmem_alloc(sz);
}

void TCB::operator delete(void * ptr)
{
    kmem_free(ptr);
}

void TCB::init()
{
    running = createThread(nullptr, nullptr, nullptr);
}