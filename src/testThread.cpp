//
// Created by os on 5/18/22.
//

#include "../h/tcb.hpp"
#include "../h/TESTprint.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

int a_finished = 0;

void workerA(void* ptr_n)
{
    int n = *(int*)ptr_n;
    printString("In A\n");
    for(int i = 1; i <= 10; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            //busy wait
        }
        printString("A: ");
        printInteger((uint64) i);
        printString("\n");
        if(i == 9)
        {
            a_finished++;
            printString("a finished\n");
            //thread_exit();
            return;
        }
        printString("a dsp\n");
        thread_dispatch();
    }
    a_finished++;
    printString("a finished\n");
}

int test_thread()
{
    thread_t threads[2];
    int *n = (int*) mem_alloc(sizeof(int) * 2);
    print("Test thread start\n");
    print(n);
    print("\n");
    n[0] = 100000;
    n[1] = 200000;
    print("A creating\n");
    if(thread_create(&threads[0], workerA, &n[0]) < 0)
        print("GRESKA: A\n");
    else
        print("A created\n");
    if(thread_create(&threads[1], workerA, &n[1]) < 0)
        print("GRESKA: B\n");
    else
        print("B created\n");
    print("Test thread nit\n");
    while(a_finished < 2) {
        print("m dsp\n");
        thread_dispatch();
    }

    return 0;
}
