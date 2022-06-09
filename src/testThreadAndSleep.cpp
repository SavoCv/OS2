//
// Created by os on 6/8/22.
//
//
// Created by os on 5/18/22.
//

#include "../h/tcb.hpp"
#include "../h/TESTprint.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.h"

int b_finished = 0;
sem_t b_mutex;
sem_t fo;

void workerB(void* ptr_n)
{
    int n = *(int*)ptr_n;
    printString("In B\n");
    for(int i = 1; true ; ++i)
    {
        time_sleep(n);
        sem_wait(fo);
        printString("A: ");
        printInteger(n);
        printString(" ");
        printInteger((uint64) i);
        printString("\n");
        sem_signal(fo);
        if(i == 10)
        {
            printString("a finished\n");
            sem_wait(b_mutex);
            b_finished++;
            sem_signal(b_mutex);
            thread_exit();
        }
    }
}

int test_thread_and_sleep()
{
    sem_open(&b_mutex, 1);
    sem_open(&fo, 1);
    const int num_of_thr = 350;
    const int max_time = 10;
    int neki_br = 3456;
    thread_t threads[num_of_thr];
    int *n = (int*) mem_alloc(sizeof(int) * num_of_thr);
    printString("Test thread start\n");
    for(int i = 0; i < num_of_thr; ++i) {
        neki_br = (neki_br * (i + 1) ) % max_time;
        n[i] = neki_br + 1;
    }
    for(int i = 0; i < num_of_thr; ++i)
    {
        char gr[] = "GRESKA: 000\n";
        char cr[] = "A 000 created\n";
        gr[10] = '0' + i % 10;
        gr[9] = '0' + (i / 10) % 10;
        gr[8] = '0' + (i / 100) % 10;

        cr[4] = '0' + i % 10;
        cr[3] = '0' + (i / 10) % 10;
        cr[2] = '0' + (i / 100) % 10;

        sem_wait(fo);
        if(thread_create(&threads[i], workerB, &n[i]) < 0)
            printString(gr);
        else
            printString(cr);
        sem_signal(fo);
    }

    printString("Test thread nit\n");
    while(b_finished < num_of_thr) {
        //printString("m dsp\n");
        thread_dispatch();
    }
    printString("m finished\n");

    return 0;
}
