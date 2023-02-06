//
// Created by os on 6/8/22.
//

#ifndef PROJEKAT_SLEEPLIST_H
#define PROJEKAT_SLEEPLIST_H
#include "list.hpp"

struct TCBAndTime{
    TCB* t;
    time_t d;
    //TCBAndTime(TCB* t, time_t time) : t(t), d(time){}
    int init(TCB* t_, time_t time){
        this->t = t_;
        d = time;
        return 0;
    }

    static TCBAndTime* produce(TCB* tcb, time_t time){
        TCBAndTime *tmp = (TCBAndTime*)kmem_alloc(sizeof(TCBAndTime));
        if(tmp == nullptr)
            return nullptr;
        tmp->init(tcb, time);
        return tmp;
    }

    /*void*  operator new(size_t_ sz){
        return kmem_alloc(sz);
    }

    void operator delete(void * ptr){
        kmem_free(ptr);
    }*/
};

class SleepList : public List <TCBAndTime>
{
public:
    static SleepList* produce()
    {
        SleepList* sl = (SleepList*)kmem_alloc(sizeof(SleepList));
        if(sl == nullptr)
            return nullptr;
        sl->init();
        return sl;
    }

    int add(TCBAndTime *data)
    {
        Elem *elem = (Elem*) kmem_alloc(sizeof(Elem));
        if(elem == nullptr)
            return -1; // Exception
        elem->data = data;

        Elem* curr = head, *prev = nullptr;

        for(; curr; prev = curr, curr = curr->next)
            if(elem->data->d <= curr->data->d)
                break;
            else
                elem->data->d -= curr->data->d;

        if(!prev)
        {
            elem->next = head;
            head = elem;
        }
        else
        {
            prev->next = elem;
            elem->next = curr;
        }
        if(curr)
            curr->data->d -= elem->data->d;
        else
            tail = elem;
        return 0;
    }
};


#endif //PROJEKAT_SLEEPLIST_H
