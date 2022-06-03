//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_PRINT_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_PRINT_HPP

#include "../lib/hw.h"
#include "../lib/console.h"

extern void printString(char const *string);

extern void printInteger(uint64 integer);

void print(const char* str);

void print(uint64, bool hex = false);

void print(const void* ptr);

void print(int n);

template <class T>
void println(T a)
{
    print(a);
    __putc('\n');
}

template <class T>
void print_arr(T *a, int sz, const char* name)
{
    if(name[0] != '\0')
        print(name), print(": ");
    for(int i = 0; i < sz; ++i)
        print(a[i]), print(" ");
    println("");
}

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_PRINT_HPP
