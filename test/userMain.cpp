#include "../h/syscall_c.h"
#include "../h/syscall_cpp.hpp"
#include "printing.hpp"


struct thread_data {
    int id;
};

class ForkThread : public Thread {
public:
    ForkThread(long _id) noexcept : Thread(), id(_id), finished(false) {}
    virtual void run() {
        //printf("Started thread id:%d\n", id);
        printString("Started thread id:");
        printInt(id);
        printString("\n");

        ForkThread* thread = new ForkThread(id + 1);
        ForkThread** threads = (ForkThread** ) mem_alloc(sizeof(ForkThread*) * id);

        if (threads != nullptr) {
            for (long i = 0; i < id; i++) {
                threads[i] = new ForkThread(id);
            }

            if (thread != nullptr) {
                if (thread->start() == 0) {

                    int tmp = 5000; // bilo je 5000, brze se testira sa 5
                    for (int i = 0; i < tmp; i++) {
                        for (int j = 0; j < tmp; j++) {

                        }
                        thread_dispatch();
                    }

                    while (!thread->isFinished()) {
                        thread_dispatch();
                    }
                }
                delete thread;
            }

            for (long i = 0; i < id; i++) {
                delete threads[i];
            }

            mem_free(threads);
        }

        //printf("Finished thread id:%d\n", id);
        printString("Finished thread id:");
        printInt(id);
        printString("\n");

        finished = true;
    }

    bool isFinished() const {
        return finished;
    }

private:
    long id;
    bool finished;
};


void userMain() {

    printInt((int)((char*)HEAP_END_ADDR - (char*)HEAP_START_ADDR), 16);
    //printString("\n");

    //printString("1\n");

    ForkThread thread(1);

    //printString("2\n");

    thread.start();

    //printString("3\n");

    printInt((int)thread.isFinished());

    while (!thread.isFinished()) {
        //printString("A");
        thread_dispatch();
    }

    //printf("User main finished\n");
    printString("User main finished\n");
}//*/
/*
#include "../h/KConsole.h"

void userMain()
{
    printString("Pocetak\n");
    constexpr int N_SIZE=5;
    void *ptrs[N_SIZE];
    for(int i = 0; i < N_SIZE; ++i) {
        ptrs[i] = BuddyAllocator::getAllocator()->allocate(i);
        printInt((int)(long)(ptrs[i]), 16);
        printString("\n");
    }
    printString("Alocirano\n");
    int tmp;
    for(int i = 0; i < N_SIZE; ++i) {
        tmp = BuddyAllocator::getAllocator()->free(ptrs[i], i);
        printInt(tmp);
        printString("\n");
    }
    printString("Kraj\n");
}//*/
/*
#include "../h/SlabAllocator.h"

void userMain()
{
    printString("Pocetak\n");
    constexpr int N_SIZE=5;
    void *ptrs[N_SIZE];

    Cache *cache = SlabAllocator::getAllocator()->create_cache("MRK", 32, nullptr, nullptr);

    for(auto & ptr : ptrs) {
        ptr = SlabAllocator::getAllocator()->alloc(cache);
        printInt((int)(long)ptr, 16);
        printString("\n");
    }
    printString("Alocirano\n");
    SlabAllocator::getAllocator()->print_cache_info(cache);
//    int tmp;
    for(auto & ptr : ptrs) {
        SlabAllocator::getAllocator()->free(cache, ptr);
        //printInt(tmp);
        //printString("\n");
    }
    SlabAllocator::getAllocator()->print_cache_info(cache);
    printString("Kraj\n");
}*/