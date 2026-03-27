#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>
#include<mpi.h>
#include"file_reader.c"
static uint64_t normal_seed;
uint32_t normal_rand() {
    normal_seed = normal_seed * 6364136223846793005ULL + 1;
    return (uint32_t)(normal_seed >> 32);
}


//TIME_UPDATE 
void TIME_UPDATE(int *dims, float state[dims[0]][dims[1]], float RECOVER_TIME, int *INFECTED_CELLS, int record[14000][2], int *record_number){
    //#pragma omp parallel for schedule(guided) num_threads(64)
    
    for(int i =0; i < dims[0]; i++){
        for(int j =0; j < dims[1]; j++){
            if(RECOVER_TIME<state[i][j]&&state[i][j]<2){
                state[i][j] = state[i][j]+RECOVER_TIME;
                if(state[i][j]<2){
                    record[*record_number][0]=i;
                    record[*record_number][1]=j;
                    (*record_number)++;
                }
            }else if(state[i][j]==RECOVER_TIME){
                state[i][j] = 1;
                record[*record_number][0]=i;
                record[*record_number][1]=j;
                (*record_number)++;
                (*INFECTED_CELLS)++;
            } 
        }
    }
}
void TIME_UPDATE_FAST(int *dims, float state[dims[0]][dims[1]], float RECOVER_TIME, int *INFECTED_CELLS, int record[14000][2], int *record_number){
    //#pragma omp parallel for schedule(guided) num_threads(64)
    for(int i =0; i < dims[0]; i++){
        for(int j =0; j < dims[1]; j++){
            if(RECOVER_TIME<state[i][j]&&state[i][j]<2){
                state[i][j] = state[i][j]+RECOVER_TIME;
                if(state[i][j]<2){
                    record[*record_number][0]=i;
                    record[*record_number][1]=j;
                    (*record_number)++;
                }
            }else if(state[i][j]==RECOVER_TIME){
                state[i][j] = 1;
                record[*record_number][0]=i;
                record[*record_number][1]=j;
                (*record_number)++;
                (*INFECTED_CELLS)++;
            } 
        }
    }
}
float GET_RATE(int i, int j, int dims, float *rate){
    float result =rate[i*dims+j];
    return result;
}
//INFECT_UPDATE
bool INFECT_UPDATE_FAST(int *dims, float state[dims[0]][dims[1]], float *rate, float RECOVER_TIME, int R, int record[14000][2], int record_number){
    bool INFECT_ABLE = false;
    int INFECTED_PERSON = 0;
    if(record_number!=0){INFECT_ABLE = true;}
    //#pragma omp parallel for schedule(guided) num_threads(64)
    for(int number=0;number<record_number; number++){
        int i = record[number][0];
        int j = record[number][1];
                //DOWN
                for(int row = i; row<dims[0]&&row-i<=R; row++){
                    //RIGHT
                    for(int column = j, distance = 0;column<dims[1]; column++){
                        int dr = row -i;
                        if(dr<0) dr = -dr;
                        int dc = column -j;
                        if(dc<0) dc = -dc;
                        distance =dr+dc;
                        // distance = abs(row-i)+abs(column-j);
                        if(distance>=0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                 state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                    //LEFT
                    for(int column = j-1, distance = 0;column>=0; column--){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                }
                //UP
                for(int row =i-1; row>=0&&i-row<=R; row--){
                    //RIGHT
                    for(int column = j, distance = 0;column<dims[1]; column++){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                 state[row][column] = RECOVER_TIME;
                                }
                            }                        
                        }else{
                            break;
                        }

                    }
                    //LEFT
                    for(int column = j-1, distance = 0;column>=0; column--){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                }
    }
    return INFECT_ABLE;
}
bool INFECT_UPDATE(int *dims, float state[dims[0]][dims[1]], float *rate, float RECOVER_TIME, int R){
    bool INFECT_ABLE = false;
    int INFECTED_PERSON = 0;
    //#pragma omp parallel for schedule(guided) num_threads(64)
    for(int i = 0; i < dims[0]; i++){
        for(int j = 0; j < dims[1]; j++){
            //unsigned int seed = i * j+(unsigned long)(omp_get_wtime() * 2131);
            if(RECOVER_TIME<state[i][j]&&state[i][j]<2){
                INFECT_ABLE = true;
                //DOWN
                for(int row = i; row<dims[0]&&row-i<=R; row++){
                    //RIGHT
                    for(int column = j, distance = 0;column<dims[1]; column++){
                        int dr = row -i;
                        if(dr<0) dr = -dr;
                        int dc = column -j;
                        if(dc<0) dc = -dc;
                        distance =dr+dc;
                        // distance = abs(row-i)+abs(column-j);
                        if(distance>=0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                 state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                    //LEFT
                    for(int column = j-1, distance = 0;column>=0; column--){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                }
                //UP
                for(int row =i-1; row>=0&&i-row<=R; row--){
                    //RIGHT
                    for(int column = j, distance = 0;column<dims[1]; column++){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                 state[row][column] = RECOVER_TIME;
                                }
                            }                        
                        }else{
                            break;
                        }

                    }
                    //LEFT
                    for(int column = j-1, distance = 0;column>=0; column--){
                        distance = abs(row-i)+abs(column-j);
                        if(distance>0&&distance<=R){
                            if(state[row][column]==0){
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<(rate[row * dims[1] + column]/distance)){
                                if((double)(normal_rand() / 4294967296.0)<(rate[row * dims[1] + column]/distance)){
                                    //#pragma omp atomic write
                                state[row][column] = RECOVER_TIME;
                                }
                            }
                        }else{
                            break;
                        }

                    }
                }

            }
        }
    }
    return INFECT_ABLE;
}
//ONE_TIME
void RUN(int *dims, float state[dims[0]][dims[1]], float *rate, float RECOVER_TIME, int R, int *INFECTED_CELLS){
    //#pragma omp parallel for reduction(+:INFECTED_CELLS) schedule(dynamic, 20) num_threads(8)
    int record[14000][2] ={0};
    int record_number =0; 
    bool i = INFECT_UPDATE(dims, state, rate, RECOVER_TIME, R);
    TIME_UPDATE(dims, state, RECOVER_TIME,INFECTED_CELLS, record, &record_number);
    for(; i == true;){
        i = INFECT_UPDATE_FAST(dims, state, rate, RECOVER_TIME, R, record, record_number);
        memset(record, 0, sizeof(int)*14000*2);
        record_number =0; 
        TIME_UPDATE(dims, state, RECOVER_TIME,INFECTED_CELLS, record, &record_number);
        if (i==false)
        {
            break;
        }
        
    }
}
int main (int i, char *basical_data[]){
    MPI_Init(&i,&basical_data);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if (i != 5){
        if (rank == 0) {
            printf("Data: %s <R> <REC_TIME> <MAX_RUNS> <infile>\n", basical_data[0]);
        }
        MPI_Finalize();
        return 1;
    }
    int R = atoi(basical_data[1]);
    int REC_TIME = atoi(basical_data[2]);
    int MAX_RUNS = atoi(basical_data[3]);
    char *infile = basical_data[4];
    float RECOVER_TIME = ceil((float)1.0f/REC_TIME*100)/100;
    int num_dims;
    int *dims = NULL;
    float *rate = NULL;
    if (rank ==0){
        num_dims = read_num_dims(infile);
        dims = read_dims(infile,num_dims);
        rate = read_array(infile, dims, num_dims);
    }
    MPI_Bcast(&num_dims,1,MPI_INT,0,MPI_COMM_WORLD);
    if(rank != 0){
        dims =(int *)malloc(num_dims * sizeof(int));
    }
    MPI_Bcast(dims,num_dims,MPI_INT,0,MPI_COMM_WORLD);
    int rate_size =dims[0]*dims[1];
    if(rank != 0){
        rate =(float *)malloc(rate_size * sizeof(float));
    }
    MPI_Bcast(rate,rate_size,MPI_FLOAT,0,MPI_COMM_WORLD);

    int base_runs = MAX_RUNS / size;
    int more_runs = MAX_RUNS % size;
    int local_runs = base_runs + (rank < more_runs ? 1 : 0);
    int start_index = rank * base_runs + (rank < more_runs ? rank : more_runs);

    float AVERAGE_CELLS_INFECTED = 0.0f;
    int INFECTED_CELLS = 0;
    int ONE_INFECTED_CELLS =0;
    int MAX_INFECTED = 0;
    int LAST_INFECTED_CELLS = 0;
    int NOW_INFECTED_CELLS = 0;
    float STD_DEV =0;
    normal_seed = (uint64_t)time(NULL) ^ ((uint64_t)clock() << 32);

    time_t start_time, end_time;
    int *local_caculate = (int *)malloc((local_runs>0?local_runs:1) * sizeof(int));
    for (int t = 0; t < local_runs; ++t) local_caculate[t] = 0;
    int local_infected = 1;
    int local_max_infected = 0;
    time(&start_time);

    for(int a =0; a<local_runs; a++){
        float state[dims[0]][dims[1]];
        memset(state, 0, sizeof(float) * dims[0] * dims[1]);
        state[normal_rand() % dims[0]][normal_rand() % dims[1]] = 1.00;
        INFECTED_CELLS++;
        local_infected++;
        LAST_INFECTED_CELLS = INFECTED_CELLS;
        RUN(dims, state, rate, RECOVER_TIME, R,&INFECTED_CELLS);
        NOW_INFECTED_CELLS = INFECTED_CELLS - LAST_INFECTED_CELLS;
        local_caculate[a] = NOW_INFECTED_CELLS;
        local_infected += NOW_INFECTED_CELLS;
        if(local_max_infected < NOW_INFECTED_CELLS){
            local_max_infected = NOW_INFECTED_CELLS;
        }
    }

    int *recv_counts = NULL;
    if (rank == 0) {
        recv_counts = (int *)malloc(size * sizeof(int));
    }
    MPI_Gather(&local_runs, 1, MPI_INT, recv_counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *displs = NULL;
    int total_recv = 0;
    int *global_caculate = NULL;
    if (rank == 0) {
        displs = (int *)malloc(size * sizeof(int));
        displs[0] = 0;
        total_recv = recv_counts[0];
        for (int r = 1; r < size; r++) {
            displs[r] = displs[r-1] + recv_counts[r-1];
            total_recv += recv_counts[r];
        }
        global_caculate = (int *)malloc((total_recv>0?total_recv:1) * sizeof(int));
    }

    MPI_Gatherv(local_caculate, local_runs, MPI_INT, global_caculate, recv_counts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    int total_infected = 0;
    MPI_Reduce(&local_infected, &total_infected, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    int global_max_infected = 0;
    MPI_Reduce(&local_max_infected, &global_max_infected, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        INFECTED_CELLS = total_infected;
        MAX_INFECTED = global_max_infected;
        time(&end_time);
        AVERAGE_CELLS_INFECTED = (float)INFECTED_CELLS / (float)MAX_RUNS;

        double sumsq = 0.0;
        for (int idx = 0; idx < MAX_RUNS; idx++) {
            int diff = global_caculate[idx] - (double)AVERAGE_CELLS_INFECTED;
            sumsq += diff * diff;
        }
        STD_DEV = sumsq/MAX_RUNS;
        STD_DEV = sqrt(STD_DEV);
        double total_time = difftime(end_time, start_time);
        printf("AVERAGE CELLS INFECTED: %f\n", AVERAGE_CELLS_INFECTED);
        printf("TOTAL TIME TAKEN: %f second\n", total_time);
        printf("MAX INFECTED: %d\n", MAX_INFECTED);
        printf("STANDARD DEVIATION: %f\n", STD_DEV);
    }
    MPI_Finalize();
    return 0;
}
