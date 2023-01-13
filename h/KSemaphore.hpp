//
// Created by os on 6/7/22.
//

#ifndef PROJEKAT_KSEMAPHORE_H
#define PROJEKAT_KSEMAPHORE_H
#include "list.hpp"
#include "tcb.hpp"

using size_t_ = decltype(sizeof(0));

class KSemaphore{
public:
    KSemaphore(unsigned init);
    ~KSemaphore();
    int wait();
    int kwait();
    int signal();
    void*  operator new(size_t_ sz);
    void operator delete(void * ptr);

private:

    int val;
    List <TCB> blocked;
    volatile int deleted = 0;//Maybe without volatile
};

#endif //PROJEKAT_KSEMAPHORE_H
