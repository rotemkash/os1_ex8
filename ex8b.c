/*
======================================================================
File: ex8b.c
======================================================================
Writen by : Rotem Kashani, ID = 209073352
Mahmood Jazmawy, ID = 211345277

 *The program defines a global array of 1000 cells. 
 * The main thread: the array owner
 * It creates 3 threads and then it waits for them and
 * when they finish it means that the array is full and 
 * then it shows how many values ​​have been added to the array and which are
 * the maximum and the minimum.
 * Three Sub threads: Prime Number Producers
 * Each thread draws a random number and tries to add it to the array.
 * For the test + addition: the thread lock the semaphore and thereby 
 * gain (this time: for real) the right to scan the array alone. 
 * Finally he releases the semaphore.
 * If a thread found all the cells of the array full 
 * (or it failed a hundred times in a row to add a new value) it print how many 
 * new primes he added to the array and finish.
 * Only one thread print after filling the array the output done, 
 * this will happen with the pthread_once mechanism.
*/

//-------include section---------------

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/wait.h>

//--------define section----------------

#define ARR_SIZE 1000
#define NUM_OF_THREADS 3
#define SEED 17
#define RUN 1
#define RAND_LIMIT 999999

//--------global section----------------

int prime_arr[ARR_SIZE] = { 0 };
int threads_num[NUM_OF_THREADS]={0};
int max_prime = 0;
int index_run = 0;
bool end = false;
sem_t *mutex;
pthread_once_t first_end_thread=PTHREAD_ONCE_INIT;
pthread_t thread_data[NUM_OF_THREADS];

//------prototypes---------------------

void* rand_num(void* arg);
bool is_prime(int num);
void end_thread(void);
void create_mutex();
void create_threads();
void join_to_threads();

//---------main section----------------

int main()
{
	srand(SEED);
	create_mutex();
	create_threads();
	join_to_threads();
	
	printf("size is: %d, min is: %d, max is:%d\n", index_run, 
			prime_arr[0], max_prime);
	
	sem_close(mutex);   //close the semaphore
	
	sem_unlink("mutex");  //remove the semaphore
	
	return EXIT_SUCCESS;
}

//-----------functions-------------------
//The function create the semaphore

void create_mutex()
{
	mutex = sem_open(".", O_CREAT, 0600, 1);
	if(mutex == SEM_FAILED)
	{
		perror("parent sem_open failed()");
		exit (EXIT_FAILURE);
	}
}

//The function create the threads

void create_threads()
{
	int status;
	for (int i = 0; i < NUM_OF_THREADS; i++)
	{
		status = pthread_create(&thread_data[i], NULL, rand_num,&i);
		if (status != 0)
		{
			fputs("pthread create failed from main procces", stderr);
			exit(EXIT_FAILURE);
		}
	}
}

//The function wait the threads to finish

void join_to_threads()
{
	for (int i = 0; i < NUM_OF_THREADS; i++)
	{
		pthread_join(thread_data[i], NULL);
	}
}

// The function creates and tries to insert prime numbers

void* rand_num(void* arg)
{
	int new_primes=0;
	int failed_counter=0;
	int num;
	int counter = 0;
	int id = *((int *)arg);
	threads_num[id]=1;
		
	while (counter == NUM_OF_THREADS)
	{
        counter = 0;
        for (int i = 0; i < NUM_OF_THREADS; i++)
            if (threads_num[i] == 0)
                counter++;
    } 
	counter=0;
	
	while(RUN)
	{
		num=(rand()%RAND_LIMIT)+2;
		if (index_run < ARR_SIZE - 1 && failed_counter < 100)
		{
			if(is_prime(num))
			{
				sem_wait(mutex);
				if (failed_counter < 100)
				{
					if (num >= max_prime)
					{
						max_prime = num;
						new_primes++;
						prime_arr[index_run] = num;
						index_run++;
					}

					else
						failed_counter++;
						
					sem_post(mutex);
				}
				
				
			}
		}
		else
				end=true;
				
		if(end)
		{
			printf("found %d primes\n", new_primes);
			pthread_once(&first_end_thread,end_thread);
			pthread_exit(NULL);
		}		
	}
	pthread_exit(NULL);
}

//The function checks if a number is prime

bool is_prime(int num) {
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

//Only one thread print the output "done"

void end_thread(void)
{
	printf("done\n");
}
