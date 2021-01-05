/*
NAME:Juan Bai
EMAIL:Daisybai66@yahoo.com
ID:105364754
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <getopt.h>

//sigmentation handler for catching segmentation errors
void sigsegv_handler()
{
  fprintf(stderr,"ERROR: Segmentation fault.\n");
  exit(4);

}

//create segmentation fault
void creat_segfault(int catch)
{
  char *ptr = NULL;
  if(catch == 1)
    {
      signal(SIGSEGV, sigsegv_handler);
    }
  (*ptr) = 0;
  return;
}

//input redirection
void input_redirect(char *input_file)
{
  int infd = open(input_file, O_RDONLY);
  if(infd < 0)
    {
      fprintf(stderr, "ERROR: Open the input file %s failed because %s\n", input_file, strerror(errno));
      exit(2);
    }
  close(0);
  dup(infd);
  close(infd);
}

//output redirection
void output_redirect(char *output_file)
{
  int outfd = creat(output_file, S_IRUSR | S_IWUSR | S_IRGRP );
      if(outfd < 0)
	{
	  fprintf(stderr, "ERROR: Create the output file %s failed because %s\n", output_file, strerror(errno));
	  exit(3);
	}
      close(1);
      dup(outfd);
      close(outfd);
}


//four types long option arguments
struct option args[] =
{
  {"input",    1,   NULL,  'i'},
  {"output",   1,   NULL,  'o'},
  {"segfault", 0,   NULL,  's'},
  {"catch",    0,   NULL,  'c'},
  {0, 0, 0, 0}
};
                           


int main(int argc, char*argv[])
{
  int input_flag = 0;
  int output_flag = 0;
  int segfault_flag = 0;
  int catch_flag = 0;
  char *input_file;
  char *output_file;
  
  //get options
  int i = 0;
  int input_first = 0;
  while((i = getopt_long(argc, argv,"", args, NULL))!= -1)
    {
      if(input_flag == 1 && output_flag == 0)
	input_first = 1;
      
      switch(i)
      {
        case 'i':
	    input_flag = 1;
	    input_file = optarg;
	    break;
        case 'o':
	    output_flag = 1;
	    output_file = optarg;
	    break;
        case 's':
	    segfault_flag = 1;
	    break;
        case 'c':
	    catch_flag = 1;
	    break;
      default:
	printf("Correct usage: ./lab0 --input=input_file --output=ouput_file --segfault --catch\n");
	exit(1);
	
      }
   }

  //  printf("%i, %i, %i, %i, %s, %s" , input_flag, output_flag, segfault_flag, catch_flag, input_file, output_file);
  //handle flags
  if(input_flag && !output_flag)
    {
      input_redirect(input_file);
    }

  if(!input_flag && output_flag)
    {
      output_redirect(output_file);
    }
  if(input_flag && output_flag)
    {
      if(input_first)
	{
	  input_redirect(input_file);
	  output_redirect(output_file);
	}
      else
	{
	  output_redirect(output_file);
	  input_redirect(input_file);
	}

    }
  

  if(segfault_flag)
    {
      creat_segfault(catch_flag);

    }


  //******************read fron stdin and write to stdout**********************************
  char buffer;
  const  int bufSize = 1;
  int numRead, numWrite;


  while(1)
    {
      numRead = read(0, &buffer, bufSize);
      if(numRead<0)
        {
          fprintf(stderr, "ERROR: Failed to read from input file %s. \n", strerror(errno));
          exit(2);
        }
      else if(numRead == 0)
        {
          exit(0);
        }


      numWrite = write(1, &buffer, numRead);
      if(numWrite<0)
	{
	  fprintf (stderr, "ERORR: Failed to write output file %s. \n", strerror(errno));
	  exit(3);
	}
      else if (numWrite == 0)
	{
	  exit(0);
	}
    }
}


