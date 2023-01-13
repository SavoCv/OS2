//
// Created by os on 1/12/23.
//

#ifndef PROJEKAT_SLABK_H
#define PROJEKAT_SLABK_H


class SlabK {
    friend class Cache;
    SlabK *next;
    int cnt;
    int kap;
    void *objects;
    int object_size;
    int size;

public:

    void init(void* space, int size, int obj_sz);

    void* alloc();

    int get_free_cnt(){ return kap - cnt; }

    int get_cnt(){return cnt;}

    int get_kap(){return kap;}

    bool try_free(void *obj, void (*dtor)(void*));
};


#endif //PROJEKAT_SLABK_H
