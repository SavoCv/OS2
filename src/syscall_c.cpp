//
// Created by os on 5/19/22.
//
#include "../h/syscall_c.h"
#include "../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

const int sizeof_fragment = 24;

void* mem_alloc(size_t size)
{
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    ra0 = (ra0 - 1 + sizeof_fragment) / MEM_BLOCK_SIZE + 1;
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
    if(stack_space == nullptr)
        return -1;
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

int time_sleep(time_t t)
{
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" (t));
    __asm__ volatile("addi a0, zero, 0x31");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (int) ra0;
}

void putc(char c)
{
    __asm__ volatile("mv a1, %[ss]" : : [ss] "r" ((uint64) c));
    __asm__ volatile("addi a0, zero, 0x42");
    __asm__ volatile("ecall");
}

extern sem_t input_ready;

char getc()
{
    sem_wait(input_ready);
    __asm__ volatile("addi a0, zero, 0x41");
    __asm__ volatile("ecall");
    uint64 ra0 = 0;
    __asm__ volatile("mv %[ra0], a0" : [ra0] "=r"(ra0));
    return (char) ra0;
}

int myfunc(int count, ...)
{
    va_list list;
    int j = 0;

    va_start(list, count);
    for(j=0; j<count; j++)
    {
        printf("%d", va_arg(list, int));
    }

    va_end(list);

    return count;
}

//Kod javnog testa OS1
void printString(char const *string)
{
    while (*string != '\0')
    {
        putc(*string);
        string++;
    }
}

//Kod javnog testa OS1
int stringToInt(const char *s) {
    int n;

    n = 0;
    while ('0' <= *s && *s <= '9')
        n = n * 10 + *s++ - '0';
    return n;
}

char digits[] = "0123456789ABCDEF";

//Kod javnog testa OS1
void printInt(int xx, int base, int sgn)
{
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if(sgn && xx < 0){
        neg = 1;
        x = -xx;
    } else {
        x = xx;
    }

    i = 0;
    do{
        buf[i++] = digits[x % base];
    }while((x /= base) != 0);
    if(neg)
        buf[i++] = '-';

    while(--i >= 0)
        putc(buf[i]);
}

/*
int myfunc(int count, ...)
{
   va_list list;
   int j = 0;

   va_start(list, count);
   for(j=0; j<count; j++)
   {
     printf("%d", va_arg(list, int));
   }

   va_end(list);

   return count;
}
 */

int printf(const char* pattern, ...)
{
    int cnt = 0;
    for(const char *i = pattern; *i; ++i)
        if(*i == '%') {
            if (i[1] == 0)
                break;
            else if (i[1] == '%')
                ++i;
            else
                ++cnt;
        }

    static sem_t sem = nullptr;
    if(sem == nullptr)
    {
        sem_open(&sem, 1);
        if(sem == nullptr)
            return -1;
    }
    sem_wait(sem);
    va_list list;
    va_start(list, pattern);

    for(const char *i = pattern; *i; ++i)
    {
        if(*i != '%')
            putc(*i);
        else
        {
            ++i;
            switch (*i) {
                case 0:
                    --i;
                    break;

                case '%':
                    putc('%');
                    break;

                case 'd': case 'i':
                    printInt(va_arg(list, int), 10, 1);
                    break;

                case 'u':
                    printInt(va_arg(list, int), 10, 0);
                    break;

                case 'x':
                    printInt(va_arg(list, int), 16, 0);
                    break;

                case 'o':
                    printInt(va_arg(list, int), 8, 0);
                    break;

                case 'p':
                    printInt((int)(uint64) va_arg(list, void*), 16, 0);
                    break;

                case 'c':
                    putc(va_arg(list, int));
                    break;

                case 's':
                    printString(va_arg(list, char*));
                    break;
            }
        }
    }

    va_end(list);
    sem_signal(sem);
    return cnt;
}

#ifdef __cplusplus
}

#endif