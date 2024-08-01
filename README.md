# Overview 
This repository includes the source code for sequential and MPI-based parallel implementations of Quick Sort algorithms using the C programming language. The primary objective of this project is to conduct a performance comparison between the two versions, demonstrating how parallel programming can enhance algorithm speed.

# Quick Sort Sequential Implementation 
- **Initialization & Pivot Selection:** Choose a pivot element from the array.
- **Partitioning:** Rearrange the array into two sub-arrays:
  - Elements less than the pivot.
  - Elements greater than or equal to the pivot.
- **Recursive Sorting:** Apply the partitioning process recursively to each sub-array.
- **Combining Results:** Combine the sorted sub-arrays to form the final sorted array.
- **Base Case:** Stop recursion when sub-arrays have one or zero elements, as they are already sorted.

  
# Quick Sort Implementation with MPI-Based Parallelization
- **Initialization:** The main process (rank 0) generates a randomly sized array.
- **Distribution:** The main process scatters segments of the array (sub-arrays) to other processes, ensuring each process has a portion of the dataset for independent sorting.
- **Parallel Sorting:** Each process independently applies the Quick Sort algorithm to its assigned sub-array.
- **Merging:** The main process (rank 0) collects the sorted sub-arrays from all processes and merges them into a single sorted array using MPI communication primitives.
