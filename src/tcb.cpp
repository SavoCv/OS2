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

TCB::TCB(Body body, uint64 timeSlice, void* stack_space, void* arg) :
    body(body),
    stack(body != nullptr ? (uint64*) stack_space : nullptr),
    timeSlice(timeSlice),
    arg(arg),
    finished(false)
{
    //print(stack_space);
    //printString(" - \n");
    if (body != nullptr) {
        Scheduler::put(this);
        context.sepc = (uint64) &threadWrapper;
    }

    context.processorContext = (uint64*) kmem_alloc(32 * sizeof(uint64));
    context.processorContext[2] = (uint64) stack_space;
}

void TCB::init()
{
    running = createThread(nullptr, nullptr, nullptr);
}

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
        //printString("<");
        //println((void*)old);
        Scheduler::put(old);
        //printString(">");
    }
    running = Scheduler::get();

    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper()
{
    //TODO some things
    //Riscv::popSppSpie();
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

void TCB::contextSwitch(TCB::Context *oldContext, TCB::Context *runningContext) {
    oldContext->sepc = Riscv::r_sepc();
    //oldContext->processorContext = Riscv::processorContext; // Cak ni ne mora
    Riscv::processorContext = runningContext->processorContext;
    Riscv::w_sepc(runningContext->sepc);
}
