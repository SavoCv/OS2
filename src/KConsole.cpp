//
// Created by os on 6/8/22.
//

#include "../h/KConsole.h"
#include "../lib/hw.h"
#include "../h/syscall_c.h"
#include "../h/SlabAllocator.h"

List <char>* KConsole::output_buffer_p;
List <char>* KConsole::input_buffer_p;
//List<char>& KConsole::output_buffer = *output_buffer_p;
//List<char>& KConsole::input_buffer = *input_buffer_p;
KSemaphore* KConsole::output_ready(0);
thread_t  KConsole::thr_t;

sem_t input_ready;

void KConsole::init()
{
    output_ready = new KSemaphore(0);
    input_ready = new KSemaphore(0);
    //sem_open(&input_ready, 0);
    thr_t = TCB::createThread(console_thread, kmem_alloc(DEFAULT_STACK_SIZE), nullptr);
    //thread_create(&thr_t, console_thread, nullptr);
    output_buffer_p = new List<char>();
    input_buffer_p = new List<char>();
}

void KConsole::putc(char c)
{

    if(c == '\r')
        c = '\n';
    char *pc = (char*) kmem_alloc(sizeof(char));
    *pc = c;
    output_buffer_p->addLast(pc);
    output_ready->signal();
    console_handler();
}

char KConsole::getc()
{
    char *tmp = input_buffer_p->removeFirst();
    char c = *tmp;
    kmem_free(tmp);
    if(c == 0)
        c = EOF;
    putc(c); //Obavezno ako se pokrece iz terminala
    return c;
}

void KConsole::aux()
{
    char* cs = (char*) CONSOLE_STATUS;
    if (*cs & CONSOLE_RX_STATUS_BIT) {
        while (*cs & CONSOLE_RX_STATUS_BIT) {
            char *drx = (char *) CONSOLE_RX_DATA;
            char *tmp = (char *) kmem_alloc(sizeof(char));
            *tmp = *drx;
            input_buffer_p->addLast(tmp);
            input_ready->signal();
        }
    }
    if (*cs & CONSOLE_TX_STATUS_BIT) {
        while (*cs & CONSOLE_TX_STATUS_BIT) {
            char *dtx = (char *) CONSOLE_TX_DATA;
            char *tmp = output_buffer_p->removeFirst();
            if (tmp) {
                *dtx = *tmp;
                kmem_free(tmp);
            } else
                break;
        }
    }
}

[[noreturn]] void KConsole::console_thread(void*)
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

    aux();

    plic_complete(CONSOLE_IRQ);
}

void KConsole::print(const char * const s) {
    for(int i = 0; s[i] != 0; ++i)
        putc(s[i]);
}

void KConsole::print(const int n) {
    char *buff = (char*) kmem_alloc(20);
            //SlabAllocator::getAllocator()->kmalloc(20);
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

void KConsole::print_hex(const long n) {
    char *buff = (char*) kmem_alloc(20);
            //SlabAllocator::getAllocator()->kmalloc(20);
    char *p = buff + 19;
    long nc = n;
    *p = 0;
    do {
        --p;
        *p = '0' + nc % 16;
        if(*p > '9')
            *p = *p - '9' - 1 + 'A';
        nc /= 16;
    } while(nc);
    print("0x");
    print(p);
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
