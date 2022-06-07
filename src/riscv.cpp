//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/TESTprint.hpp"
#include "../h/MemoryAllocator.h"

void* Riscv::sysStack = nullptr;
void* Riscv::initialSysStack = nullptr;
uint64* Riscv::processorContext = nullptr;

void Riscv::init()
{
    TCB::init();
    initialSysStack = sysStack = (void*) kmem_alloc(DEFAULT_STACK_SIZE);
    sysStack = (char*) sysStack + DEFAULT_STACK_SIZE;
    processorContext = TCB::running->context.processorContext;
    w_stvec((uint64) &supervisorTrap);
    //Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
}

void Riscv::handle_thread_dispatch() {
    TCB::timeSliceCounter = 0;
    TCB::dispatch();
}

void Riscv::handle_mem_alloc()
{
    //uint64 ra1 = 0;
    //__asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(ra1));
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    ma->allocate(TCB::running->context.processorContext[11]);
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    processorContext[10] = ra0;
}

void Riscv::handle_mem_free()
{
    //uint64 ra1 = 0;
    //__asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(ra1));
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    ma->free((void*) TCB::running->context.processorContext[11]);
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    processorContext[10] = ra0;
}

void Riscv::handle_thread_create() {
    thread_t* handle = (thread_t*) TCB::running->context.processorContext[11];
    void (*start_routine)(void*) = (void(*)(void*))TCB::running->context.processorContext[12];
    void *arg = (void*) TCB::running->context.processorContext[13];
    void *stack_space = (void*) TCB::running->context.processorContext[14];

    /*println("{");
    println((void*)handle);
    println((void*)start_routine);
    println((void*)stack_space);
    println((void*)arg);
    println("}");*/

    *handle = TCB::createThread(start_routine, stack_space, arg);
    uint64 ra0 = 0;
    if(handle)
        ra0 = 0;
    else
        ra0 = -16;
    processorContext[10] = ra0;
}

void Riscv::handle_thread_exit() {
    TCB::running->setFinished(true);
    TCB::dispatch();
}

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        w_sepc(r_sepc() + 4);
        TCB::running->context.sepc = r_sepc();
        uint64 sstatus = r_sstatus();
        uint64 ra0 = 0;
        /*TCB::timeSliceCounter = 0;
        TCB::dispatch();*/
        __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
        switch (ra0) {
            case 0x1:
                handle_mem_alloc();
                break;

            case 0x2:
                handle_mem_free();
                break;

            case 0X11:
                handle_thread_create();
                break;

            case 0X12:
                handle_thread_exit();
                break;

            case 0x13 :
                handle_thread_dispatch();
                break;


        }
        w_sstatus(sstatus);
        w_sepc(TCB::running->context.sepc);

    } else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            TCB::running->context.sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(TCB::running->context.sepc);
        }
        mc_sip(SIP_SSIP);
    } else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    } else
    {
        // unexpected trap cause
        uint64 sepc = r_sepc();
        printString("neocekivano: ");
        printInteger(scause);
        printString(" ");
        print((void*) r_sepc());
        printString("\n");
        w_sepc(sepc);
    }
}