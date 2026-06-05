#include <stdio.h>
#include <stdlib.h>

int main() {
    int capacity = 2; // Initial tracking capacity
    int count = 0;    // Number of elements added
    int input;

    // 1. Initial allocation on the heap
    int *arr = malloc(capacity * sizeof(int));
    if (arr == NULL) {
        perror("Initial allocation failed");
        return 1;
    }

    printf("Enter integers (type -1 to stop):\n");

    while (scanf("%d", &input) == 1 && input != -1) {
        // 2. Check if the array is full
        if (count >= capacity) {
            capacity *= 2; // Double the capacity strategy

            // 3. MANDATORY: Assign to a temporary pointer first
            int *temp = realloc(arr, capacity * sizeof(int));

            // 4. Handle allocation failure safely
            if (temp == NULL) {
                perror("Reallocation failed! Keeping original data.");
                free(arr); // Clean up original buffer before exiting
                return 1;
            }

            // 5. Transfer pointer only after confirming success
            arr = temp;
            printf("--- Array expanded to capacity: %d ---\n", capacity);
        }

        // Add the element to the dynamically resized array
        arr[count++] = input;
    }

    // Print results
    printf("\nYou entered:\n");
    for (int i = 0; i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // 6. Clean up memory
    free(arr);
    return 0;
}
