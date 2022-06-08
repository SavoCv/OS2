//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"
#include "../h/TESTprint.hpp"

List<TCB> Scheduler::readyThreadQueue;

TCB *Scheduler::get()
{
    return readyThreadQueue.removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    //printString("#");
    //println((void*)&readyThreadQueue);
    readyThreadQueue.addLast(ccb);
    //printString("$");
}

bool Scheduler::has_n_threads(int n) {
    return readyThreadQueue.has_n_elements(n);
}
