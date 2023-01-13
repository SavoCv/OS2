//
// Created by os on 6/8/22.
//

#ifndef PROJEKAT_KCONSOLE_H
#define PROJEKAT_KCONSOLE_H
#include "list.hpp"
#include "KSemaphore.hpp"

class KConsole {
    static List <char> output_buffer;
    static List <char> input_buffer;
    static KSemaphore* output_ready;
    static thread_t thr_t;

    static void aux();

    static void console_thread(void*);
public:
    static void init();

    static void putc(char c);

    static char getc();

    static void console_handler();

    static void print(const char* s);

    static void print(const int n);

    static void print(char* s){ print((const char *) s);}


    /*static void print(double n);

    static void print(double n, int dec);*/

    template<class T>
    static void println(T s);
};


#endif //PROJEKAT_KCONSOLE_H
