//
// Created by os on 5/16/22.
//
#include "../lib/hw.h"
#include "../h/TESTprint.hpp"
#include "../h/MemoryAllocator.h"

void test_allocator();

int* allocate_arr(int size, const char* name);

void free(void* ptr, const char* name);

int* allocate_arr(int size, const char* name) {
    const int diff = 100;
    static int off = 1 - diff;
    off += diff;
    int *x = (int*) kmem_alloc(size * sizeof(int));

    print("Allocated ");
    print(name);
    print(" with address ");
    print(x);
    print(" (offset ");
    print((void*)((char*)x - (char*)HEAP_START_ADDR));
    print(") and size ");
    println(size);

    for(int i = 0; i < size; ++i)
        x[i] = i + off;
    return x;
}

void free(void* ptr, const char* name)
{
    kmem_free(ptr);
    print("Deleted ");
    println(name);
}

void test_allocator() {
    print("Heap start: ");
    println(HEAP_START_ADDR);
    print("Heap end: ");
    println(HEAP_END_ADDR);
    print("Mem block size: ");
    println(MEM_BLOCK_SIZE);

    MemoryAllocator* m = MemoryAllocator::getAllocator();
    MemoryAllocator* m2 = MemoryAllocator::getAllocator();

    println("");
    println("Allocator address:");
    println(m);
    println(m2);

    println("");

    int *x, *y, *z;

    x = allocate_arr(10, "x");

    y = allocate_arr(20, "y");

    print_arr(x, 10, "x");
    print_arr(y, 20, "y");
    println("");

    free(x, "x");

    x = allocate_arr(11, "x");

    z = allocate_arr(9, "z");

    print_arr(x, 11, "x");
    print_arr(y, 20, "y");
    print_arr(z, 9, "z");
    println("");

    free(z, "z");

    z = allocate_arr(10, "z");

    print_arr(x, 11, "x");
    print_arr(y, 20, "y");
    print_arr(z, 10, "z");
    println("");

    free(x, "x");
    free(y, "y");
    free(z, "z");

    int *a[5];
    const char *n[5] = {"a0", "a1", "a2", "a3", "a4"};
    for(int i = 0; i < 5; ++i)
        a[i] = allocate_arr(10, n[i]);
    for(int i = 1; i < 4; ++i)
        free(a[i], n[i]);
    a[1] = allocate_arr(34, "LAST");
    free(a[0], n[0]);
    free(a[1], n[1]);
    free(a[4], n[4]);

    x = allocate_arr(500, "x");
    free(x, "x");
}