//
// Created by os on 1/13/23.
//
#include "../h/slab.h"
#include "../h/SlabAllocator.h"

using size_t = decltype(sizeof(0));

void kmem_init(void *space, int block_num)
{
    SlabAllocator::getAllocator()->init(space, block_num);
}

kmem_cache_t *kmem_cache_create(const char *name, size_t size,
                                void (*ctor)(void *),
                                void (*dtor)(void *))
{
    return SlabAllocator::getAllocator()->create_cache(name, size, ctor, dtor);
}

int kmem_cache_shrink(kmem_cache_t *cachep)
{
    return SlabAllocator::getAllocator()->shrink_cache(cachep);
}

void *kmem_cache_alloc(kmem_cache_t *cachep)
{
    return SlabAllocator::getAllocator()->alloc(cachep);
}

void kmem_cache_free(kmem_cache_t *cachep, void *objp)
{
    SlabAllocator::getAllocator()->free(cachep, objp);
}

void *kmalloc(size_t size)
{
    return SlabAllocator::getAllocator()->kmalloc(size);
}

void kfree(const void *objp)
{
    SlabAllocator::getAllocator()->kfree(objp);
}

void kmem_cache_destroy(kmem_cache_t *cachep)
{
    SlabAllocator::getAllocator()->delete_cache(cachep);
}

void kmem_cache_info(kmem_cache_t *cachep)
{
    SlabAllocator::getAllocator()->print_cache_info(cachep);
}

int kmem_cache_error(kmem_cache_t *cachep)
{
    return SlabAllocator::getAllocator()->print_error_cache(cachep);
}