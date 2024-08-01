#include "mpi.h"
#include<stdio.h>
#include <stdlib.h>
#include "math.h"
#include <stdbool.h>
#include <time.h>

#define SIZE 1000000
void generate_random_array(int *arr, int size) {
    for (int j = 0; j < size; ++j) {
        arr[j] = (int) rand() % 100000;
    }
}

int hoare_partition(int *arr, int low, int high){
    int middle = floor((low+high)/2);
    int pivot = arr[middle];
    int j,temp;

    temp=arr[middle];  
    arr[middle]=arr[high];
    arr[high]=temp;

    int i = (low - 1);
    for (j=low;j<=high-1;j++){
        if(arr[j] < pivot){
            i++;
            temp=arr[i];  
            arr[i]=arr[j];
            arr[j]=temp;	
        }
    }

    temp=arr[i+1];  
    arr[i+1]=arr[high];
    arr[high]=temp; 

    return (i+1);
}


void quicksort(int *number,int first,int last){
    if(first<last){
        int pivot_index = hoare_partition(number, first, last);
        quicksort(number,first,pivot_index-1);
        quicksort(number,pivot_index+1,last);
    }
}


void merge(int *first,int *second, int *result,int first_size,int second_size){
	int i=0;
	int j=0;
	int k=0;

	while (i < first_size && j < second_size) {
        if (first[i] < second[j]) {
            result[k++] = first[i++];
        } else {
            result[k++] = second[j++];
        }
    }

    while (i < first_size) {
        result[k++] = first[i++];
    }

    while (j < second_size) {
        result[k++] = second[j++];
    }
}

int main(int argc, char *argv[]) {
	
    int *unsorted_array = (int *)malloc(SIZE * sizeof(int));
    int *result = (int *)malloc(SIZE * sizeof(int));
    int array_size = SIZE;
    int size, rank;
    int sub_array_size;
	int own_chunk_size;
    int number_of_process;
	double start_time, end_time;
	MPI_Status status;    

	// Start parallel execution
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if(rank==0){
    	generate_random_array(unsorted_array, SIZE);
	}
	 
	MPI_Barrier(MPI_COMM_WORLD);
	clock_t start, end;
	start_time = MPI_Wtime();
	int iter_count = size; 
	sub_array_size = (SIZE % iter_count  == 0) ? (SIZE / iter_count) : SIZE / (iter_count  - 1);
	own_chunk_size = (SIZE >= sub_array_size * (rank + 1)) ? sub_array_size : (SIZE - sub_array_size * rank);
	int chunk_size;

	// Cluster 0 (Master) splits the array and sends each subarray to the respective machine
	if( rank == 0 ){
		start = clock();
		
		int i =0;
		if(iter_count > 1){
			// ==============================================SENDING DATA==============================================
			for(i=0;i<iter_count;i++){
				chunk_size = (SIZE >= sub_array_size * (i + 1)) ? sub_array_size : (SIZE - sub_array_size * (i));
				if (i > 0){
				//send the subarray
				MPI_Send(&unsorted_array[i*sub_array_size],chunk_size,MPI_INT,i,0,MPI_COMM_WORLD); 
				}
				
			}
		
			// ========================================CALCULATE FIRST SUBARRAY========================================
			int i =0;
			int *sub_array = (int *)malloc(sub_array_size*sizeof(int));
			for(i=0;i<sub_array_size;i++){
				// Passing the first sub array since rank 0 always calculates the first sub array
				sub_array[i]=unsorted_array[i];
			}
			// Sequentially sorting the first array
			quicksort(sub_array,0,sub_array_size-1);
			
			// =============================================RECEIVING DATA=============================================
		
			for (i=0;i<iter_count;i++){
				if(i > 0){ 
					
					int received_chunk_size = (SIZE >= sub_array_size * (i + 1)) ? sub_array_size : (SIZE - sub_array_size * i);
					int temp_sub_array[received_chunk_size];

					// Receive each subarray
					MPI_Recv(temp_sub_array,received_chunk_size,MPI_INT,i,777,MPI_COMM_WORLD,&status);
					// printf("\nReceived %d elment from Process No. %d\n", received_chunk_size, i);
					//printf("%d Elements in received array:\n", i);
	
					int j;
					int temp_size;
					temp_size = i*sub_array_size;
					int temp_result[temp_size];

					for(j=0;j<temp_size;j++){
						temp_result[j]=result[j];
					}
					// Merge it back into the result array
					merge(temp_sub_array,temp_result,result,received_chunk_size,temp_size);
					// printf("Elements in result  after merge main array:\n");
					// for (int j = 0; j < SIZE; j++) {
    				// 	printf("%d ", result[j]);
					// }

				}else{
					// On first iteration we just pass the sorted elements to the result array
					int j;	
					for(j=0;j<sub_array_size;j++){	
						result[j]=sub_array[j];
					}		
					free(sub_array);
				}
			}
		}else{
			// if it runs only in a single Cluster
			quicksort(unsorted_array,0,SIZE-1);
			for(i=0;i<SIZE;i++){
				result[i]=unsorted_array[i];
			}
		}
		end = clock();
		end_time = MPI_Wtime();
		printf("End Result: \n");
	}else{
		// All the other Clsuters have to sort the data and send it back
		int *sub_array = (int *)malloc(own_chunk_size*sizeof(int));
		MPI_Recv(sub_array,own_chunk_size,MPI_INT,0,0,MPI_COMM_WORLD,&status);	
		quicksort(sub_array,0,own_chunk_size-1);
		int i=0;
		MPI_Send(sub_array,own_chunk_size,MPI_INT,0,777,MPI_COMM_WORLD);//sends the data back to rank 0	
		free(sub_array);
	}

	if(rank==0){
        // --- VALIDATION CHECK ---
        printf("Checking.. \n");
        bool error = false;
        int i=0;
        for(i=0;i<SIZE-1;i++) {
            if (result[i] > result[i+1]){
                error = true;
        		printf("error in i=%d \n", i);
        	}
        }

        if(error)
            printf("Error..Not sorted correctly\n");
        else
            printf("Correct!\n");
		
		printf("Before Sorting:\n");
        for (int i = 0; i < SIZE; ++i) {
            printf("%d ", unsorted_array[i]);
        }
        printf("\n");

		printf("After Sorting:\n");
        for (int i = 0; i < SIZE; ++i) {
            printf("%d ", result[i]);
        }
        printf("\n");
    }

	printf("Cluster Size %d, execution time measured : %2.7f  \n",size, (end_time - start_time) * 1000);

	free(unsorted_array);
	MPI_Finalize();
}

