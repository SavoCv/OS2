//
// Created by os on 5/18/22.
//

#include "../h/tcb.hpp"
#include "../h/TESTprint.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

void workerA(void* ptr_n)
{
    int n = *(int*)ptr_n;
    for(int i = 1; i <= 10; ++i)
    {
        for(int j = 0; j < n; ++j)
        {
            //busy wait
        }
        print("A: ");
        print((uint64) i);
        print("\n");
        if(i == 9)
        {
            thread_exit();
        }
    }
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
    if(thread_create(&threads[0], workerA, &n[0]) < 0)
        print("GRESKA: A\n");
    else
        print("A created");
    if(thread_create(&threads[1], workerA, &n[1]) < 0)
        print("GRESKA: B\n");
    print("Test thread nit\n");

    return 0;
}
