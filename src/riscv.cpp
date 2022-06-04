//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/TESTprint.hpp"
#include "../h/MemoryAllocator.h"

void** Riscv::sys_stack = nullptr;
uint64** Riscv::user_stack = nullptr;

void Riscv::init()
{
    sys_stack = (void**) kmem_alloc(sizeof(void **));
    user_stack = (uint64**) (kmem_alloc(sizeof(uint64**)));
    *sys_stack = kmem_alloc(DEFAULT_STACK_SIZE);
    *sys_stack = (char*) *sys_stack + DEFAULT_STACK_SIZE - 1;
    w_stvec((uint64) &supervisorTrap);
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
}

void Riscv::handle_thread_dispatch() {
    TCB::timeSliceCounter = 0;
    TCB::dispatch();
}

void Riscv::handle_mem_alloc()
{
    uint64 ra1 = 0;
    __asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(ra1));
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    ma->allocate(ra1);
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    uint64 *x;
    x = *user_stack;
    x[10] = ra0;
}

void Riscv::handle_mem_free()
{
    uint64 ra1 = 0;
    __asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(ra1));
    MemoryAllocator *ma = MemoryAllocator::getAllocator();
    ma->free((void*) ra1);
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    uint64 *x;
    x = *user_stack;
    x[10] = ra0;
}

void Riscv::handle_thread_create() {
    thread_t* handle = nullptr;
    void (*start_routine)(void*);
    void *arg;
    void *stack_space;
    __asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(handle));
    __asm__ volatile("mv %[ra2], a2" : [ra2] "=r"(start_routine));
    __asm__ volatile("mv %[ra3], a3" : [ra3] "=r"(arg));
    __asm__ volatile("mv %[ra4], a4" : [ra4] "=r"(stack_space));
    *handle = TCB::createThread(start_routine, stack_space, arg);
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    uint64 *x;
    x = *user_stack;
    x[10] = ra0;
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

        uint64 sepc = r_sepc() + 4;
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
        w_sepc(sepc);

    } else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            uint64 sepc = r_sepc();
            uint64 sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
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