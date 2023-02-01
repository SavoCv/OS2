//
// Created by os on 1/3/23.
//

#include "../h/BuddyAllocator.h"
#include "../h/MemoryAllocator.h"

BuddyAllocator *BuddyAllocator::getAllocator() {
    static BuddyAllocator *ma = nullptr;
    if(!ma) {
        ma = (BuddyAllocator *) MemoryAllocator::getAllocator()->allocate(sizeof(BuddyAllocator));
    }
    return ma;
}

int BuddyAllocator::init(void *space, int block_num)
{
    if(space == nullptr || block_num == 0)
        return -1;

    starting = (char*) space;

    int i, j;
//    for(i = 0, j = 1; j <= block_num; ++i, j <<= 1);
//    // j = 1 << i;
//    --i;
//    j >>= 1;
//    if(i >= X_SIZE)
//        return -2;

    for(i = X_SIZE - 1, j = 1 << i; i>= 0; --i, j >>= 1)
        if(block_num >= j) {
            x[i] = space;
            *(void**)x[i] = nullptr;
            space = (char *) space + j * BLOCK_SIZE;
            block_num -= j;
        }
        else
            x[i] = nullptr;

    if(block_num > 0)
        return -2;

    return 0;
}

void* BuddyAllocator::allocate(int vel)
{
    if(vel < 0 || vel >= X_SIZE)
        return nullptr;
    int i;
    for(i = vel; i < X_SIZE; ++i)
        if(x[i] != nullptr)
            break;
    if(i == X_SIZE)
        return nullptr;
    void* ret = x[i];
    x[i] = *(void**)x[i];
    for(int j = 1 << i; i > vel; --i, j >>=1)
    {
        void* ptr = (char*) ret + (j >> 1) * BLOCK_SIZE;
        *(void**)ptr = x[i-1];
        x[i-1] = ptr;
    }
    return ret;
}

int BuddyAllocator::free(void *space, int vel)
{
    if(vel < 0 || vel >= X_SIZE)
        return -1;
    
    while(true) {
        int rv = 1 << vel;
        int rb = ((char *) space - starting) / BLOCK_SIZE / rv;
        void *buddy, *sp_and_bd;
        if (rb % 2) {
            buddy = (char *) space + rv * BLOCK_SIZE;
            sp_and_bd = space;
        } else {
            buddy = (char *) space - rv * BLOCK_SIZE;
            sp_and_bd = buddy;
        }

        bool found_buddy = false;
        if (x[vel] == buddy) {
            x[vel] = *(void **) x[vel];
            found_buddy = true;
        } else
            for (void *i = x[vel]; i != nullptr;) {
                void *next = *(void **) i;
                if (next == buddy) {
                    *(void **) i = *(void **) next;
                    found_buddy = true;
                    break;
                }
                i = next;
            }
        if(!found_buddy)
        {
            *(void **)space = x[vel];
            x[vel] = space;
            break;
        }
        else
        {
            space = sp_and_bd;
            ++vel;
        }
    }
    return 0;
}

int BuddyAllocator::getLogSize(int size)
{
    if(size < 0)
        return -1;
    int i, j;
    for(i = 0, j = 1; j <= size; ++i, j <<= 1);
    return i;
}