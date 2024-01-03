/*
======================================================================
File: ex8a1.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

* This program defines in a shared memory an array of integers
* To the first cell he enters his id. 
* Then he goes to sleep until he gets a signal and then he shows 
* how many different values there are in the array, what is the
* smallest value and what is the largest value.

*/

//-------include section---------------

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h> 
#include <semaphore.h>

//--------const section----------------

const int ARR_SIZE = 104;

//------prototypes---------------------

void catch_signal(int sig_num);
void start_job(int shm_id);
void init_primes_arr(int *primes_arr);
void delete_shm(int shm_id);

//---------main section----------------
int main()
{
	signal(SIGINT, catch_signal);
	
	key_t key;
	int shm_id;
	sem_t* mutex;
	
	if((key = ftok(".", '8')) == -1)
	{
		perror("ftok() failed");
		exit (EXIT_FAILURE);
	}
	
	if((shm_id = shmget(key, ARR_SIZE, IPC_CREAT
		| IPC_EXCL | 0600)) == -1)
	{
		perror("shmget() failed");
		exit (EXIT_FAILURE);	
	}
	
	mutex = sem_open(".", O_CREAT, 0600, 1);
	if(mutex == SEM_FAILED)
	{
		perror("parent sem_open failed()");
		exit (EXIT_FAILURE);
	}
	
	start_job(shm_id);
		
	delete_shm(shm_id);
	
	sem_close(mutex);   //close the semaphore
	
	sem_unlink("mutex");  //remove the semaphore


	return EXIT_SUCCESS;
}


//-----------functions-------------------

//signal handler
void catch_signal(int sig_num) 
{

}

//----------------------------------------
//The function reads the array from the shared memory and finally
//prints how many different values there are in the array

void start_job(int shm_id)
{
	int *ptr;
	
	if((ptr = (int *) shmat (shm_id, NULL, 0)) < 0)
	{
		perror("shmat() failed");
		exit (EXIT_FAILURE);
	}
	
	init_primes_arr(ptr);
	
	pause();
	
	
	    int size = ptr[1] + ptr[2] + ptr[3] - 3;
    printf("size %d, min: %d, max: %d\n", size, ptr[5], ptr[size + 4]);
	
}

//-------------------------------------
//The function initializes the array, the first cell enters the pid,
//and the other cells in the array enter 0.

void init_primes_arr(int *primes_arr)
{	
	for(int i = 0; i < ARR_SIZE; i++)
		primes_arr[i] = 0;          //reset_arr
		
	primes_arr[0] = getpid();
}

//---------------------------------------
//The function deletes the shm_id

void delete_shm(int shm_id) {
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl() failed");
        exit(EXIT_FAILURE);
    }
}
