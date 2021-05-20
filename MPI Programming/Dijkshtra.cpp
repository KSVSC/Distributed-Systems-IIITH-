#include<bits/stdc++.h>
#include "mpi.h"
#include<string.h>
#include<cstdlib>
#include <fstream>

using namespace std;

int main( int argc, char **argv ) {
    int rank, numprocs;
    long long int V, E, S;

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

    inFile>>V;
    inFile>>E;
    long long int max = 0;
    long long int adjacency_matrix[V][V] = {0};
    long long int i=0, x=1, a, b, w;

    for(long long int i=0; i<V; i++){
        for(long long int j=0; j<V; j++){
            if(i == j)
                adjacency_matrix[i][j] = 0;
            else
                adjacency_matrix[i][j] = 1000000;
        }
    }

    while (x)
    {
        if(i < 3*E){
            inFile>>a;
            inFile>>b;
            inFile>>w;
            adjacency_matrix[a-1][b-1] = w;
            adjacency_matrix[b-1][a-1] = w;
            max += w;
            i = i+3;
        }
        else{
            inFile>>S;
            S = S -1;
            x = 0;
        }
    }
    max = E*max*E*max;

    inFile.close();

    long long int P = numprocs - 1;
    long long int partition_size;
    if(P != 0){
        partition_size = V/P;
    }

    if(rank == 0){
        long long int visited[V] = {false};
        long long int distance_matrix[V];
        for(long long int i=0; i<V; i++){
            distance_matrix[i] = max;
        }

        if(P == 0){
            distance_matrix[S] = 0;

            for(long long int c=0; c<V-1; c++){
                long long int min = max, u;

                for(long long int l=0; l<V; l++){
                    if(visited[l] == false){
                        if(distance_matrix[l]<= min){
                            min = distance_matrix[l];
                            u = l;
                        } 
                    }
                }

                visited[u] = true;
                for(long long int v=0; v<V; v++){
                    if((!visited[v]) && (adjacency_matrix[u][v])){
                        if((distance_matrix[u] != max) && (distance_matrix[u] + adjacency_matrix[u][v] < distance_matrix[v])){
                            distance_matrix[v] = distance_matrix[u] + adjacency_matrix[u][v];
                        }
                    } 
                }
            }

            for(long long int i=0; i<V; i++){
                if(distance_matrix[i] >= max){
                    outFile<<i+1<<" "<<"-1";
                    if(i != V-1)
                        outFile<<endl;
                }
                else{
                    outFile<<i+1<<" "<<distance_matrix[i];
                    if(i != V-1)
                        outFile<<endl;
                }
            }
            outFile.close();

            MPI_Barrier( MPI_COMM_WORLD );
            double elapsedTime = MPI_Wtime() - tbeg;
            double maxTime;
            MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
            printf( "Total time (s): %f\n", maxTime );

            /* shut down MPI */
            MPI_Finalize();
            return 0;
        }

        for(long long int i=1; i<=P; i++)
        {
            long long int start = (i-1)*partition_size;
            long long int colmn_partition = partition_size;
            if(i != P){
                MPI_Send(&partition_size, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            }
            else{
                long long int size = (V-(P-1)*partition_size);
                MPI_Send(&size, 1, MPI_LONG_LONG_INT, P, 0, MPI_COMM_WORLD);
                colmn_partition = V-(P-1)*partition_size;
            }
            MPI_Send(&start, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);

            for(long long int j=0; j<colmn_partition; j++){
                MPI_Send(&adjacency_matrix[start+j][0], V, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
            }
        }

        for(long long int i=0; i<V; i++){
            long long int init_vertex = -99;
            long long int min_dist = -99;
            long long int sent_vertex;
            long long int sent_dist;
            for(long long int j=1; j<=P; j++){
                MPI_Recv(&sent_vertex, 1, MPI_LONG_LONG_INT, j, 0, MPI_COMM_WORLD, &status);
                MPI_Recv(&sent_dist, 1, MPI_LONG_LONG_INT, j, 0, MPI_COMM_WORLD, &status);
                
                if(sent_vertex != -99){
                    if((init_vertex == -99) || (min_dist > sent_dist)){
                        init_vertex = sent_vertex;
                        min_dist = sent_dist;
                    }
                }
            }
            visited[init_vertex] = true;
            distance_matrix[init_vertex] = min_dist;

            for(long long int l=1; l<=P; l++){
                MPI_Send(&init_vertex, 1, MPI_LONG_LONG_INT, l, 0, MPI_COMM_WORLD);
                MPI_Send(&min_dist, 1, MPI_LONG_LONG_INT, l, 0, MPI_COMM_WORLD);
            }
        }

        for(long long int i=0; i<V; i++){
            if(distance_matrix[i] >= max){
                outFile<<i+1<<" "<<"-1";
                if(i != V-1)
                    outFile<<endl;
            }
            else{
                outFile<<i+1<<" "<<distance_matrix[i];
                if(i != V-1)
                    outFile<<endl;
            }
        }
        outFile.close();
    }

    else{

        MPI_Recv(&partition_size, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);

        long long int start_vertex;
        long long int local_weight[partition_size][V];

        MPI_Recv(&start_vertex, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);

        for(long long int i=0; i<partition_size; i++){
            MPI_Recv(&local_weight[i], V, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        bool visited_matrix[partition_size] = {false};

        for(long long int i=0; i<V; i++){
            long long int init_vertex = -99;
            long long int min_dist = -99;
            for(long long int j=0; j<partition_size; j++){
                if(!visited_matrix[j]){
                    if((init_vertex == -99) || (min_dist > local_weight[j][S])){
                        init_vertex = j + start_vertex;
                        min_dist = local_weight[j][S];
                    }
                }
            }

            MPI_Send(&init_vertex, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&min_dist, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&init_vertex, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&min_dist, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD, &status);

            if(start_vertex <= init_vertex && init_vertex < start_vertex + partition_size){
                visited_matrix[init_vertex - start_vertex] = true;
            }
            for(long long int j=0; j<partition_size; j++){
                if(!visited_matrix[j]){
                    long long int updated_wt = min_dist + local_weight[j][init_vertex];
                    if(updated_wt < local_weight[j][S]){
                        local_weight[j][S] = updated_wt;
                    } 
                }
            }
        }
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