//
// Created by os on 5/20/22.
//
#include "../h/syscall_c.h"

typedef _Bool  bool;

void printstr(const char* str)
{
    for(int i = 0; str[i] != '\0'; ++i)
        putc(str[i]);
}

void print (uint64 x, bool hex) {
    char s[20];
    int i = 20;
    s[--i] = '\0';
    if(!hex)
        do {
            s[--i] = '0' + x % 10;
            x /= 10;
        } while(x > 0);
    else {
        do {
            s[--i] = '0' + x % 16;
            if (s[i] > '9')
                s[i] = s[i] - '9' + 'A' - 1;
            x /= 16;
        } while (x > 0);
        s[--i] = 'x';
        s[--i] = '0';
    }
    printstr(s + i);
}

void test_allocator_c_api()
{
    int * a = (int*) mem_alloc(10 * sizeof(int));
    print((uint64)a, 1);
    putc('\n');
    print(mem_free(a), 0);
    putc('\n');
}