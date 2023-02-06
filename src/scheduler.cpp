//
// Created by marko on 20.4.22..
//

#include "../h/scheduler.hpp"
#include "../h/KConsole.h"

List<TCB>* Scheduler::readyThreadQueue_p = List<TCB>::produce();
        //new List<TCB>();

TCB *Scheduler::get()
{
    return readyThreadQueue_p->removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    if(!readyThreadQueue_p)
        readyThreadQueue_p = List<TCB>::produce();
                //new List<TCB>();
    //printString("#");
    //println((void*)&readyThreadQueue);
    if(readyThreadQueue_p->addLast(ccb) != 0) {
        SlabAllocator::getAllocator()->shrink_all_caches();
        if(readyThreadQueue_p->addLast(ccb) != 0)
        {
            KConsole::print("Bez memorije");
        }
    }
    //printString("$");
}

bool Scheduler::has_n_threads(int n) {
    return readyThreadQueue_p->has_n_elements(n);
}
