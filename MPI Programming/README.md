## Parallel Quick Sort Using MPI:

### Algorithm Used : Parallel Sorting by Regular Sampling
- Divide the array into p lists and sort them in parallel with different processes.
- Select (p-1) pivots from each of p lists and send them to the main process.
- Sort the pivots and partition the lists according to the pivots.
- Exchange the partitions among the processes by sending the partions whose elements are less than are equal to 1st pivot to first process.
- Similarly with the other partitions too.
- Finally merge all the partitions into single list.
#### Complexity: This algorithm takes O(p) space and O(nlogn) time.


## Single Source Shortest Path Using MPI:

### Algorithm Used : Dijkshtras Algorithm

- Divide the vertices among the existing processes.
- The adjacency matrix is also partitioned accordingly.
- Then each process checks for the local minimum distance from global source vertex to their vertices.
- Every process reports their minimum distances to the main process.
- One of the processes has found the global minimum distance from source vertex.
- This procedure is repeated for (n-1) times.
- After completion the minimum distance from source vertex to every vertex is found. 
#### Complexity: This algorithm takes O(n^2) space and O(n^2) time.