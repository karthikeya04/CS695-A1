#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    int stage = 1;
    size_t page_size = getpagesize();
    size_t total_allocated = 0;

    printf("Before any allocations...\n");

    printf("Enter any digit to proceed\n");
    int x; scanf("%d", &x); // wait

    size_t size_to_allocate = stage * page_size * 200;

    while (1) {
        // Allocate memory in multiples of page size
        void *memory_block = malloc(size_to_allocate);

        if (memory_block == NULL) {
            fprintf(stderr, "Memory allocation failed at stage %d\n", stage);
            break;
        }

        total_allocated += size_to_allocate;

        printf("Stage %d: Allocated %zu bytes of memory\n", stage, size_to_allocate);

        printf("Enter\n 1: to touch\n 2: to break\n any other number: to allocate more\n");
        int x; scanf("%d", &x); // wait
        if(x == 1)
        {
            int* ptr = (int*) memory_block;
            for(int i=0;i < size_to_allocate/sizeof(int);i++)
            {
                ptr[i] = 101;
            }
            printf("Enter any digit to proceed\n");
            scanf("%d", &x); // wait
        }
        else if(x == 2)
        {
            continue;
        }
    }

    printf("Total allocated memory: %zu bytes\n", total_allocated);

    return 0;
}
