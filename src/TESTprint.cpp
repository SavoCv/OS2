//
// Created by marko on 20.4.22..
//

#include "../h/TESTprint.hpp"
#include "../h/riscv.hpp"
#include "../lib/console.h"

void printString(char const *string)
{
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    while (*string != '\0')
    {
        __putc(*string);
        string++;
    }
    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void printInteger(uint64 integer)
{
    uint64 sstatus = Riscv::r_sstatus();
    Riscv::mc_sstatus(Riscv::SSTATUS_SIE);
    static char digits[] = "0123456789";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (integer < 0)
    {
        neg = 1;
        x = -integer;
    } else
    {
        x = integer;
    }

    i = 0;
    do
    {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0) { __putc(buf[i]); }
    Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void print(const char* str)
{
    for(int i = 0; str[i] != '\0'; ++i)
        __putc(str[i]);
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
    print(s + i);
}

void print(int n)
{
    print((uint64)n);
}

void print(const void* ptr)
{
    print((uint64)ptr, true);
}
