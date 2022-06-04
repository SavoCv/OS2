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

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg);

int thread_exit();

void thread_dispatch();

#ifdef __cplusplus
}

#endif


#endif //PROJECT_BASE_SYSCALL_C_H
