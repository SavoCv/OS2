//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"
#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/MemoryAllocator.h"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

int TCB::init(Body body, uint64 timeSlice, void* stack_space, void* arg, bool p)

{
    this->body = body;
    stack = body != nullptr ? (uint64*) stack_space : nullptr;
    this->timeSlice = timeSlice;
    this->arg = arg;
    finished = false;
    //print(stack_space);
    //printString(" - \n");
    if (body != nullptr) {
        Scheduler::put(this);
        context.sepc = (uint64) &threadWrapper;
    }

    context.processorContext = (uint64*) kmem_alloc(32 * sizeof(uint64));
    if(context.processorContext == nullptr)
        return -1; //Exception
    if(body != nullptr)
        for(int i = 0; i < 32; ++i)
            context.processorContext[i] = running->context.processorContext[i];
    context.processorContext[2] = (uint64) stack_space;
    context.sstatus = Riscv::r_sstatus();
    if(p)
        context.sstatus &= ~Riscv::SSTATUS_SPP;
    return 0;
}

void TCB::init()
{
    running = createThread(nullptr, nullptr, nullptr);
}

TCB *TCB::createThread(Body body, void* stack_space, void* param)
{
    TCB *t = (TCB*)kmem_alloc(sizeof(TCB));
    if(t == nullptr)
        return t;
    if(t->init(body, TIME_SLICE, stack_space, param) != 0){
        kmem_free(t);
        return nullptr;
    }
            //new TCB(body, TIME_SLICE, stack_space, param);
    if(t == nullptr || t->context.processorContext == nullptr)
        return nullptr;
    return t;
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
    TCB* tmp = running; // useless just for easier debug
    TCB::contextSwitch(&old->context, &tmp->context);
}

void TCB::dispatch_without_puting()
{
    TCB *old = running;
    running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);
}

void TCB::k_dispatch_without_puting()
{
    TCB *old = running;
    __asm__ volatile("mv %[x], ra" : [x] "=r"(old->context.sepc));
    running = Scheduler::get();
    TCB::contextSwitch(&old->context, &running->context);
    Riscv::w_sstatus(TCB::running->context.sstatus);
    Riscv::w_sepc(TCB::running->context.sepc);
    Riscv::processorContext = TCB::running->context.processorContext;
    Riscv::returnFromInerrupt();
}

void TCB::threadWrapper()
{
    //TODO some things
    //Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    TCB::yield();
}

/*void *TCB::operator new(size_t_ sz) {
    return kmem_alloc(sz);
}

void TCB::operator delete(void * ptr)
{
    kmem_free(ptr);
}*/

void TCB::contextSwitch(TCB::Context *oldContext, TCB::Context *runningContext) {
    oldContext->sepc = Riscv::r_sepc();
    //oldContext->processorContext = Riscv::processorContext; // Cak ni ne mora
    Riscv::processorContext = runningContext->processorContext;
    Riscv::w_sepc(runningContext->sepc);
}