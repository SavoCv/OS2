//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/TESTprint.hpp"

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
    if (!old->isFinished()) {
        printString("<");
        println((void*)old);
        Scheduler::put(old);
        printString(">");
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    //Riscv::popSppSpie();
    TCB *t = running;
    t += 0;
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

void *TCB::operator new(size_t_ sz) {
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

void TCB::contextSwitch(TCB::Context *oldContext, TCB::Context *runningContext) {
    oldContext->sepc = Riscv::r_sepc();
    oldContext->sp = Riscv::user_stack;
    Riscv::user_stack = runningContext->sp;
    Riscv::w_sepc(runningContext->sepc);
}
