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
#include "SortedList.h"

int threads = 1;
int iterations = 1;
char *yield_arg;
int sync_flag = 0;
int yield_flag = 0;
char* sync_arg="none";
char*nameTest;
SortedListElement_t * pool;
int opt_yield = 0;
SortedListElement_t  *listheads;
pthread_mutex_t *mutex_lock; //one lock for each sublist
long *spin_lock;
unsigned long* wait_time=NULL;
int num_sub_lists =1; 

void segfault_handler()
{
  fprintf(stderr, "Encounter segmentation fault\n ");
  exit(2);
}

int hashKey(const char* key)
{
  return *key % num_sub_lists;
}

static inline unsigned long get_nanosec_from_timespec(struct timespec * spec)
{
  unsigned long ret= spec->tv_sec;
  ret = ret * 1000000000 + spec->tv_nsec;
  return ret;
}
void getName() // ldi 3 bits
{
  if(strcmp(sync_arg,"s")==0)
  {
    if(opt_yield==0)
      nameTest = "list-none-s";
    else if(opt_yield==1)
      nameTest = "list-i-s";
    else if(opt_yield==2)
      nameTest = "list-d-s";
    else if(opt_yield==3)
      nameTest = "list-id-s";
    else if(opt_yield==4)
      nameTest = "list-l-s";
    else if(opt_yield==5)
      nameTest = "list-il-s";
    else if(opt_yield==6)
      nameTest = "list-dl-s";
    else if(opt_yield==7)
      nameTest = "list-idl-s";
  }
  else if (strcmp(sync_arg,"m")==0)
  {
    if(opt_yield==0)
      nameTest = "list-none-m";
    else if(opt_yield==1)
      nameTest = "list-i-m";
    else if(opt_yield==2)
      nameTest = "list-d-m";
    else if(opt_yield==3)
      nameTest = "list-id-m";
    else if(opt_yield==4)
      nameTest = "list-l-m";
    else if(opt_yield==5)
      nameTest = "list-il-m";
    else if(opt_yield==6)
      nameTest = "list-dl-m";
    else if(opt_yield==7)
      nameTest = "list-idl-m";
  }
  else //none
  {
    if(opt_yield==0)
      nameTest = "list-none-none";
    else if(opt_yield==1)
      nameTest = "list-i-none";
    else if(opt_yield==2)
      nameTest = "list-d-none";
    else if(opt_yield==3)
      nameTest = "list-id-none";
    else if(opt_yield==4)
      nameTest = "list-l-none";
    else if(opt_yield==5)
      nameTest = "list-il-none";
    else if(opt_yield==6)
      nameTest = "list-dl-none";
    else if(opt_yield==7)
      nameTest = "list-idl-none";
  }
}


