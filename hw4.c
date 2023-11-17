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

//the ptr is the index of the header
//block size is the size of the payload
void coalesce(int ptr, int blockSize) {
    // Forward coalesce: Check if there are adjacent free blocks 
    // after going past the block size, and if there are, merge the free blocks together

    int next = ptr + blockSize+1; // Get the address of the next block
    // printf("pointer: %d\n", ptr);
    // printf("heap[next]: %d next: %d\n", heap[next], next);
    // printf("heap[next]&1 %d\n",heap[next]&1);

    if ((heap[next] & 1) == 0){

        // Handle sequences of zero bytes
        int start = ptr + blockSize; // Start from the end of the coalesced block

        for (int i = start; i < HEAP_SIZE; ++i) {
        if (heap[i] != 0 || i == HEAP_SIZE-1) {
                //printf("i: %d\n",i);

                // End of zero byte sequence, merge with the previous block
                blockSize += (i - start);
                heap[ptr] = (blockSize << 1);
                
                set_allocated(ptr, false);
                break;
            }
        }

        bool flag = 0;
        int newPtr = 0;
        while (next < HEAP_SIZE && heap[next] != 0 && ((heap[next] & 1) == 0)) {
            newPtr = ptr;
            bool flag = 1;
            int nextBlockSize = (heap[next] >> 1) & 0x7F; // Extract the size from the next block's header
            //printf("nextBlockSize: %d\n",nextBlockSize);
            heap[next] = 0;
            // Merge the current and next blocks
            //printf("merged: %d\n", ((blockSize + nextBlockSize) << 1) + 2);
            heap[ptr] = ((blockSize + nextBlockSize) << 1) + 2;

            // Move to the next block
            blockSize += nextBlockSize; // Update the merged block's size
            next = ptr + blockSize; // Recalculate the address of the next block
        }
    }

}



int mallocBlock(int size) {
    int x = 0;
    int prevBlockEnd = 0;


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
        // printf("%d\n", ((heap[x] >> 1) & 0x7F) + 1);
        x += ((heap[x] >> 1) & 0x7F) + 1; // Move to the next block using the size from the header (considering the first 7 bits)
    }
    //best fit 
    // Shift all allocated blocks towards the beginning of the heap
    int y = 0;
    while(y < HEAP_SIZE){
        //printf("y = %d, size = %d\n", y , (heap[y] >> 1) & 0x7F);
        if((heap[y] & 1) == 0){
            // int block_size = heap[y] >> 1;
            int free_block_size = ((heap[y] >> 1) & 0x7F);
            int free_block_index = y;
            int nextBlock = y + free_block_size + 1;
            //printf("free_block_size[1] = %d\n", free_block_size);
            // printf("nextBlock = %d\n", nextBlock);
            //printf("next block size = %d, heap[nextBlock]&1= %d \n", heap[nextBlock], heap[nextBlock] & 1);
            //printf("free_block_index = %d\n", free_block_index);
            while (nextBlock < HEAP_SIZE && (heap[nextBlock] & 1) == 0) {
                free_block_size += ((heap[nextBlock] >> 1) & 0x7F); // Calculate the combined size of adjacent free blocks
                //printf("free_block_size = %d", free_block_size);
                nextBlock += ((heap[nextBlock] >> 1) & 0x7F) + 1; // Move to the next block
                //printf("nextBlock = %d\n", nextBlock);
            }
            if ((heap[nextBlock] & 1) == 1){
                int allocated_block_size = (heap[nextBlock] >> 1) & 0x7F;

                // Copy the allocated block to the start of the free block
                memcpy(&heap[free_block_index], &heap[nextBlock], allocated_block_size);

                // Mark the destination block as allocated
                heap[free_block_index] = (allocated_block_size << 1) | 1;

                // Update the nextBlock pointer
                nextBlock = free_block_index + allocated_block_size + 1;

                for (int i = y + allocated_block_size; i < HEAP_SIZE - free_block_size; ++i) {
                    heap[i] = heap[i + free_block_size]; // Shift the block contents free_block_size bytes backward
                }
            }
        }
        y += ((heap[y] >> 1) & 0x7F) + 1; // Move to the next block using the size from the header (considering the first 7 bits)
    }
    x = 0;
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
        x += ((heap[x] >> 1) & 0x7F) + 1; // Move to the next block using the size from the header (considering the first 7 bits)
    }
    return -1;
}

void freeBlock(int ptr){
    int blockHeader = ptr - 1; // Calculate the header index from the pointer
    int blockSize = ((heap[blockHeader] >> 1) & 0x7F); //gets the size of the block excluding the header 
    //printf("block size: %d\n", blockSize);
    int headerPayload = blockSize + blockHeader;
    set_allocated(blockHeader, false); //make the header have LSB 0 to show it's not allocated 
    for(int i = ptr; i<=headerPayload; i++){ //iterates from ptr to the last allocated block and sets the value to 0
        //printf("index: %d, value: %d \n", i, heap[i]);
        heap[i] = 0; //sets everything inlcuding header to 0
    }
    coalesce(blockHeader, blockSize); //after freeing coelesce from the freed header 
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
            freeBlock(ptr);//frees the old heder 

            // heap[ptr - 1] = 0; // Free the old block
            // memset(&heap[ptr], 0, block_size); //Set the old block to zeros
            heap[newPtr - 1] = (newSize << 1) | 1;
        }



        return newPtr;
    }
}






void blockList(){
    int start = 0;

    while (start < HEAP_SIZE){
        if ((heap[start] & 1) == 1){ //if the LSB is 1 that means it's a header
            int blockSize = ((heap[start] >> 1) & 0x7F);
            //print from start+1 (payload) to start + blockSize (end of payload)
            printf("%d, %d, allocated\n", start+1, blockSize);      
            start = start + blockSize + 1;   
        }else{ //not allocated
            int secondStart = start;
            int end = secondStart;
            while (end < HEAP_SIZE && ((heap[end] & 1) == 0 || heap[end] == 0)){
                end++;
            }
            printf("%d, %d, free\n", secondStart+1, end - secondStart-1); 
            start = end;         
        }


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
            //printf("payload: %d freed\n",ptr);
        }else if (strcmp(command, "blocklist") == 0){
            blockList();

        }else if (strcmp(command, "printheap") == 0){
            for (int i = 0; i < HEAP_SIZE; i++ ){
                printf("%d\n", heap[i]);
            }
        }else if (strcmp(command, "writemem") == 0){
            int ptr;
            int i =0;
            char input[127];
            scanf("%d %s", &ptr, input);

            while (input[i] != '\0'){
                heap[ptr] = input[i];
                ptr++;
                i++;
            }
        }else if (strcmp(command, "printmem") == 0){
            int ptr, size;
            int i =0;
            scanf("%d %d", &ptr, &size);

            while (i < size){
                printf("%x ", heap[ptr+i]);
                i++;
            }
            printf("\n");
        }else if (strcmp(command, "quit") == 0){
            break;
        }else {
            printf("Invalid command\n");
        }
    }

    return 0;
}


