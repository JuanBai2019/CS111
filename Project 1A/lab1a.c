/*
NAME:Juan Bai
EMAIL:Daisybai66@yahoo.com
ID:105364754
*/

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <sys/wait.h>
#include <poll.h>

struct terrmios
{
  tcflag_t c_iflag;     //input mode
  tcflag_t c_oflag;     //output modes
  tcflag_t c_cflag;     //control modes
  tcflag_t c_lflag;     //local modes
  cc_t c_cc[NCCS];      //special characters
};

int tcgetattr(int fd, struct termios *termios_p);  //get the state of the terminal specified by fd
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p); //set the state of the terminal specified by fd
//optional_actions:


void terminal_setup()
{
  
  struct termios tmp;
  
  tcgetattr(0, &tmp); //get old terminal and modify it 
  tmp.c_iflag = ISTRIP;
  tmp.c_oflag = 0;
  tmp.c_lflag = 0;
  tcsetattr(0,TCSANOW,&tmp); //set the modified terminal using tmp
}

int to_shell[2];
int from_shell[2];
pid_t ret;
int status;
void sigpipe_handler()
{
  close(to_shell[1]);
  close(from_shell[0]);
  kill(ret,SIGINT);
  waitpid(ret,&status,0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
  exit(0);
}

struct termios old_tmp;
int main(int argc, char*argv[])
{
  int shell_flag = 0;
  int bufSize = 256;
  char buffer[bufSize];
  struct termios old_tmp;
  tcgetattr(0, &old_tmp);
  terminal_setup();
  char *shell_program;
  int readBytes;
  struct option args[] =
  {
    {"shell", 1,  NULL,  's'},
    {0, 0, 0, 0}
  };
  
  int opt = 0;
  while((opt = getopt_long(argc, argv,"", args, NULL))!=-1)
    {
      switch(opt)
	{
        case 's':
	  shell_flag = 1;
	  shell_program = optarg;
	  break;
	default:
	  printf("Correct usage: ./lab1a --shell=program \n");
	  exit(1);
	}
    }

  //set array buffer to 0 to avoid trash
  for(int i=0; i<bufSize; i++)
  {
    buffer[i]=0;
  }

  if(!shell_flag)
    {
      while(1)
	{
	  int writeBytes = 0;
	  //read input from keyboard into buffer
	  readBytes = read(0,&buffer,sizeof(buffer));
	  if (readBytes < 0)
	    {
	      fprintf(stderr,"Error to read from fd to buffer: %s\n",strerror(errno));
	      exit(1);

	    }
	  for(int i=0; i < readBytes; i++)
	    {
	 
	      if(buffer[i]==0x4) //ctrl+D
		{
		  writeBytes = write(1, "^D", 2);
		  if ( writeBytes < 0)
		    {
		      fprintf(stderr,"Error to write ^D to fd from buffer: %s\n",strerror(errno));
		      exit(1);
		    }
		  tcsetattr(0,TCSANOW,&old_tmp); //restore the old terminal
		  exit(0);
		}
	      else if(buffer[i] == '\r' || buffer[i] == '\n') //detacting carrige return or new line
		{
		  writeBytes = write(1, "\r\n", sizeof("\r\n")-1); //four characters + one null character
		  if ( writeBytes < 0)
                    {
                      fprintf(stderr,"Error to write \r\n from buffer to fd: %s\n",strerror(errno));
                      exit(1);
                    }
		}	      
	      else
		{
		  writeBytes = write(1, &buffer[i], 1);
		  if ( writeBytes < 0)
                    {
                      fprintf(stderr,"Error to write character from buffer to fd: %s\n",strerror(errno));
                      exit(1);
                    }
		}
	    }
	} //end of while(1)
    }//end of if(!shell_flag)

  else //create a new process/child 
    {
      //signal(SIGINT, sigpipe_handler);
      signal(SIGPIPE, sigpipe_handler);
      pipe(to_shell);
      pipe(from_shell);
      ret = fork();
      if(ret==0)//child process********************
	{
	  close(to_shell[1]); //close the write end of pipe
	  close(from_shell[0]);
	  close(0);
	  dup(to_shell[0]); //point to the read end of pipe
	  close(to_shell[0]); //close the read end of pipe
	  
	  close(1); //close stdin
	  close(2); //close stdout
	  dup(from_shell[1]); //point the lowest unusedfd to write end
	  dup(from_shell[1]);
	  close(from_shell[1]);
	  //int execlp(const char *file, const char *arg,(char*)NULL);
	  if(execlp(shell_program, shell_program,NULL) < 0)
	    {
	      fprintf(stderr, "Error to replace the current process image with a new process image. \n");
	      exit(1);
	    }
	}
      else if(ret > 0) //parent process********************
	{
	  int exit_flag=0;
	  close(to_shell[0]);
	  close(from_shell[1]);
	  
          struct pollfd pollfds[2];
          pollfds[0].fd = 0;
          pollfds[0].events = POLLIN + POLLHUP + POLLERR;
          //POLLIN: data to read, POLLHUP: FD is closed by the other side, POLLERR: error occurred   
          pollfds[1].fd = from_shell[0];
          pollfds[1].events = POLLIN + POLLHUP + POLLERR;
	  
	  while(1)
	    {
	      int writeBytes = 0;
	      poll(pollfds, 2, -1); // -1 means no time out. 
	      if (pollfds[0].revents & POLLIN) 
		{
		  //read input from keyboard into buffer
		  readBytes = read(0,&buffer,sizeof(buffer));		
		  if (readBytes < 0)
		    {
		      fprintf(stderr,"Error to read from fd to buffer: %s\n",strerror(errno));
		      exit(1);
		    }
		  for(int i = 0; i < readBytes;i++)//bufSize; i++)
		    {	      
		    if(buffer[i]==0x4) //ctrl+D
		      {
			close(to_shell[1]);
			writeBytes = write(1,"^D",2);
			if ( writeBytes < 0)
			  {
			    fprintf(stderr,"Error to write ^D to fd from buffer: %s\n",strerror(errno));
			    exit(1);
			  }
			//will be blocked if not set flag
			exit_flag=1;
			//			break;
			



			//tcsetattr(0,TCSANOW,&old_tmp); //restore the old terminal
			//waitpid(ret,&status,0);
			//fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
			//exit(0);
		      }
		    else if(buffer[i]==0x03) //ctrl+C
		      {
			//close(to_shell[1]);
			if( write(1,"^C",2) < 0)
			  {
			    fprintf(stderr,"Error to write ^C to stdout: %s\n",strerror(errno));
			    exit(1);
			  }
			signal(SIGINT, sigpipe_handler);
			if(kill(ret,SIGINT)<0)
			  printf("Error to kill child process.");
			/*
			readBytes=read(from_shell[0],&buffer,sizeof(buffer));
			if(readBytes<0)
			  fprintf(stderr,"Error to read after kill: %s\n",strerror(errno));
			for(int i=0; i<readBytes;i++)
			  {
			    if(write(1,&buffer[i],1)<0)
			      {
				fprintf(stderr,"Error to write to stdout from buffer after kill syscall: %s\n",strerror(errno));
				//exit(1);
			      }
			    }
			//close(from_shell[0]);
			*/
			//	exit_flag=1;
			//exit(0);
		      }		   
		    else if(buffer[i] == '\r' || buffer[i] == '\n') //detacting carrige return or new line
		      {
			writeBytes = write(1, "\r\n", 2 ); //four characters + one null character
			if (writeBytes < 0)
			  {
			    fprintf(stderr,"Error to write \r\n to stdout: %s\n",strerror(errno));
			    exit(1);
			  }
			
			writeBytes = write(to_shell[1], "\n",1);
			if ( writeBytes < 0)
			  {
			    fprintf(stderr,"Error to write \r\n from buffer to fd: %s\n",strerror(errno));
			    exit(1);
			  }
		      }
		    else
		      {
			writeBytes = write(1, &buffer[i], 1);
			if ( writeBytes < 0)
			  {			  
			    fprintf(stderr,"Error to write a single character at time to stdout: %s\n",strerror(errno));
			    exit(1);
			  }
			writeBytes = write(to_shell[1], &buffer[i], 1);
			if ( writeBytes < 0)
			  {
			    fprintf(stderr,"Error to write character from buffer to fd: %s\n",strerror(errno));
			    exit(1);
			  }
		      }
		    }//end of for loop
		}//end of if(pollfds[0].....)
	      else if (pollfds[1].revents & POLLIN)
		{
                  readBytes = read(from_shell[0], &buffer, sizeof(buffer));
                  //forward to stdout.
		  for(int i=0; i<readBytes;i++)
		    {
		      if(buffer[i] == '\n') //detacting carrige return or new line
			{
			  if(write(1, "\r\n", 2)<0)
			    {
			      fprintf(stderr,"Error to write \r\n to stdout from LOOP else if (pollfds[1].revents & POLLIN): %s\n",strerror(errno));
			      exit(1);
			    }
			}
		      else
			{
			  if(write(1,&buffer[i],1)<0)
			    {
			      fprintf(stderr,"Error to write to stdout from buffer at LOOP else if (pollfds[1].revents & POLLIN): %s\n",strerror(errno));
                              exit(1);
			    }
			}
		    }
		}
	      else if(pollfds[0].revents & (POLLHUP | POLLERR))
		{
		  //waitpid(ret,&status,0);
		  //fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
		  // exit(0);
		  fprintf(stderr,"Error to read from read end/stdin. %s\n", strerror(errno));
		  //exit(1);
		  exit_flag=1;
		}
	      if(exit_flag==1)
		break;
	    }//end of while(1)

	  waitpid(ret,&status,0);
	  tcsetattr(0,TCSANOW,&old_tmp); //restore the old terminal       
	  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
	  exit(0);
	}
      else  //failed to create a new process
	{
	  fprintf(stderr, "Error creating from fork %s\n", strerror(errno));
	  exit(1);
	}
    }
  }//end of main
 
