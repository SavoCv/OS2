//
// Created by os on 6/3/22.
//
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"
#include "../h/KConsole.h"

void userMain();

void userMainWrapper(void*)
{
    userMain();
}

[[noreturn]] void idle(void*)
{
    while(true) {
        //printString("idle dispatch\n");
        thread_dispatch();
    }
}

int main()
{
    thread_t uMain, idle_t;
    Riscv::init(idle, idle_t);
    thread_create(&uMain, userMainWrapper, nullptr);
    while(!uMain->isFinished()) {
        //printString("main dispatch\n");
        thread_dispatch();
    }


    //printString("Kernel OS finished\n");
    idle_t->setFinished(true);
    thread_dispatch();

    return 0;
}