// Mert Kilicaslan - Programming Assingment 3
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

pthread_barrier_t barrier4, barrierTotal;
pthread_mutex_t lockCounterA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockCounterB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockCounterBarrier = PTHREAD_MUTEX_INITIALIZER;

int counterA = 0, counterB = 0, countBarrier = 0;


void* rideShare(void* args){
    char teamLetter =  *(char *) args;

    pthread_barrier_wait(&barrierTotal);
    printf("The Thread ID: %ld, Team: %c, I am looking for a car\n", pthread_self(), teamLetter);

    while(1){
        
        // Either all 4 in the same team or 2 - 2 equality must be in the band
        // Valid threads try to enter the band until counters reset 
        if((counterA + counterB < 4) && ((counterA <= 1 && counterB <= 2 && teamLetter == 'A') || (counterA <= 2 && counterB <= 1 && teamLetter == 'B') ||
         (counterA == 2 && teamLetter == 'A' && counterB == 0) || (counterA == 0 && teamLetter == 'B' && counterB == 2) || 
         (counterA == 3 && teamLetter == 'A' && counterB == 0) || (counterA == 0 && teamLetter == 'B' && counterB == 3))) {
            
            // Thread from team A entered the band
            if(teamLetter == 'A'){ 
                pthread_mutex_lock(&lockCounterA); 
                counterA++; 
                pthread_mutex_unlock(&lockCounterA);
                }

            // Thread from team B entered the band
            else if(teamLetter == 'B'){ 
                pthread_mutex_lock(&lockCounterB); 
                counterB++; 
                pthread_mutex_unlock(&lockCounterB);
                }

            // Threads wait until a valid combination is formed, last thread wakes them up and make them cross the barrier
            pthread_barrier_wait(&barrier4);
            printf("The Thread ID: %ld, Team: %c, I have found a spot in a car\n", pthread_self(), teamLetter);
                     
            // If thread is the last one forming the ride share, it declare itself as the captain. Set counters as zero to triggers other treads to create new band
            pthread_mutex_lock(&lockCounterBarrier); 
            countBarrier++;
            if(countBarrier == 4){
                printf("The Thread ID: %ld, Team: %c, I am the captain and driving the car\n", pthread_self(), teamLetter);
                counterA = 0;
                counterB = 0;
                countBarrier = 0;
            }
            pthread_mutex_unlock(&lockCounterBarrier);
            
            // After all threads finish their job, they cross the barrier and break the loop
            pthread_barrier_wait(&barrierTotal);
            break;
        }    
    }
}


int main(int argc, char* argv[]){

    const int numA = strtol(argv[1], NULL, 10);
    const int numB = strtol(argv[2], NULL, 10);

    // Each group size must be an even number && total number of supporters must be a multiple of four
    if(numA % 2 != 0 || numB % 2 != 0 || (numA + numB) % 4 != 0){
        printf("The main terminates\n");
        exit(1);
    }

    char letterA = 'A';
    char letterB = 'B';

    pthread_barrier_init(&barrier4, NULL, 4); // Barrier holding 4 threads
    pthread_barrier_init(&barrierTotal, NULL, numA + numB); // Barirer holding all threads
    pthread_t tA[numA], tB[numB];

    // Creating team A
    for (int i = 0; i < numA; i++)
        if (pthread_create(&tA[i], NULL, &rideShare, &letterA) != 0){
            perror("Failed to create a thread from team A\n");
            return -1;
        }  
    // Creating team B
    for( int i = 0; i < numB; i++)    
        if (pthread_create(&tB[i], NULL, &rideShare, &letterB) != 0){
            perror("Failed to create a thread from team B\n");
            return -1;
        }
        

    // Main is waiting team A
    for (int i = 0; i < numA; i++)
        if (pthread_join(tA[i], NULL) != 0 ){
            perror("Failed to join a thread from team A\n");
            return -1;
        }
    // Main is waiting team B
    for (int i = 0; i < numB; i++)
        if (pthread_join(tB[i], NULL) != 0 ){
            perror("Failed to join a hread from team B\n");
            return -1;
        }

    pthread_barrier_destroy(&barrier4);
    pthread_barrier_destroy(&barrierTotal);

    printf("The main terminates\n");

    return 0;
}