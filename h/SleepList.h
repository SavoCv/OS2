//
// Created by os on 6/8/22.
//

#ifndef PROJEKAT_SLEEPLIST_H
#define PROJEKAT_SLEEPLIST_H
#include "list.hpp"

struct TCBAndTime{
    TCB* t;
    time_t d;
    TCBAndTime(TCB* t, time_t time) : t(t), d(time){}

    void*  operator new(size_t_ sz){
        return kmem_alloc(sz);
    }

    void operator delete(void * ptr){
        kmem_free(ptr);
    }
};

class SleepList : public List <TCBAndTime>
{
public:
    void add(TCBAndTime *data)
    {
        Elem *elem = (Elem*) kmem_alloc(sizeof(Elem));
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
    }

};


#endif //PROJEKAT_SLEEPLIST_H
