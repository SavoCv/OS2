//
// Created by marko on 20.4.22..
//

#ifndef OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#define OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
#include "MemoryAllocator.h"
#include "SlabAllocator.h"
#include "utils.h"

using size_t_ = decltype(sizeof(0));

class TCB;

template <class T>
class List
{
protected:
    struct Elem
    {
        T *data;
        Elem *next;

        Elem(T *data, Elem *next) : data(data), next(next) {}

        /*void*  operator new(size_t_ sz){
            return kmem_alloc(sz);
        }

        void operator delete(void * ptr){
            kmem_free(ptr);
        }*/
    };

    Elem *head, *tail;

public:
    //List() : head(0), tail(0) {}

    int init(){
        head = 0;
        tail = 0;
        return 0;
    }

    static List<T>* produce()
    {
        List<T>* tmp = (List<T>*) kmem_alloc(sizeof(List<T>));
        if(tmp == nullptr)
            return nullptr;
        tmp->init();
        return tmp;
    }

    List(const List<T> &) = delete;

    List<T> &operator=(const List &) = delete;

    __attribute__((noinline)) int addFirst(T *data)
    {
        Elem *elem = (Elem*) kmem_alloc(sizeof(Elem));
        if(elem == nullptr)
            return -1; // Exception
        elem->data = data;
        elem->next = head;

        head = elem;
        if (!tail) { tail = head; }
        return 0;
    }

    __attribute__((noinline)) int addLast(T *data)
    {
        if(data == nullptr)
            return -1;
        Elem *elem = (Elem*)kmem_alloc(sizeof(Elem));
        if(elem == nullptr)
            return -1; //Exception
        elem->data = data;
        elem->next = 0;
        if (tail)
        {
            tail->next = elem;
            tail = elem;
        } else
        {
            head = tail = elem;
        }
        return 0;
    }

    __attribute__((noinline)) T *removeFirst()
    {
        if (!head) {
            return 0;
        }

        Elem *elem = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = elem->data;
        kmem_free(elem);
        return ret;
    }

    T *peekFirst()
    {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast()
    {
        if (!head) { return 0; }

        Elem *prev = 0;
        for (Elem *curr = head; curr && curr != tail; curr = curr->next)
        {
            prev = curr;
        }

        Elem *elem = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = elem->data;
        kmem_free(elem);
        return ret;
    }

    T *peekLast()
    {
        if (!tail) { return 0; }
        return tail->data;
    }

    bool has_n_elements(int n) {
        for (Elem *curr = head; curr; curr = curr->next)
        {
            --n;
            if(n < 0)
                return false;
        }
        return n == 0;
    }

    /*void*  operator new(size_t_ sz){
        return kmem_alloc(sz);
    }

    void operator delete(void * ptr){
        kmem_free(ptr);
    }*/
};

#endif //OS1_VEZBE07_RISCV_CONTEXT_SWITCH_2_INTERRUPT_LIST_HPP
