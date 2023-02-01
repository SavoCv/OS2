//
// Created by os on 6/3/22.
//
#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"
#include "../h/KConsole.h"
#include "../h/SlabAllocator.h"

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

    void *space = nullptr;
    long block_num = ((char*) HEAP_END_ADDR - (char*)HEAP_START_ADDR) / 8 / MEM_BLOCK_SIZE;
    block_num /= SlabAllocator::block_size;
    space = MemoryAllocator::getAllocator()->allocate(block_num * SlabAllocator::block_size);
    SlabAllocator::getAllocator()->init(space, block_num * MEM_BLOCK_SIZE);

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

//int main()
//{
//    KConsole::init();
//    KConsole::putc('A');
//    KConsole::console_handler();
//    KConsole::print("MRK");
//    KConsole::console_handler();
//
//    return 0;
//}