#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>

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

int convert(long long n) {
  int dec = 0, i = 0, rem;

  while (n != 0) {

    // get remainder of n divided by 10
    rem = n % 10;

    // divide n by 10
    n /= 10;

    // multiply rem by (2 ^ i)
    // add the product to dec
    dec += rem * pow(2, i);

    // increment i
    ++i;
  }

  return dec;
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
        x += ((heap[x] >> 1) & 0x7F) + 1; // Move to the next block using the size from the header (considering the first 7 bits)
    }
    //best fit 
    return -1;
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
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}