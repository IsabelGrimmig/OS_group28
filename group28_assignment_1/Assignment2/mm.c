#include <stdint.h>
#include <stdio.h>
#include "mm.h"

typedef struct header {
    struct header *next;     // Bit 0 is used to indicate free block 
    uint64_t user_block;  // Empty array to ensure user block is aligned
} BlockHeader;

/* Macros to handle the free flag and next pointer */
#define GET_NEXT(p)    (BlockHeader *)((uintptr_t)(p->next) & ~0x1)
#define SET_NEXT(p, n) p->next = (BlockHeader *)(((uintptr_t)n & ~0x1) | ((uintptr_t)p->next & 0x1))
#define GET_FREE(p)    (uint8_t)((uintptr_t)(p->next) & 0x1)
#define SET_FREE(p, f) p->next = (BlockHeader *)(((uintptr_t)p->next & ~0x1) | (f & 0x1))
#define SIZE(p)        ((uintptr_t)(GET_NEXT(p)) - (uintptr_t)(p) - sizeof(BlockHeader))

#define MIN_SIZE       (8)

void split_block(BlockHeader *block, size_t size);
void coalesce(BlockHeader *block);
void coalesce_all_blocks();  // New function for memory defragmentation

static BlockHeader *first = NULL;
static BlockHeader *current = NULL;

/* Initialize the memory block structure */
void simple_init() {
    uintptr_t aligned_memory_start = (memory_start + 7) & ~0x07;
    uintptr_t aligned_memory_end = memory_end & ~0x07;

    // Check if there is enough memory for at least one block and the dummy block
    if (aligned_memory_start + 2 * sizeof(BlockHeader) + MIN_SIZE > aligned_memory_end) {
        printf("Error: Not enough memory to initialize the block structure.\n");
        first = NULL;  // Mark initialization failure
        return;
    }

    first = (BlockHeader *)aligned_memory_start;
    SET_NEXT(first, (BlockHeader *)(aligned_memory_end - sizeof(BlockHeader)));
    SET_FREE(first, 1);

    BlockHeader *dummy = (BlockHeader *)(aligned_memory_end - sizeof(BlockHeader));
    SET_NEXT(dummy, first);
    SET_FREE(dummy, 0);

    current = first;
}

/* Allocates a block of memory */
void* simple_malloc(size_t size) {
    printf("Requesting allocation of size: %zu\n", size);

    if (first == NULL) {
        simple_init();
        if (first == NULL) {
            printf("Memory initialization failed! Not enough memory to set up the block structure.\n");
            return NULL;
        }
    }

    // Align the requested size to 8 bytes
    size_t aligned_size = (size + 7) & ~0x07;
    printf("Aligned size: %zu\n", aligned_size);

    BlockHeader *search_start = current;
    int search_attempts = 0;  // Track the number of cycles through memory

    while (search_attempts < 2) {  // Allow two full cycles through memory to avoid fragmentation
        printf("Checking block at %p with size %zu, free status: %d\n", current, SIZE(current), GET_FREE(current));

        if (GET_FREE(current) && SIZE(current) >= aligned_size) {
            printf("Found a free block at %p of size %zu\n", current, SIZE(current));

            if (SIZE(current) == aligned_size) {
                // Exact fit, no need to split
                SET_FREE(current, 0);
                BlockHeader *allocated_block = current;
                current = GET_NEXT(current);
                printf("Allocated exact-fit block at %p\n", allocated_block);
                return (void *)(allocated_block + 1);
            } else if (SIZE(current) - aligned_size >= sizeof(BlockHeader) + MIN_SIZE) {
                // Split the block if remaining size is enough
                printf("Splitting block at %p\n", current);
                split_block(current, aligned_size);
            }

            SET_FREE(current, 0);
            BlockHeader *allocated_block = current;
            current = GET_NEXT(current);
            printf("Allocated block at %p with size %zu\n", allocated_block, SIZE(allocated_block));
            return (void *)(allocated_block + 1);  // Return pointer to memory region after header
        }

        current = GET_NEXT(current);  // Move to the next block
        printf("Moving to next block: %p\n", current);

        // If we complete one full cycle, attempt another to check for potential memory compaction opportunities
        if (current == search_start) {
            search_attempts++;
            printf("Completed one cycle through memory, retrying to avoid fragmentation.\n");
            if (search_attempts == 2) {
                printf("Attempting to coalesce all blocks to defragment memory.\n");
                coalesce_all_blocks();  // New defragmentation attempt
                current = first;  // Start over after coalescing
            }
        }
    }

    // If no suitable block is found after two full cycles
    printf("No suitable block found for size %zu after multiple attempts. Memory allocation failed!\n", aligned_size);
    return NULL;  // Return NULL to indicate memory allocation failure
}

/* Frees the allocated block */
void simple_free(void *ptr) {
    if (!ptr) return;

    BlockHeader *block = (BlockHeader *)ptr - 1;
    SET_FREE(block, 1);
    coalesce(block);
}

/* Splits a block */
void split_block(BlockHeader *block, size_t size) {
    size_t remaining_size = SIZE(block) - size - sizeof(BlockHeader);

    // Only split if the remaining size is large enough
    if (remaining_size >= sizeof(BlockHeader) + MIN_SIZE) {
        BlockHeader *new_block = (BlockHeader *)((uintptr_t)block + sizeof(BlockHeader) + size);
        SET_NEXT(new_block, GET_NEXT(block));
        SET_NEXT(block, new_block);
        SET_FREE(new_block, 1);  // Mark the new block as free
        printf("Block at %p split into new block at %p with remaining size %zu\n", block, new_block, remaining_size);
    }
}

/* Coalesces adjacent free blocks */
void coalesce(BlockHeader *block) {
    BlockHeader *next = GET_NEXT(block);
    
    // Ensure we don't coalesce past the last block
    while (GET_FREE(next) && (uintptr_t)next >= (uintptr_t)first && (uintptr_t)next < (uintptr_t)memory_end) {
        printf("Coalescing block at %p with next block at %p\n", block, next);
        SET_NEXT(block, GET_NEXT(next));  // Merge the current block with the next block
        next = GET_NEXT(block);  // Move to the next block and check again

        // Add a safety check to prevent infinite loops
        if (next == block) {
            printf("Warning: Detected potential infinite loop in coalesce function.\n");
            break;
        }
    }

    printf("Final coalesced block at %p with size %zu\n", block, SIZE(block));
}

/* Defragmentation: Coalesces all free blocks */
void coalesce_all_blocks() {
    BlockHeader *block = first;
    BlockHeader *start = first;  // Keep track of the starting block to detect a full cycle

    do {
        if (GET_FREE(block)) {
            coalesce(block);  // Try to coalesce all free blocks
        }
        block = GET_NEXT(block);

        // Add a safety check to prevent infinite loops
        if (block == start) {
            printf("Warning: Detected potential infinite loop in coalesce_all_blocks function.\n");
            break;
        }
    } while ((uintptr_t)block >= (uintptr_t)first && (uintptr_t)block < (uintptr_t)memory_end);
}

