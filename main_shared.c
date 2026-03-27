#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<stdint.h>
#include<stdbool.h>
#include"file_reader.c"
#include<omp.h>
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
                                //if((double)(rand_r(&seed+column) % 1001) / 1000.0<rate[i*dims[1]+j]/distance){
                                if((double)(normal_rand() / 4294967296.0)<rate[i*dims[1]+j]/distance){
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
    if (i != 5){
        printf("Data: %s <R> <REC_TIME> <MAX_RUNS> <infile>\n", basical_data[0]);
        return 1;
    }
    int R = atoi(basical_data[1]);
    int REC_TIME = atoi(basical_data[2]);
    int MAX_RUNS = atoi(basical_data[3]);
    char *infile = basical_data[4];
    float RECOVER_TIME = ceil((float)1.0f/REC_TIME*100)/100;
    srand(time(NULL));
    int num_dims = read_num_dims(infile);
    int *dims = read_dims(infile,num_dims);
    float *rate = read_array(infile, dims, num_dims);
    float AVERAGE_CELLS_INFECTED = 0;
    int INFECTED_CELLS = 0;
    normal_seed = (uint64_t)time(NULL) ^ ((uint64_t)clock() << 32);
    time_t start_time, end_time;
    //double start_time = omp_get_wtime();
    time(&start_time);
    //RUN_MAX_RUNS
    #pragma omp parallel for reduction(+:INFECTED_CELLS)
    for(int a =0; a<MAX_RUNS; a++){
        float state[dims[0]][dims[1]];
        memset(state, 0, sizeof(float) * dims[0] * dims[1]);
        // unsigned int seed_x = (unsigned int)(a*79+12345*omp_get_thread_num());
        // unsigned int seed_y = (unsigned int)(a*79+54321*omp_get_thread_num());
        //unsigned int seed = i+(unsigned long)(omp_get_wtime()*31);
        //ZERO
        //state[rand_r(&seed) % dims[0]][rand_r(&seed) % dims[1]] = 1.00;
        state[normal_rand() % dims[0]][normal_rand() % dims[1]] = 1.00;
        INFECTED_CELLS++;
        RUN(dims, state, rate, RECOVER_TIME, R,&INFECTED_CELLS);
    }
    //double end_time = omp_get_wtime();
    time(&end_time);
    AVERAGE_CELLS_INFECTED = (float)INFECTED_CELLS/MAX_RUNS;
    double total_time = end_time - start_time;
    printf("AVERAGE CELLS INFECTED: %f\n", AVERAGE_CELLS_INFECTED);
    printf("TOTAL TIME TAKEN: %f second\n", total_time);
    return 0;

}
