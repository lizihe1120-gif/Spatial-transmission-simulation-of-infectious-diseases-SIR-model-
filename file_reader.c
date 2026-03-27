#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>

/*This code is for reading and writing to files for 2025-2026 COMP528 OpenMP assignment*/

/*Use the functions in this file to read from the input file, and write to the output file*/

/*You should use this file when compiling your code*/

/*Declare these functions at the top of each 'main' file or use a header file*/

/*If there are any issues with this code, please contact: h.j.forbes@liverpool.ac.uk*/
int product(const int *dims, int num_dims){

    int product = 1; 
    int i;

    for(i = 0; i < num_dims; i++){
        product *= dims[i];
    }

    return product;
}

int read_num_dims(const char *filename){

    FILE *file = fopen(filename,"r");
    
    if(file == NULL){
        printf("Unable to open file: %s\n", filename);
        return -1; //-1 means error
    }

    char firstline[500];

    if(fgets(firstline, 500, file) == NULL){
        printf("Error reading file %s\n", filename);
        return -1; //-1 means error
    }

    int line_length = strlen(firstline);
    int num_dims = 0;

    int i;
    for(i = 0; i < line_length; i++){
        if(firstline[i] == ' '){
            num_dims++;
        }
    }

    fclose(file);
    return num_dims;
}


/*Gets the number of the radiator temperatures in the file. Returns as a single integer*/
int *read_dims(const char *filename, int num_dims) {

    FILE *file = fopen(filename,"r");
    
    if(file == NULL){
        printf("Unable to open file: %s\n", filename);
        return NULL;
    }

    char firstline[500];

    if(fgets(firstline, 500, file) == NULL){
        printf("Error reading file %s\n", filename);
        return NULL;
    }

    const char s[2] = " ";
    char *token;
    token = strtok(firstline, s);
    
    int *dims = malloc((num_dims) * sizeof(int));

    if(dims == NULL){
        printf("Error allocating memory for dimension array\n");
        return NULL;
    }

    int i = 0;
    while(token != NULL) {
        dims[i] = atoi(token);
        i++;
        token = strtok(NULL, s);
    }

    fclose(file);
    return dims;
}



/*Gets the data from the file. Returns as an array of doubles. Ignores the first numOfValues*/
float *read_array(const char *filename, const int *dims, int num_dims) {

    FILE *file = fopen(filename,"r");
    
    if(file == NULL) {
        printf("Unable to open file: %s\n", filename);
        fclose(file);
        return NULL;
    }

    char firstline[500];
    
    if(fgets(firstline, 500, file) == NULL){
        printf("Error reading file\n");
        fclose(file);
        return NULL;
    }

    int total_elements = product(dims, num_dims);
    float *data = malloc(total_elements * sizeof(float));

    if(data == NULL){
        printf("Error allocating memory for data from %s\n", filename);
        return NULL;
    }

    int i;
    float temp;
    for(i=0; i < total_elements; i++) {
        if(fscanf(file, "%f", &temp) != 1){
           printf("Error reading file");
            return NULL; //error  
        }
        data[i] = temp;
    }

    fclose(file);
    return data;
}

/*Writes to the output file*/
int write_to_output_file(const char *filename, float *output, int *dims, int num_dims){

    FILE *file = fopen(filename, "w");
    
    if(file == NULL){
        printf("Unable to open file %s\n", filename);
        return 1;
    }

    printf("\nFile opened, writing dims\n");

    int i;
    for(i = 0; i < num_dims; i++){
        fprintf(file, "%d ", dims[i]);
    }
    fprintf(file, "\n");

    int total_elements = product(dims, num_dims);

    printf("\nWriting output data\n");
    for(i=0; i < total_elements; i++) {
        fprintf(file, "%.2f ", output[i]);
    }

    fclose(file);
    return 0;
}






