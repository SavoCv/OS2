//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP

#include "../lib/hw.h"
#include "scheduler.hpp"
#include "MemoryAllocator.h"
#include "riscv.hpp"

using size_t_ = decltype(sizeof(0));

// Thread Control Block
class TCB
{
public:
    static void init();

    ~TCB() { delete[] stack; }

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* stack_space, void*);

    static void yield();

    static void dispatch();

    static void dispatch_without_puting();

    static void k_dispatch_without_puting();

    static TCB *running;

    void*  operator new(size_t_ sz);

    void operator delete(void * ptr);

private:
    TCB(Body body, uint64 timeSlice, void* stack_space, void* arg, bool p = false);

    struct Context
    {
        uint64 sepc;
        uint64* processorContext;
        uint64 sstatus;
    };

    Body body;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    void *arg;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static uint64 timeSliceCounter;

    static uint64 constexpr TIME_SLICE = 2;
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_TCB_HPP
