//
// Created by os on 6/8/22.
//

#include "../h/KConsole.h"
#include "../lib/hw.h"
#include "../h/syscall_c.h"
#include "../h/SlabAllocator.h"

List<char> KConsole::output_buffer;
List<char> KConsole::input_buffer;
KSemaphore* KConsole::output_ready(0);
thread_t  KConsole::thr_t;

sem_t input_ready;

void KConsole::init()
{
    //TODO
    output_ready = new KSemaphore(0);
    sem_open(&input_ready, 0);
    thread_create(&thr_t, console_thread, nullptr);
}

void KConsole::putc(char c)
{
    //TODO
    if(c == '\r')
        c = '\n';
    char *pc = (char*) kmem_alloc(sizeof(char));
    *pc = c;
    output_buffer.addLast(pc);
    output_ready->signal();
    console_handler();
}

char KConsole::getc()
{
    char *tmp = input_buffer.removeFirst();
    char c = *tmp;
    kmem_free(tmp);
    if(c == 0)
        c = EOF;
    //putc(c); //Obavezno ako se pokrece iz terminala
    return c;
}

void KConsole::aux()
{
    char* cs = (char*) CONSOLE_STATUS;
    if (*cs & CONSOLE_RX_STATUS_BIT) {
        //Read TODO
        while (*cs & CONSOLE_RX_STATUS_BIT) {
            char *drx = (char *) CONSOLE_RX_DATA;
            char *tmp = (char *) kmem_alloc(sizeof(char));
            *tmp = *drx;
            input_buffer.addLast(tmp);
            input_ready->signal();
        }
    }
    if (*cs & CONSOLE_TX_STATUS_BIT) {
        //Write TODO
        while (*cs & CONSOLE_TX_STATUS_BIT) {
            char *dtx = (char *) CONSOLE_TX_DATA;
            char *tmp = output_buffer.removeFirst();
            if (tmp) {
                *dtx = *tmp;
                kmem_free(tmp);
            } else
                break;
        }
    }
}

void KConsole::console_thread(void*)
{
    while(true) {
        aux();
        thread_dispatch();
    }
}

void KConsole::console_handler()
{
    if(plic_claim() != CONSOLE_IRQ)
        return;

    //TODO
    aux();

    plic_complete(CONSOLE_IRQ);
}

void KConsole::print(const char *s) {
    for(int i = 0; s[i] != 0; ++i)
        putc(s[i]);
}

void KConsole::print(const int n) {
    char *buff = (char*) SlabAllocator::getAllocator()->kmalloc(20);
    char *p = buff + 19;
    int nc = n;
    *p = 0;
    do {
        --p;
        *p = '0' + nc % 10;
        nc /= 10;
    } while(nc);
    print(p);
}

template<class T>
void KConsole::println(T s) {
    print(s);
    print("\n");
}

/*void KConsole::print(double n, int dec) {
    print((int)n);
    int tmp = 1;
    if(dec > 8)
        dec = 8;
    for(int i = 0; i < dec; ++i)
        tmp *= 10;
    n -= (int)n;
    n *= tmp;
    n += 0.5;
    print((int)n);
}

void KConsole::print(double n) {
    print(n, 2);
}*/
