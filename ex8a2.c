/*
======================================================================
File: ex8a2.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* This program Log in to the shared memory assigned by the ex8a1
* program.
* The program will run in a loop. each round in a loop The program
* produces a number if it is prime it adds it to the array in the
* shared memory.
* She counts how many new values she added
* Finally she prints how many new values she added to the array
*/

//-------include section---------------

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> 
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>


//--------const section----------------

const int ARR_SIZE = 104;
const int ARGC_SIZE = 2;

//------prototypes---------------------

void start_job(int shm_id, char* argv[],sem_t* mutex);
void fill_arr(int* ptr, int process_num, sem_t* mutex);
bool is_prime(int num);

//---------main section----------------

int main(int argc, char *argv[])
{
	if(argc != ARGC_SIZE)
	{
		perror("Incorrect number of arguments");
		exit(EXIT_FAILURE);
	}
	
	key_t key;
	int shm_id;
	sem_t* mutex;
	
	srand(atoi(argv[1]));
	
	mutex = sem_open(".", 0);
	if(mutex == SEM_FAILED)
	{
		perror("parent sem_open failed()");
		exit (EXIT_FAILURE);
	}
	
	if((key = ftok(".", '8')) == -1)
	{
		perror("ftok() failed");
		exit (EXIT_FAILURE);
	}
	
	if((shm_id = shmget(key, 0, 0600)) == -1)
	{
		perror("shmget() failed");
		exit (EXIT_FAILURE);	
	}
	
	start_job(shm_id, argv, mutex);
	
	sem_close(mutex);

	return EXIT_SUCCESS;
}


//-----------functions-------------------

//The function connects to the shared memory,
//starts working and finally sends a signal.

void start_job(int shm_id, char *argv[], sem_t* mutex)
{
	int *ptr;
	
	if((ptr = (int *) shmat (shm_id, NULL, 0)) < 0)
	{
		perror("shmat() failed");
		exit (EXIT_FAILURE);
	}
	
	    int process_num = atoi(argv[1]);

		ptr[process_num] = 1;
	
    while (ptr[1] < 1 || ptr[2] < 1 || ptr[3] < 1) {}

    fill_arr(ptr, process_num,mutex);
    
    kill(ptr[0], SIGINT);    //signal for ex8a1
}

//-------------------------------------------
//The function guerrilla numbers and if the number is prime it
//adds it to the array. Finally the function prints how many new values
//it has added to the array and which value it has added the most times.

void fill_arr(int *ptr,int process_num, sem_t* mutex)
{
	int  curr_index;

    for (int i = 5; i < ARR_SIZE; ++i) {
        int num = rand() % ((int)(pow(10, 6)) - 1) + 2;

		if(is_prime(num))
		{
				
			if(ptr[i] == 0)    //the cell is empty
			{
				curr_index = ptr[1] + ptr[2] + ptr[3] + 5 - 3;

            if (curr_index == 5) { // if it's the first time inserting
                ptr[5] = num;
                ptr[process_num]++; //add the counter for this process
            }
            else {
                if (ptr[curr_index - 1] <= num) {
                    ptr[curr_index++] = num;
                    ptr[process_num]++; //add the counter for this process
                }
            }
			sem_post(mutex);     // open
		}
		
	}
}
	printf("found: %d\n", ptr[process_num] - 1);
}

//-------------------------------------------
//The function receives a number and checks if it is a prime number.

bool is_prime(int num)
{
    int mid = num / 2;
    if (num < 2) {
        return false;
    }
    for (int i = 2; i <= mid; i++) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}
