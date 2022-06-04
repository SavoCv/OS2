//
// Created by os on 5/19/22.
//
#include "../h/syscall_c.h"
#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

void* mem_alloc(size_t size)
{
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    ra0 = (ra0 - 1) / MEM_BLOCK_SIZE + 1;
    __asm__ volatile("mv a1, %[ra0]" : :[ra0] "r"(ra0));
    __asm__ volatile("addi a0, zero, 0x1");
   // __asm__ volatile ("mul a1, a1, %[BLOCK]" : : [BLOCK] "r"(MEM_BLOCK_SIZE));
    __asm__ volatile("ecall");
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (void*) ra0;
}

int mem_free(void* ptr)
{
    __asm__ volatile("mv a1, a0");
    __asm__ volatile("addi a0, zero, 0x2");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

int thread_create(thread_t* handle, void (*start_routine)(void*), void* arg)
{
    void * stack_space = mem_alloc(DEFAULT_STACK_SIZE);
    __asm__ volatile("mv a3, a2");
    __asm__ volatile("mv a2, a1");
    __asm__ volatile("mv a1, a0");
    __asm__ volatile("mv a4, %[ss]" : : [ss] "r" (stack_space));
    __asm__ volatile("addi a0, zero, 0x11");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

int thread_exit()
{
    __asm__ volatile("addi a0, zero, 0x12");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

void thread_dispatch()
{
    __asm__ volatile("addi a0, zero, 0x12");
    __asm__ volatile("ecall");
}

#ifdef __cplusplus
}

#endif