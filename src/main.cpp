//
// Created by os on 6/3/22.
//
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"

void userMain();

void userMainWrapper(void*)
{
    userMain();
}

[[noreturn]] void idle(void*)
{
    while(true) {
        printString("idle dispatch\n");
        thread_dispatch();
    }
}

int main()
{
    Riscv::init();
    thread_t uMain, idle_t;
    thread_create(&uMain, userMainWrapper, nullptr);
    thread_create(&idle_t, idle, nullptr);
    while(!uMain->isFinished()) {
        printString("main dispatch\n");
        thread_dispatch();
    }

    printString("Kernel OS finished\n");
    idle_t->setFinished(true);
    thread_dispatch();

    return 0;
}