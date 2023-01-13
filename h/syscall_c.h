//
// Created by os on 5/19/22.
//

#ifndef PROJECT_BASE_SYSCALL_C_H
#define PROJECT_BASE_SYSCALL_C_H

#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

void *mem_alloc(size_t size);

int mem_free(void* ptr);

struct TCB;
typedef struct TCB* thread_t;

struct KSemaphore;
typedef struct KSemaphore* sem_t;

typedef unsigned long time_t;

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

int sem_open(sem_t *handle, unsigned init);

int sem_close(sem_t handle);

int sem_wait(sem_t id);

int sem_signal(sem_t id);

int time_sleep(time_t t);

const int EOF = -1;

void putc(char c);

char getc();

#ifdef __cplusplus
}

#endif


#endif //PROJECT_BASE_SYSCALL_C_H
