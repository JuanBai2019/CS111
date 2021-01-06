/*
NAME:Juan Bai
EMAIL:Daisybai66@yahoo.com
ID:105364754
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

int yield_flag=0;
int sync_flag=0;
long long sum = 0;
char* nameTest;
char* sync_arg = "none";
long lock = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void add(long long *pointer, long long value)
{
  long long sum = *pointer + value;
  if (yield_flag)
    sched_yield();
  *pointer = sum;
}
/*
void atomic_add(void * ptr, unsigned long value)
{

  long long prev, sum;
  do{
    prev =*((long long*)ptr);
    
    sum = prev + value;
  }while(__sync_val_compare_and_swap((long long*)ptr, prev, sum) == prev);
}
*/
void atomic_add(long long *ptr, long long value) {
  long long prev;
  do {
    prev =sum ;
  } while (__sync_val_compare_and_swap(ptr, prev, prev+value) != prev);
}
void * thread_worker(void *iteration)
{
  int iter = *((int*)iteration); //cast to int* than derefrence
  if(strcmp(sync_arg,"s")==0)
    while(__sync_lock_test_and_set(&lock,1));
  if(strcmp(sync_arg,"m")==0)
    pthread_mutex_lock(&mutex);
  // if(strcmp(sync_arg,"c")==0)
  if(strcmp(sync_arg, "c")==0)
    {
      for (int i = 0; i < iter; i++) 
	atomic_add(&sum, 1);
      for (int i = 0; i < iter; i++)  
	atomic_add(&sum, -1);
    }
  else
    {
      for (int i = 0; i < iter; i++)
        add(&sum, 1);
      for (int i = 0; i < iter; i++)
        add(&sum, -1);
    }
  
  if(strcmp(sync_arg,"s")==0)
    __sync_lock_release(&lock);
  if(strcmp(sync_arg, "m")==0)
    pthread_mutex_unlock(&mutex);

  return NULL;
}
static inline unsigned long get_nanosec_from_timespec(struct timespec * spec)
{
  unsigned long ret= spec->tv_sec;
  ret = ret * 1000000000 + spec->tv_nsec;
  return ret;
}

void get_testName()
{
  if(yield_flag==1)
    {
      if(strcmp(sync_arg, "m")==0)
	nameTest="add-yield-m";
      else if(strcmp(sync_arg,"s")==0)
	nameTest="add-yield-s";
      else if(strcmp(sync_arg,"c")==0)
	nameTest="add-yield-c";
      else
	nameTest="add-yield-none";
    }
  else//yield_flag==0
    {
      if(strcmp(sync_arg,"m")==0)
        nameTest="add-m";
      else if(strcmp(sync_arg,"s")==0)
        nameTest="add-s";
      else if(strcmp(sync_arg,"c")==0)
        nameTest="add-c";
      else
        nameTest="add-none";
    }
}



int main(int argc,char* argv[]) 
{
  struct option args[]=
    {
      {"threads",1,NULL,'t'},
      {"iterations",1,NULL,'i'},
      {"yield",0,NULL,'y'},
      {"sync",1,NULL,'s'},
      {0,0,0,0},
    };
  int opt;
  int numThreads = 1;
  int numIterations = 1;
  while((opt=getopt_long(argc,argv,"",args,NULL))!=-1)
  {
      switch(opt)
      {
      case 't':
	numThreads = atoi(optarg);
	
	break;
      case 'i':
	numIterations = atoi(optarg);
	break;
      case 'y':
	yield_flag=1;
	break;
      case 's':
	sync_flag = 1;
	sync_arg=optarg;
	break;
	
	
      default:
	fprintf(stderr,"Correct usege: ./lab2_add --threads=numThread --iterations=numIteration --yield --sync='m' --yield='s' --yield='c'\n");
	exit(1);
      }
  }

 

  struct timespec begin, end;
  unsigned long totalTime = 0;
  

  if(clock_gettime(CLOCK_MONOTONIC, &begin) <0)
    fprintf(stderr,"Error from fetching the beginning time: %s\n", strerror(errno));
  pthread_t *thread_ids = (pthread_t*)malloc(numThreads *sizeof(pthread_t));
  //  if(sync_arg=='m')
  for (int i = 0; i < numThreads; i++)
    {
      if( pthread_create(&thread_ids[i],NULL,thread_worker,&numIterations)<0)
	fprintf(stderr,"Error from pthread_create(): %s\n", strerror(errno));
    }
  for (int i = 0; i < numThreads; i++)
    {
      if( pthread_join(thread_ids[i],NULL) < 0)
	fprintf(stderr,"Error from pthread_jion(): %s\n", strerror(errno));
    }
  clock_gettime(CLOCK_MONOTONIC, &end);
  totalTime =  get_nanosec_from_timespec(&end) - get_nanosec_from_timespec(&begin);  //diff stores the execution time in ns
  int numOps = numThreads*numIterations*2;

  get_testName();
    fprintf(stdout,"%s,%d,%d,%d,%lu,%lu,%lld\n",nameTest,numThreads,numIterations,numOps,totalTime,totalTime/numOps,sum);
    if(strcmp(sync_arg,"m")==0)
      pthread_mutex_destroy(&mutex);
    free(thread_ids);
    exit(0);

}






