#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int array[], int low, int high) {
    int pivot = array[high];
    int i = (low - 1);

    for (int j = low; j < high; j++) {
        if (array[j] <= pivot) {
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[high]);
    return (i + 1);
}

void quickSort(int array[], int low, int high) {
    if (low < high) {
        int pi = partition(array, low, high);
        quickSort(array, low, pi - 1);
        quickSort(array, pi + 1, high);
    }
}

void printArray(int array[], int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d  ", array[i]);
    }
    printf("\n");
}

int main() {

    clock_t start, end, cpu_time_used;
    srand((unsigned int)time(NULL)); 

    int N = 10000; 
    int data[N]; 
    
    printf("Unsorted Array\n");
    for (int i = 0; i < N; ++i) {
        data[i] = rand() % 100000;
        printf("%d  ", data[i]);
    }
    printf("\n");

    start = clock();
    quickSort(data, 0, N - 1);
    end = clock();

    printf("Sorted array in ascending order: \n");
    printArray(data, N);

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
     printf("Time taken by CPU in miliseconds: %ld \n", cpu_time_used);

    return 0;
}
