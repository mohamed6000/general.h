#ifndef GENERAL_POOL_INCLUDE_H
#define GENERAL_POOL_INCLUDE_H
/*

    General pool allocator.


    To include pool implementation as cpp file use:

    #define POOL_IMPLEMENTATION
    #include "pool.h"

*/

#include "general.h"
#include "array.h"


const s64 POOL_BUCKET_SIZE_DEFAULT = KB(4);
const s64 POOL_ALIGNMENT_DEFAULT   = 8;

typedef struct Pool {
    s64 memblock_size = POOL_BUCKET_SIZE_DEFAULT;
    s64 alignment     = POOL_ALIGNMENT_DEFAULT;

    u8 *current_memblock = null;
    u8 *current_pos      = null;
    s64 bytes_left       = 0;

    Array<u8 *> used_memblocks;
    Array<u8 *> unused_memblocks;

    Allocator block_allocator = {heap_allocator, null};
} Pool;

void set_allocators(Pool *pool, 
                    Allocator block_allocator = {heap_allocator, null},
                    Allocator array_allocator = {heap_allocator, null});

inline void pool_init(Pool *pool, 
                      s64 block_size = POOL_BUCKET_SIZE_DEFAULT, 
                      s64 alignment  = POOL_ALIGNMENT_DEFAULT) {
    pool->memblock_size = block_size;
    pool->alignment     = alignment;

    pool->current_memblock = null;
    pool->current_pos      = null;
    pool->bytes_left       = 0;

    set_allocators(pool, {heap_allocator, null}, {heap_allocator, null});
}

TINYRT_EXTERN void *pool_get(Pool *pool, s64 nbytes);
TINYRT_EXTERN void pool_release(Pool *pool);
TINYRT_EXTERN void pool_reset(Pool *pool);

TINYRT_EXTERN ALLOCATOR_PROC(pool_allocator_proc);

#endif  // GENERAL_POOL_INCLUDE_H


#ifdef POOL_IMPLEMENTATION

static void pool_cycle_new_block(Pool *pool) {
    if (!pool->block_allocator.proc) {
        write_string("You must call set_allocators or pool_init before using it!\n", true);
        assert(false);
    }

    Allocator a = pool->block_allocator;

    u8 *new_block;
    if (pool->unused_memblocks.count) {
        array_pop(&pool->unused_memblocks, &new_block);
    } else {
        assert(a.proc != null);
        new_block = (u8 *)a.proc(ALLOCATOR_ALLOCATE, pool->memblock_size, 0, null, a.data);
    }

    pool->bytes_left = pool->memblock_size;
    pool->current_memblock = new_block;
    pool->current_pos      = new_block;
}

void set_allocators(Pool *pool, Allocator block_allocator, Allocator array_allocator) {
    if (!block_allocator.proc) {
        block_allocator.proc = heap_allocator;
        block_allocator.data = null;
    }

    if (!array_allocator.proc) {
        array_allocator.proc = heap_allocator;
        array_allocator.data = null;
    }

    pool->block_allocator = block_allocator;

    pool->used_memblocks.allocator   = array_allocator;
    pool->unused_memblocks.allocator = array_allocator;
}

TINYRT_EXTERN void *pool_get(Pool *pool, s64 nbytes) {
    assert(pool != null);

    s64 extra = (pool->alignment - (nbytes % pool->alignment)) % pool->alignment;
    nbytes += extra;

    // @Todo: Check large allocations.

    if (pool->bytes_left < nbytes) {
        pool_cycle_new_block(pool);
        if (!pool->current_memblock) return null;
    }

    void *result = pool->current_pos;
    pool->current_pos += nbytes;
    pool->bytes_left  -= nbytes;
    return result;
}

TINYRT_EXTERN void pool_release(Pool *pool) {
    pool_reset(pool);

    assert(pool->block_allocator.proc != null);
    for (s64 index = 0; index < pool->unused_memblocks.count; ++index) {
        u8 *it = pool->unused_memblocks[index];
        pool->block_allocator.proc(ALLOCATOR_FREE, 0, 0, it, pool->block_allocator.data);
    }
    pool->unused_memblocks.count = 0;
}

TINYRT_EXTERN void pool_reset(Pool *pool) {
    if (pool->current_memblock) {
        array_add(&pool->unused_memblocks, pool->current_memblock);
        pool->current_memblock = null;
    }

    for (s64 index = 0; index < pool->used_memblocks.count; ++index) {
        array_add(&pool->unused_memblocks, pool->used_memblocks[index]);
    }
    pool->used_memblocks.count = 0;
}

TINYRT_EXTERN ALLOCATOR_PROC(pool_allocator_proc) {
    UNUSED(old_size);
    UNUSED(old_memory);

    Pool *pool = (Pool *)allocator_data;
    assert(pool != null);

    switch (mode) {
        case ALLOCATOR_ALLOCATE:
            return pool_get(pool, size);

        case ALLOCATOR_RESIZE:
            // We do not resize our pools.
            assert(false);
            return null;

        case ALLOCATOR_FREE:
            // Not supported.
            assert(false);
            return null;

        case ALLOCATOR_FREE_ALL: {
            pool_release(pool);
            return null;
        } break;

        default:
            assert(false);
            return null;
    }
}

#endif  // POOL_IMPLEMENTATION
