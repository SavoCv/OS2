//
// Created by os on 1/18/23.
//
#include "../h/slab.h"

struct myData{
    int x[5];
    char str[20];
};

/*
void userMain()
{
    constexpr int len = 125;
    myData *md[len];
    Cache *cache = kmem_cache_create("cache1", sizeof(myData), 0, 0);
    for(int i = 0; i < 2; ++i) {
        for (int i = 0; i < len; ++i) {
            md[i] = (myData *) kmem_cache_alloc(cache);
            if (i % 20 == 0) {
                printString("Alocirani pokazivac: ");
                printInt((long) md[i], 16);
                printString("\n");
                kmem_cache_info(cache);
            }
        }
        kmem_cache_info(cache);
        printString("+++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n");
        for (int i = 0; i < len; ++i) {
            kmem_cache_free(cache, md[i]);
            if(i % 20 == 0)
                kmem_cache_info(cache);
        }
        kmem_cache_info(cache);

        kmem_cache_shrink(cache);
        kmem_cache_info(cache);

        printString("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n\n\n");
    }
}
//*/

/*
void userMain()
{
    printf("Nula: %d\nString:%s\n", 0, "neki string");
}
//*/