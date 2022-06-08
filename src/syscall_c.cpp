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
    ra0 = (ra0 - 1 + 16) / MEM_BLOCK_SIZE + 1;
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
    stack_space = (char*) stack_space + DEFAULT_STACK_SIZE;
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (handle));
    __asm__ volatile("mv a2, %[ss]" : : [ss] "r" (start_routine));
    __asm__ volatile("mv a3, %[ss]" : : [ss] "r" (arg));
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
    __asm__ volatile("addi a0, zero, 0x13");
    __asm__ volatile("ecall");
}

int sem_open(sem_t *handle, unsigned init)
{
    __asm__ volatile("mv a2, %[ss]" : : [ss] "r" (init));
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (handle));
    __asm__ volatile("addi a0, zero, 0x21");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

int sem_close(sem_t handle)
{
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (handle));
    __asm__ volatile("addi a0, zero, 0x22");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

int sem_wait(sem_t id)
{
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (id));
    __asm__ volatile("addi a0, zero, 0x23");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

int sem_signal(sem_t id)
{
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (id));
    __asm__ volatile("addi a0, zero, 0x24");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

#ifdef __cplusplus
}

#endif