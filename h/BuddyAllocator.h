//
// Created by os on 1/3/23.
//

#ifndef PROJEKAT_BUDDYALLOCATOR_H
#define PROJEKAT_BUDDYALLOCATOR_H
#include "MemoryAllocator.h"

#define BLOCK_SIZE (4096)

/**
 * Alokator koji radi po sistemu parnjaka \n
 * Pretopostavljeno je da ce se init pozvati pre poziva bilo koje druge operacije
 */
class BuddyAllocator {
public:
    BuddyAllocator(const BuddyAllocator&) = delete;
    BuddyAllocator(BuddyAllocator&&) = delete;
    BuddyAllocator& operator=(const BuddyAllocator&) = delete;
    BuddyAllocator& operator=(BuddyAllocator&&) = delete;

    /**
     *
     * @return The (only) instance of BuddyAllocator
     */
    static BuddyAllocator* getAllocator();

    /**
     *
     * @param vel zeljena velicina 2^vel blokova
     * @return pokazivac na alociranu memoriju ili nullptr
     */
    void* allocate(int vel);

    /**
     *
     * @param space pokazivac na prostor koji se oslobadja
     * @param vel   velicina 2^vel blokova koja se oslobadja
     * @return 0    ako nema greske, -1   ako ima greska
     */
    int free(void *space, int vel);

    /**
     *
     * @param space pokazivac na prostor kojim treba da rukuje alokator
     * @param block_num broj blokova prostora kojim treba da rukuje alokator
     * @return 0 u slucaju uspeha, u suprotnom negativnu vrednost
     */
    int init(void *space, int block_num);

    /**
     *
     * @param size velicina zeljene memorije
     * @return k gde je k najmanje tako da zadovoljava 2^k >= size
     */
    static int getLogSize(int size);

private:
    static const int X_SIZE = 32; // 2 ^ X_SIZE maksimalan broj blokova koji moze rukovoditi sa alokatorom
    void *x[X_SIZE];
    char *starting;
};


#endif //PROJEKAT_BUDDYALLOCATOR_H
