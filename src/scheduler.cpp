//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"
#include "../h/TESTprint.hpp"

List Scheduler::readyThreadQueue;

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