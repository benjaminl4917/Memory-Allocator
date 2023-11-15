#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>


#define HEAP_SIZE 127

unsigned char heap[HEAP_SIZE];

void set_allocated(int header, bool allocated) {
    if (allocated) {
        heap[header] |= 1;
    } else{
        heap[header] &= ~1;
    }
}

void heap_initialization(){
    memset(heap, 0, HEAP_SIZE);
    heap[0] = HEAP_SIZE << 1;
    set_allocated(0, false);
}


int mallocBlock(int size) {
    int x = 0;
    while (x < HEAP_SIZE) {
        if ((heap[x] & 1) == 0) {
            int block_size = heap[x] >> 1; // Extract the size from the first 7 bits
            if (block_size >= size) {
                heap[x] = (size << 1) | 1; // Set the size in the first 7 bits and mark as allocated (set LSB to 1)
                return x + 1; // Return the start address of the allocated block
            } else {
                int nextBlock = x + ((heap[x] >> 1) & 0x7F) + 1; // Get the address of the next block
                while (nextBlock < HEAP_SIZE && (heap[nextBlock] & 1) == 0) {
                    block_size += ((heap[nextBlock] >> 1) & 0x7F) + 1; // Calculate the combined size of adjacent free blocks
                    if (block_size >= size) {
                        heap[x] = (block_size << 1) | 1; // Allocate the combined blocks
                        return x + 1; // Return the start address of the allocated block
                    }
                    nextBlock += ((heap[nextBlock] >> 1) & 0x7F) + 1; // Move to the next block
                }
            }
        }
        printf("%d\n", ((heap[x] >> 1) & 0x7F) + 1);
        x += ((heap[x] >> 1) & 0x7F) + 1; // Move to the next block using the size from the header (considering the first 7 bits)
    }
    //best fit 
    return -1;
}

int reallocBlock(int ptr, int newSize) {
    int blockHeader = ptr - 1; // Calculate the header index from the pointer

    if (ptr < 1 || blockHeader >= HEAP_SIZE || (heap[blockHeader] & 1) == 0) {
        // Invalid pointer or block is not allocated
        return -1;
    }

    int block_size = heap[blockHeader] >> 1; // Extract the size from the header

    if (block_size >= newSize) {
        // Truncate the block if the new size is smaller than the current size
        heap[blockHeader] = (newSize << 1) | 1;
        return ptr;
    } else {
        int nextBlock = blockHeader + block_size + 1; // Get the address of the next block

        while (nextBlock < HEAP_SIZE && (heap[nextBlock] & 1) == 0) {
            block_size += ((heap[nextBlock] >> 1) & 0x7F) + 1; // Calculate the combined size of adjacent free blocks

            if (block_size >= newSize) {
                // Expand the block into adjacent free space
                heap[blockHeader] = (block_size << 1) | 1; // Allocate the expanded blocks
                return ptr;
            }
            nextBlock += ((heap[nextBlock] >> 1) & 0x7F) + 1; // Move to the next block
        }

        // Allocate a new block if there is not enough adjacent free space
        int newPtr = mallocBlock(newSize);
        if (newPtr != -1) {
            memcpy(&heap[newPtr - 1], &heap[ptr - 1], block_size); // Copy contents to the new block
            heap[ptr - 1] = 0; // Free the old block
        }
        return newPtr;
    }
}

int freeBlock(int ptr){
    int blockHeader = ptr - 1; // Calculate the header index from the pointer
    int blockSize = ((heap[blockHeader] >> 1) & 0x7F); //gets the size of the block excluding the header 
    //printf("block size: %d\n", blockSize);
    int headerPayload = blockSize + blockHeader;
    for(int i = blockHeader; i<=headerPayload; i++){ //iterates from ptr to the last allocated block and sets the value to 0
        //printf("index: %d\n", i);
        heap[i] = 0; //sets everything inlcuding header to 0
    }
}


//00000001 1 2 3 4 5 6 7 8 9 10
//Header 1 2 3 4 5 6 7 8 9 10
//Header1 ========== Header2 ============ _________________ 
int main() {

    heap_initialization();
    char command[10];
    while (1) {
        printf(">");
        scanf("%s", command);

        if (strcmp(command, "malloc") == 0) {
            int size, start;
            int x = 0;
            scanf("%d", &size);
            //when heap is still empty just change the header 
            
            int ptr = mallocBlock(size);
            printf("%d\n", ptr);
        } else if (strcmp(command, "realloc") == 0) {
            int ptr, newSize;
            scanf("%d %d", &ptr, &newSize);
            int newPtr = reallocBlock(ptr, newSize);
            printf("%d\n", newPtr);
        }else if (strcmp(command, "free") == 0){
            int ptr;
            scanf("%d", &ptr);
            freeBlock(ptr);
            printf("payload: %d freed\n",ptr);


        }else if (strcmp(command, "printheap") == 0){
            for (int i = 0; i < HEAP_SIZE; i++ ){
                printf("%d\n", heap[i]);
            }
        }else {
            printf("Invalid command\n");
        }
    }

    return 0;
}