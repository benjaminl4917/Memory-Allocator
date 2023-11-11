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

// int mallocBlock(manager, int size) {
//     for (int i = 0; i < manager->free_list_size; i += 2) {
//         int start = manager->free_list[i];
//         int block_size = manager->free_list[i + 1];
//         if (block_size >= size) {
//             if (block_size == size) {
//                 // Remove the block from the free list
//                 for (int j = i; j < manager->free_list_size - 2; j++) {
//                     manager->free_list[j] = manager->free_list[j + 2];
//                 }
//                 manager->free_list_size -= 2;
//             } else {
//                 // Update the block in the free list
//                 manager->free_list[i] = start + size;
//                 manager->free_list[i + 1] = block_size - size;
//             }
//             // Mark the block as allocated
//             manager->heap[start] = size | 0x01;
//             return start + 1;
//         }
//     }
//     return -1; // Unable to allocate block
// }


int main() {

    heap_initialization();

    // char command[10];
    // while (1) {
    //     printf(">");
    //     scanf("%s", command);

    //     if (strcmp(command, "malloc") == 0) {
    //         int size;
    //         scanf("%d", &size);
    //         int ptr = mallocBlock(&manager, size);
    //         printf("%d\n", ptr);
    //     } else {
    //         printf("Invalid command\n");
    //     }
    // }

    return 0;
}