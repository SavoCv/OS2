//
// Created by os on 6/9/22.
//
#include "../h/syscall_c.h"
#include "../h/TESTprint.hpp"

int test_input_output()
{
    char c[] = "Test output\n";
    char s[10];
    int j;
    for(int i = 0; i < 10; ++i)
    {
        printString(c);

        j = 0;
        while(true)
        {
            s[j++] = getc();
            //putc(s[j-1]);
            if(s[j - 1] == '\r')
                break;
        }
        s[j - 1] = '\0';

        printString(s);
        putc('\n');
    }

    return 0;
}