void Mul_SortedList_insert(SortedListElement_t *element)
{
  int list_num = hashKey(element->key); //hashkey return between [0 , # of sublists-1] 
  if(strcmp(sync_arg,"m")==0)
    {
      // clock_gettime(CLOCK_MONOTONIC, &start_time);
      pthread_mutex_lock(&mutex_lock[list_num]);
      //      clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  if(strcmp(sync_arg,"s")==0)
    {
      //      clock_gettime(CLOCK_MONOTONIC, &start_time);
      while(__sync_lock_test_and_set(&spin_lock[list_num],1));
      //      clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  SortedList_insert(&listheads[list_num], element);

  if(strcmp(sync_arg,"s")==0)
    __sync_lock_release(&spin_lock[list_num]);
  if(strcmp(sync_arg,"m")==0)
    pthread_mutex_unlock(&mutex_lock[list_num]);
}

SortedListElement_t *Mul_SortedList_lookup(const char * key)
{
  SortedListElement_t * ret = NULL;
  int list_num = hashKey(key);
  if(strcmp(sync_arg,"m")==0)
    {
      //      clock_gettime(CLOCK_MONOTONIC, &start_time);
      pthread_mutex_lock(&mutex_lock[list_num]);
      //      clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  if(strcmp(sync_arg,"s")==0)
    {
      //      clock_gettime(CLOCK_MONOTONIC, &start_time);
      while(__sync_lock_test_and_set(&spin_lock[list_num],1));
      //      clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  if((ret = SortedList_lookup(&listheads[list_num], key))==NULL)
    {
      fprintf(stderr,"Error from SortedList_lookup()!\n");
      exit(2);
    }
  if(strcmp(sync_arg,"s")==0)
    __sync_lock_release(&spin_lock[list_num]);
  if(strcmp(sync_arg,"m")==0)
    pthread_mutex_unlock(&mutex_lock[list_num]);
  return ret;
}

int Mul_SortedList_length()
{
  int length = 0;
  for (int i = 0; i < num_sub_lists; i++)
    {
      if(strcmp(sync_arg,"m")==0)
	{
	  //  clock_gettime(CLOCK_MONOTONIC, &start_time);
	  pthread_mutex_lock(&mutex_lock[i]);
	  // clock_gettime(CLOCK_MONOTONIC, &end_time);
	}
      if(strcmp(sync_arg,"s")==0)
	{
	  //	  clock_gettime(CLOCK_MONOTONIC, &start_time);
	  while(__sync_lock_test_and_set(&spin_lock[i],1));
	  //	  clock_gettime(CLOCK_MONOTONIC, &end_time);
	}
      int l=0;
      if((l = SortedList_length(&listheads[i])) == -1)
	{
	  fprintf(stderr,"Error from SortedList_length()!\n");
	  exit(2);
	}
      length+=l;


      if(strcmp(sync_arg,"s")==0)
	__sync_lock_release(&spin_lock[i]);
      if(strcmp(sync_arg,"m")==0)
	pthread_mutex_unlock(&mutex_lock[i]);
     }
  return length;
}

void Mul_SortedList_delete (SortedList_t* e)
{
  int list_num = hashKey(e->key);
  if(strcmp(sync_arg,"m")==0)
    {
      //    clock_gettime(CLOCK_MONOTONIC, &start_time);
    pthread_mutex_lock(&mutex_lock[list_num]);
    //    clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  if(strcmp(sync_arg,"s")==0)
    {
      //      clock_gettime(CLOCK_MONOTONIC, &start_time);
      while(__sync_lock_test_and_set(&spin_lock[list_num],1));
      //      clock_gettime(CLOCK_MONOTONIC, &end_time);
    }
  if(SortedList_delete(e)!=0)
  {
    fprintf(stderr, "Error from SortedList_delete()!\n");
    exit(2);
  }
  if(strcmp(sync_arg,"s")==0)
    __sync_lock_release(&spin_lock[list_num]);
  if(strcmp(sync_arg, "m")==0)
    pthread_mutex_unlock(&mutex_lock[list_num]);
  //   wait_time += (get_nanosec_from_timespec(&end_time)-get_nanosec_from_timespec(&start_time));
}


void * thread_worker(void * threadNum)
{
  struct timespec start_time, end_time;
  for(int i = *((int*)threadNum); i < threads*iterations; i=i+threads)
    {
      clock_gettime(CLOCK_MONOTONIC, &start_time);
      Mul_SortedList_insert(&pool[i]);
      clock_gettime(CLOCK_MONOTONIC, &end_time);
      wait_time[*((int*)threadNum)] += (get_nanosec_from_timespec(&end_time)-get_nanosec_from_timespec(&start_time));
    }

  clock_gettime(CLOCK_MONOTONIC, &start_time);
  Mul_SortedList_length( *((int*)threadNum));
  clock_gettime(CLOCK_MONOTONIC, &end_time);
  wait_time[*((int*)threadNum)] += (get_nanosec_from_timespec(&end_time)-get_nanosec_from_timespec(&start_time));

  SortedList_t* e;
  for(int i = *((int*)threadNum); i < threads*iterations; i=i+threads)
    {
      clock_gettime(CLOCK_MONOTONIC, &start_time);
      e= Mul_SortedList_lookup(pool[i].key);
      clock_gettime(CLOCK_MONOTONIC, &end_time);
      wait_time[*((int*)threadNum)] += (get_nanosec_from_timespec(&end_time)-get_nanosec_from_timespec(&start_time));
      clock_gettime(CLOCK_MONOTONIC, &start_time);
      Mul_SortedList_delete(e);
      clock_gettime(CLOCK_MONOTONIC, &end_time);
      wait_time[*((int*)threadNum)] += (get_nanosec_from_timespec(&end_time)-get_nanosec_from_timespec(&start_time));
    }
  return NULL;
}


int main(int argc, char* argv[])
{
  signal(SIGSEGV, segfault_handler);
   struct option args[]=
     {
       {"threads",1,NULL,'t'},
       {"iterations",1,NULL,'i'},
       {"yield",1,NULL,'y'},
       {"sync",1,NULL,'s'},
       {"lists",1,NULL,'l'},
       {0,0,0,0},
     };

   int opt;
   while((opt=getopt_long(argc,argv,"",args,NULL))!=-1)
     {
       switch(opt)
	 {
	 case 't':
	   threads = atoi(optarg);

	   break;
	 case 'i':
	   iterations = atoi(optarg);
	   break;
	 case 'y':
	   for (long unsigned int  i = 0; i < strlen(optarg); i++) 
	     {
	       if (optarg[i] == 'i') 
		   opt_yield |= INSERT_YIELD;
	       else if (optarg[i] == 'd') 
		 opt_yield |= DELETE_YIELD;
	       else if (optarg[i] == 'l') 
		 opt_yield |= LOOKUP_YIELD;
	     }
	   break;
	 case 's':
	   sync_arg=optarg;
	   break;
	 case 'l':
	   num_sub_lists=atoi(optarg);
	   break;

	 default:
	   fprintf(stderr,"Correct usege: ./lab2_add --threads=numThread --iterations=numIteration --yield=[idl] --sync='m' --sync='s'\n");
	   exit(1);
	 }
     }
   struct timespec start_time, end_time;
   unsigned long totalTime = 0;
   listheads = malloc(sizeof(SortedListElement_t) * num_sub_lists);
   mutex_lock  = malloc(sizeof(pthread_mutex_t) * num_sub_lists);
   spin_lock = malloc(sizeof(long) * num_sub_lists);
   wait_time = malloc(sizeof(unsigned long)*threads);
   memset(wait_time,0,threads);
  int total_element = threads * iterations;
   pool = malloc(sizeof(SortedListElement_t)*total_element);
   char randomKey[total_element][128];
   srand(time(0));

   for(int i=0; i< total_element; i++)
     {
       for(int j=0; j<127; j++)
	 {
	   randomKey[i][j]=rand()%94+33;
	 }
       randomKey[i][127]='\0';
       pool[i].key = randomKey[i];
     }

   
   int *count = malloc(threads * sizeof(int));
   for (int i = 0; i < threads; i++)
     count[i] = i;
   if(clock_gettime(CLOCK_MONOTONIC, &start_time) <0)
     fprintf(stderr,"Error from obtaining the begin time: %s\n", strerror(errno));
   pthread_t pthreads[threads];
   for (int i = 0; i < threads; i++)
     {
       if( pthread_create(&pthreads[i],NULL,thread_worker,&count[i])<0)
	 fprintf(stderr,"Error from pthread_create(): %s\n", strerror(errno));
     }
   for (int i = 0; i < threads; i++)
     {
       if( pthread_join(pthreads[i],NULL) < 0)
	 fprintf(stderr,"Error from pthread_jion(): %s\n", strerror(errno));
     }
   if(clock_gettime(CLOCK_MONOTONIC, &end_time) <0)
     fprintf(stderr,"Error from obtaining the end time: %s\n", strerror(errno));
   totalTime =  get_nanosec_from_timespec(&end_time) - get_nanosec_from_timespec(&start_time);  //diff stores the execution time in ns
  int numOps = threads*iterations*3;

  getName();
  unsigned long wait_time_total = 0;
  for(int i =0;i<threads;i++)
   wait_time_total += wait_time[i];

 fprintf(stdout,"%s,%d,%d,%d,%d,%lu,%lu,%lu\n",nameTest,threads,iterations,num_sub_lists,numOps,totalTime,totalTime/numOps,wait_time_total/(4*iterations*threads));


 if(strcmp(sync_arg,"m")==0)
   free(mutex_lock);
 else if(strcmp(sync_arg,"s")==0)
   free(spin_lock);
 free(listheads);
 free(wait_time);
 free(pool);
 exit(0);
}

