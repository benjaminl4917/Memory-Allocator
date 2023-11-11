#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_SIZE 127

typedef struct {
    unsigned char *heap;
    int heap_size;
    int free_list_size;
    int *free_list;
} HeapManager;

void initializeHeap(HeapManager *manager) {
    manager->heap = (unsigned char *)malloc(HEAP_SIZE);
    manager->heap_size = HEAP_SIZE;
    manager->free_list_size = 1;
    manager->free_list = (int *)malloc(sizeof(int) * 2);
    manager->free_list[0] = 0;
    manager->free_list[1] = HEAP_SIZE;
}

int mallocBlock(HeapManager *manager, int size) {
    for (int i = 0; i < manager->free_list_size; i += 2) {
        int start = manager->free_list[i];
        int block_size = manager->free_list[i + 1];
        if (block_size >= size) {
            if (block_size == size) {
                // Remove the block from the free list
                for (int j = i; j < manager->free_list_size - 2; j++) {
                    manager->free_list[j] = manager->free_list[j + 2];
                }
                manager->free_list_size -= 2;
            } else {
                // Update the block in the free list
                manager->free_list[i] = start + size;
                manager->free_list[i + 1] = block_size - size;
            }
            // Mark the block as allocated
            manager->heap[start] = size | 0x01;
            return start + 1;
        }
    }
    return -1; // Unable to allocate block
}


int main() {
    HeapManager manager;
    initializeHeap(&manager);



    char command[10];
    while (1) {
        printf(">");
        scanf("%s", command);

        if (strcmp(command, "malloc") == 0) {
            int size;
            scanf("%d", &size);
            int ptr = mallocBlock(&manager, size);
            printf("%d\n", ptr);
        } else {
            printf("Invalid command\n");
        }
    }

    return 0;
}