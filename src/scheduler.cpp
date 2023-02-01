//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"

List<TCB>* Scheduler::readyThreadQueue_p = new List<TCB>();

TCB *Scheduler::get()
{
    return readyThreadQueue_p->removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    if(!readyThreadQueue_p)
        readyThreadQueue_p = new List<TCB>();
    //printString("#");
    //println((void*)&readyThreadQueue);
    readyThreadQueue_p->addLast(ccb);
    //printString("$");
}

bool Scheduler::has_n_threads(int n) {
    return readyThreadQueue_p->has_n_elements(n);
}
