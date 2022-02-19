// Mert Kilicaslan - Programming Assingment 2
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

bool checkHorizontal(char** arr, int size){
    for(int i = 0; i < size; i++){

        char letter = arr[i][0];
        if(letter == ' ')
            continue;

        int counter = 0;
        for(int j = 0; j < size; j++){
            if(letter == arr[i][j])
                counter++;         
        }

        if(counter == size)
            return true;
    }
    return false;
}

bool checkVertical(char** arr, int size){
    for(int j = 0; j < size; j++){
        
        char letter = arr[0][j];
        if(letter == ' ')
            continue;

        int counter = 0;
        for(int i = 0; i < size; i++){
            if(letter == arr[i][j])
                counter++;
        }

        if(counter == size)
            return true;
    }
    return false;
}

bool checkPrimaryDiagonal(char** arr, int size){
    char letter = arr[0][0];
    int counter = 0;

    if(letter == ' ')
        return false;

    for(int i = 0; i < size; i++)
        if(arr[i][i] == letter)
            counter++;
    
    if(counter == size)
        return true;

    return false;
}

bool checkSecondaryDiagonal(char** arr, int size){
    int counter = 0;
    char letter = arr[0][size-1];

    if(letter == ' ')
        return false;

    for(int i = 0; i < size; i++)
        if(arr[i][size-i-1] == letter)
            counter++;
    
    if(counter == size)
        return true;
      
    return false;
}

struct tstruct{
    char** arr;
    char letter;
    int size;
};

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int done = 0;
bool gameEnd = false;

void* routine(void* arg){
    struct tstruct* ts = (struct tstruct*) arg;
    
    srand(time(NULL));
    int ri, rj, counter = 0;

    if(ts->letter == 'o')
         usleep(10);

    while(counter < ts->size*ts->size){

        pthread_mutex_lock(&lock);

        if(done % 2 == 0){
            done++;
            counter++;

            pthread_cond_wait(&cond, &lock);

            if(gameEnd)
                break;

            do{
                ri = rand() % ts->size;
                rj = rand() % ts->size;
            } while(ts->arr[ri][rj] != ' ');
            
            ts->arr[ri][rj] = ts->letter;
            printf("Player %c played on: (%d,%d)\n", ts->letter, ri, rj);

            if(checkHorizontal(ts->arr, ts->size) || checkVertical(ts->arr, ts->size) || checkPrimaryDiagonal(ts->arr, ts->size) || checkSecondaryDiagonal(ts->arr, ts->size)){

                printf("Game end\n");
                printf("Winner is %c\n", ts->letter);
                gameEnd = true;
                
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&lock);  
                break;
            }
        }

        else{
            done++;
            counter++;
            pthread_cond_signal(&cond); 
        } 
        pthread_mutex_unlock(&lock);  
    } 
}

int main(int argc, char* argv[]){

    int size = strtol(argv[1], NULL, 10);
    printf("Board Size: %dx%d\n", size, size);
    
    // Dynamically allocating 2D array
    char** arr = malloc(sizeof(char*) * size);
    for (int i = 0; i < size; i++)
        arr[i] = malloc(sizeof(char) * size);
    
    // Making elements of 2d array an empty char
    for (int i = 0; i < size; i++)
        for(int j = 0; j < size; j++)
            arr[i][j] = ' ';

    pthread_t t1, t2;

    // Struct variables to pass into threads
    struct tstruct s1 = {arr, 'x', size};
    struct tstruct s2 = {arr, 'o', size};

    // Creating threads with different structs passed
    if (pthread_create(&t1, NULL, routine, &s1) != 0)
        perror("Failed to create thread\n");
    if (pthread_create(&t2, NULL, routine, &s2) != 0)
        perror("Failed to create thread\n");

    // Waiting children treads to finish
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    if(!gameEnd){
        printf("Game end\n");
        printf("It is a tie\n");
    }

    // Printing 2D array
    for (int i = 0; i < size; i++){
        for(int j = 0; j < size; j++)
            printf("[%c] ",arr[i][j]);
        printf("\n");
    }

    // Deallocating 2d array
    for (int i = 0; i < size; i++){
        free(arr[i]);
        arr[i] = NULL;
    }
    free(arr);
    arr = NULL;

    return 0;
}