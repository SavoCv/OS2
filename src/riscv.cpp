//
// Created by marko on 20.4.22..
//

#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/MemoryAllocator.h"
#include "../h/KSemaphore.hpp"
#include "../h/KConsole.h"
#include "../h/syscall_c.h"

void* Riscv::sysStack = nullptr;
void* Riscv::initialSysStack = nullptr;
uint64* Riscv::processorContext = nullptr;
SleepList Riscv::sl;

void Riscv::init(void (*idle)(void* ), thread_t& idle_t)
{
    TCB::init();
    initialSysStack = sysStack = (void*) kmem_alloc(DEFAULT_STACK_SIZE);
    sysStack = (char*) sysStack + DEFAULT_STACK_SIZE;
    processorContext = TCB::running->context.processorContext;
    w_stvec((uint64) &supervisorTrap);
    thread_create(&idle_t, idle, nullptr);
    KConsole::init();
    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);
}

void Riscv::handle_thread_dispatch() {
    TCB::timeSliceCounter = 0;
    TCB::dispatch();
}

void Riscv::handle_mem_alloc()
{
    //uint64 ra1 = 0;
    //__asm__ volatile("mv %[ra1], a1" : [ra1] "=r"(ra1));
    /*static int cnt = 1;
    ++cnt;
    if(cnt % 50 == 0)
        KConsole::print("handle mem alloc\n");*/
    /*uint64 ra0_v1 = (uint64)kmem_alloc(TCB::running->context.processorContext[11]);
    processorContext[10] = ra0_v1;*/
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

void Riscv::handle_sem_create()
{
    sem_t *handle = (sem_t *)processorContext[11];
    *handle = new KSemaphore((unsigned) processorContext[12]);
    processorContext[10] = 0;
}

void Riscv::handle_sem_close()
{
    sem_t handle = (sem_t)processorContext[11];
    delete handle;
    processorContext[10] = 0;
}

void Riscv::handle_sem_wait()
{
    sem_t handle = (sem_t)processorContext[11];
    processorContext[10] = handle->wait();
}

void Riscv::handle_sem_signal()
{
    sem_t handle = (sem_t)processorContext[11];
    processorContext[10] = handle->signal();
}

void Riscv::handle_time_sleep()
{
    time_t time = processorContext[11];
    sl.add(new TCBAndTime(TCB::running, time));
    TCB::dispatch_without_puting();
    processorContext[10] = 0;
}

void Riscv::handle_getc()
{
    processorContext[10] = KConsole::getc();
}

void Riscv::handle_putc()
{
    KConsole::putc((char)processorContext[11]);
}

void Riscv::handleSupervisorTrap()
{
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL)
    {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        w_sepc(r_sepc() + 4);
        TCB::running->context.sepc = r_sepc();
        TCB::running->context.sstatus = r_sstatus();
        volatile uint64 ra0 = 0;
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

            case 0x21:
                handle_sem_create();
                break;

            case 0x22:
                handle_sem_close();
                break;

            case 0x23:
                handle_sem_wait();
                break;

            case 0x24:
                handle_sem_signal();
                break;

            case 0x31:
                handle_time_sleep();
                break;

            case 0x41:
                handle_getc();
                break;

            case 0x42:
                handle_putc();
                break;
        }
        w_sstatus(TCB::running->context.sstatus);
        w_sepc(TCB::running->context.sepc);

    } else if (scause == 0x8000000000000001UL)
    {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        TCB::timeSliceCounter++;
        TCBAndTime *tat;
        tat = sl.peekFirst();
        if(tat)
            --tat->d;
        while(tat && tat->d == 0)
        {
            tat = sl.removeFirst();
            if(!tat->t->isFinished())
                Scheduler::put(tat->t);
            delete tat;
            tat = sl.peekFirst();
        }
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice())
        {
            TCB::running->context.sepc = r_sepc();
            TCB::running->context.sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(TCB::running->context.sstatus);
            w_sepc(TCB::running->context.sepc);
        }
        mc_sip(SIP_SSIP);
    } else if (scause == 0x8000000000000009UL)
    {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        KConsole::console_handler();
    } else if (scause == 0x2)
    {
        KConsole::print("GRESKA: ilegalna instrukcija:\nsepc: ");
        KConsole::print_hex((long)r_sepc());
        KConsole::print("\nstval: ");
        KConsole::print_hex((long)r_stval());
        KConsole::print("\n");
        KConsole::console_handler();
        handle_thread_exit();
    } else
    {
        // unexpected trap cause
        uint64 sepc = r_sepc();
        KConsole::print("ERROR: Unexpected trap cause: scause=");
        KConsole::print(scause);
        KConsole::print(", sepc=");
        KConsole::print_hex(sepc);
        KConsole::print("\nstval: ");
        uint64 stval = (long)r_stval();
        KConsole::print_hex(stval);
        KConsole::print("\n");
        KConsole::console_handler();
        w_sepc(sepc);
    }
}