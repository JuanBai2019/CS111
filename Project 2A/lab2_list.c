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
long lock = 0;
SortedListElement_t listhead;
SortedListElement_t * pool;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int opt_yield = 0;
void segfault_handler()
{
  fprintf(stderr, "Encounter segmentation fault\n ");
  exit(2);
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

void * thread_worker(void * threadNum)
{
  
  if(strcmp(sync_arg,"s")==0)
    while(__sync_lock_test_and_set(&lock,1));
  if(strcmp(sync_arg,"m")==0)
      pthread_mutex_lock(&mutex);
  for(int i = *((int*)threadNum); i < threads*iterations; i=i+threads)
    { 
      SortedList_insert(&listhead,&pool[i]);
      if(SortedList_length(&listhead)==-1)
      {
	fprintf(stderr, "Error from SortedList_length()!\n");
	exit(2);
      }
    }
  SortedList_t* e;
  for(int i = *((int*)threadNum); i < threads*iterations; i=i+threads)
    { 
      e = SortedList_lookup(&listhead,pool[i].key);
      if(e==NULL)
	{
	  fprintf(stderr, "Error from SortedList_lookup()!\n");
	  exit(2);
	}
      if(SortedList_delete(e)!=0)
	{
	  fprintf(stderr, "Error from SortedList_delete()!\n");
	  exit(2);
	}
    }
  
  if(strcmp(sync_arg,"s")==0)
    __sync_lock_release(&lock);
  if(strcmp(sync_arg, "m")==0)
    pthread_mutex_unlock(&mutex);
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

	 default:
	   fprintf(stderr,"Correct usege: ./lab2_add --threads=numThread --iterations=numIteration --yield=[idl] --sync='m' --sync='s'\n");
	   exit(1);
	 }
     }

   struct timespec begin, end;
   unsigned long totalTime = 0;


   
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

   if(clock_gettime(CLOCK_MONOTONIC, &begin) <0)
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
   if(clock_gettime(CLOCK_MONOTONIC, &end) <0)
     fprintf(stderr,"Error from obtaining the end time: %s\n", strerror(errno));
   totalTime =  get_nanosec_from_timespec(&end) - get_nanosec_from_timespec(&begin);  //diff stores the execution time in ns
  int numOps = threads*iterations*3;

 getName();
 fprintf(stdout,"%s,%d,%d,1,%d,%lu,%lu\n",nameTest,threads,iterations,numOps,totalTime,totalTime/numOps);
 if(strcmp(sync_arg,"m")==0)
   pthread_mutex_destroy(&mutex);


 free(pool);
 exit(0);
}

