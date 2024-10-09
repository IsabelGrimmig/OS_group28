/**
 * @file   mm.c
 * @Author 02335 team
 * @date   September, 2024
 * @brief  Memory management skeleton.
 * 
 */

#include <stdint.h>

#include "mm.h"



/* Proposed data structure elements */

typedef struct header {
  struct header * next;     // Bit 0 is used to indicate free block 
  uint64_t user_block[0];   // Standard trick: Empty array to make sure start of user block is aligned
} BlockHeader;

/* Macros to handle the free flag at bit 0 of the next pointer of header pointed at by p */
#define GET_NEXT(p)    (BlockHeader *)((uintptr_t)(p->next) & ~0x1)  // Mask out the free flag
#define SET_NEXT(p,n)  p->next = (BlockHeader *)(((uintptr_t)n & ~0x1) | ((uintptr_t)p->next & 0x1))  // Preserve the free flag
#define GET_FREE(p)    (uint8_t) ( (uintptr_t) (p->next) & 0x1 )   /* OK -- do not change */
#define SET_FREE(p,f)  p->next = (BlockHeader *)(((uintptr_t)p->next & ~0x1) | (f & 0x1))
#define SIZE(p)    ((uintptr_t)(GET_NEXT(p)) - (uintptr_t)(p) - sizeof(BlockHeader))

#define MIN_SIZE     (8)   // A block should have at least 8 bytes available for the user

void split_block(BlockHeader *block, size_t size);
void coalesce(BlockHeader *block);



static BlockHeader * first = NULL;
static BlockHeader * current = NULL;

/**
 * @name    simple_init
 * @brief   Initialize the block structure within the available memory
 *
 */
void simple_init() {
    uintptr_t aligned_memory_start = (memory_start + 7) & ~0x07;
    uintptr_t aligned_memory_end = memory_end & ~0x07;

    if (aligned_memory_start + 2 * sizeof(BlockHeader) + MIN_SIZE <= aligned_memory_end) {
        first = (BlockHeader *)aligned_memory_start;
        SET_NEXT(first, (BlockHeader *)(aligned_memory_end - sizeof(BlockHeader)));
        SET_FREE(first, 1);

        BlockHeader *dummy = (BlockHeader *)(aligned_memory_end - sizeof(BlockHeader));
        SET_NEXT(dummy, first);
        SET_FREE(dummy, 0);

        current = first;
    } else {
        first = NULL;
    }
}




/**
 * @name    simple_malloc
 * @brief   Allocate at least size contiguous bytes of memory and return a pointer to the first byte.
 *
 * This function should behave similar to a normal malloc implementation. 
 *
 * @param size_t size Number of bytes to allocate.
 * @retval Pointer to the start of the allocated memory or NULL if not possible.
 *
 */
void* simple_malloc(size_t size) {
    if (first == NULL) {
        simple_init();
        if (first == NULL) return NULL;
    }

    // Align the requested size to 8 bytes
    size_t aligned_size = (size + 7) & ~0x07;

    // Search for a free block
    BlockHeader *search_start = current;
    do {
        if (GET_FREE(current)) {
            // Check if free block is large enough
            if (SIZE(current) >= aligned_size) {
                // Will the remainder be large enough for a new block?
                if (SIZE(current) - aligned_size >= sizeof(BlockHeader) + MIN_SIZE) {
                    // Split the block
                    split_block(current, aligned_size);
                }
                // Mark block as allocated (non-free)
                SET_FREE(current, 0);
                
                // Update current pointer for next-fit allocation
                BlockHeader* allocated_block = current;
                current = GET_NEXT(current);  // Move to the next block after allocation
                
                // Return the pointer to the start of the memory area after the block header
                return (void *)(allocated_block + 1);
            }
        }
        current = GET_NEXT(current);  // Move to the next block
    } while (current != search_start);  // Loop until all blocks are checked

    // No suitable block found
    return NULL;
}




/**
 * @name    simple_free
 * @brief   Frees previously allocated memory and makes it available for subsequent calls to simple_malloc
 *
 * This function should behave similar to a normal free implementation. 
 *
 * @param void *ptr Pointer to the memory to free.
 *
 */
void simple_free(void *ptr) {
    if (!ptr) return;  // Null check

    // Find the block header corresponding to the pointer
    BlockHeader *block = (BlockHeader *)ptr - 1;  // The block header is just before the memory area

    // Mark the block as free
    SET_FREE(block, 1);

    // Coalesce with adjacent free blocks if possible
    coalesce(block);
}


void split_block(BlockHeader *block, size_t size) {
    BlockHeader *new_block = (BlockHeader *)((uintptr_t)block + sizeof(BlockHeader) + size);
    SET_NEXT(new_block, GET_NEXT(block));  // Link the new block to the next block
    SET_NEXT(block, new_block);  // Link the current block to the new block
    SET_FREE(new_block, 1);  // Mark the new block as free
}

void coalesce(BlockHeader *block) {
    // Get the next block
    BlockHeader *next = GET_NEXT(block);

    // Check if the next block is free
    if (next != first && GET_FREE(next)) {
        // Merge the current block with the next block
        SET_NEXT(block, GET_NEXT(next));  // Point the current block to the block after the next one
    }

}



