#include<bits/stdc++.h>
#include "mpi.h"
#include<string.h>
#include<cstdlib>
#include <fstream>

using namespace std;

int main( int argc, char **argv ) {
    int rank, numprocs;
    long long int N;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );

    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */
    ifstream inFile;
    ofstream outFile;

    inFile.open(argv[1]);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    
    outFile.open(argv[2]);
    if (!outFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }

    long long int MAX_LIMIT = 1000000;
    long long int inp_arr[MAX_LIMIT];
    long long int i=0, x;
    while (inFile >> x) {
        inp_arr[i] = x;
        i++;
    }
    N = i;
    inFile.close();

    if(numprocs == 1){
        sort(inp_arr, inp_arr+N);
        for(long long int i=0; i<N; i++){
            outFile<<inp_arr[i];
            if(i != N-1)
                outFile<<" ";
        }
        outFile.close();
        MPI_Barrier( MPI_COMM_WORLD );
        double elapsedTime = MPI_Wtime() - tbeg;
        double maxTime;
        MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
        if ( rank == 0 ) {
            printf( "Total time (s): %f\n", maxTime );
        }

        MPI_Finalize();
        return 0;
    }

    long long int P = numprocs - 1;
    long long int partition_size = N / P;

    if(rank == 0){
        for(long long int i=0; i<P-1; i++)
        {
            MPI_Send(&inp_arr[i*partition_size], partition_size, MPI_LONG_LONG_INT, i+1, 0, MPI_COMM_WORLD);
        }
        MPI_Send(&inp_arr[(P-1)*partition_size], N-(P-1)*partition_size, MPI_LONG_LONG_INT, P, 0, MPI_COMM_WORLD);

        long long int gathered_samples[P][P];
        for(long long int i=0; i<P; i++){
            MPI_Recv(&gathered_samples[i], P, MPI_LONG_LONG_INT, i+1, 0, MPI_COMM_WORLD, &status);
        }

        long long int sorted_samples[P*P];
        long long int cnt[P] = {0};
        for(long long int i=0; i<P*P; i++){
            long long int val;
            long long int pos;
            for(long long int j=0; j<P; j++){
                if(cnt[j] < P){
                    val = gathered_samples[j][cnt[j]];
                    pos = j;
                    break;
                }
            }
            for (long long int j=0; j<P; j++){
                if(cnt[j]<P){
                    if(gathered_samples[j][cnt[j]] < val){
                        val = gathered_samples[j][cnt[j]];
                        pos = j;
                    }
                }
            }
            sorted_samples[i] = val;
            cnt[pos]++;
        }

        long long int pivot_interval = P;
        long long int pivots[P-1];
        long long int c = ceil(P/2);
        for(long long int i=0; i<P-1; i++){
            pivots[i] = sorted_samples[c + (i+1)*pivot_interval];
        }

        for(long long int i=0; i<P; i++){
            MPI_Send(pivots, P-1, MPI_LONG_LONG_INT, i+1, 0, MPI_COMM_WORLD);
        }

        long long int final_sorted_arr[N];
        long long int total_size;
        long long int idx = 0;
        for(long long int i=1; i<=P; i++){
            MPI_Recv(&total_size, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&final_sorted_arr[idx], total_size, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            idx = idx + total_size;
        }

        for(long long int i=0; i<N; i++){
            outFile<<final_sorted_arr[i];
            if(i != N-1)
                outFile<<" ";
        }
        outFile.close();
    }


    else{

        if(rank == P){
            partition_size = N - (P-1) * partition_size; 
        }

        long long int partition_arr[partition_size];
        
        MPI_Recv(partition_arr, partition_size, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);
        sort(partition_arr, partition_arr + partition_size);

        long long int local_samples[P];
        long long int sample_length = partition_size / P;
        
        for(long long int i=0; i<P; i++){
            local_samples[i] = partition_arr[i*sample_length];
        }
        MPI_Send(local_samples, P, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);

        long long int global_samples[P-1];
        MPI_Recv(global_samples, P-1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);

        long long int allotment[P][2] = {0};
        long long int n = 0, i = 0;
        while(n<P && i<partition_size){
            if(n==P-1 || partition_arr[i] <= global_samples[n]){
                allotment[n][1]++;
                i++;
            }
            else{
                n++;
                allotment[n][0] = i;
            }
        }

        long long int merged_partitions[P][partition_size];
        long long int len[P];

        len[rank-1] = allotment[rank-1][1];
        for(long long int i=0; i<len[rank-1]; i++){
            long long int temp = allotment[rank-1][0];
            merged_partitions[rank-1][i] = partition_arr[temp+i];
        }

        for(long long int i=1; i<rank; i++){
            long long int size = allotment[i-1][1];
            MPI_Send(&size, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            if(size > 0){
                MPI_Send(&partition_arr[allotment[i-1][0]], size, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            }
        }

        for(long long int i=rank+1; i<=P; i++){
            long long int size = allotment[i-1][1];
            MPI_Send(&size, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            if(size>0){
                MPI_Send(&partition_arr[allotment[i-1][0]], size, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            }
        }

        for(long long int i=1; i<rank; i++){
            MPI_Recv(&len[i-1], 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            if(len[i-1] > 0){
                MPI_Recv(&merged_partitions[i-1][0], len[i-1], MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            }
        }

        for(long long int i=rank+1; i<=P; i++){
            MPI_Recv(&len[i-1], 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            if(len[i-1] > 0){
                MPI_Recv(&merged_partitions[i-1][0], len[i-1], MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &status);
            }
        }

        long long int total_size = accumulate(len, len+P, 0);
        long long int sorted_samples[total_size];
        long long int cnt[P] = {0};
        for(long long int i=0; i<total_size; i++){
            long long int val;
            long long int pos;
            for(long long int j=0; j<P; j++){
                if(cnt[j] < len[j]){
                    val = merged_partitions[j][cnt[j]];
                    pos = j;
                    break;
                }
            }
            for(long long int j=0; j<P; j++){
                if(cnt[j] < len[j]){
                    if(merged_partitions[j][cnt[j]] < val){
                        val = merged_partitions[j][cnt[j]];
                        pos = j;
                    }
                }
            }
            sorted_samples[i] = val;
            cnt[pos]++;
        }
        MPI_Send(&total_size, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(sorted_samples, total_size, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();
    return 0;
}