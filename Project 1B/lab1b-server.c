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
#include <sys/stat.h>
#include <poll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <mcrypt.h>
#include <zlib.h>
#include <string.h>
#include <ulimit.h>


void init_compress_stream (z_stream * stream);
void compress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len);
void decompress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len);
void fini_compress_stream(z_stream * stream);
void fini_decompress_stream(z_stream * stream);
void init_decompress_stream (z_stream * stream);
int server_connect(unsigned int port_num);
void sigpipe_handler();
void read_from_socket();
void read_from_shell();
//MCRYPT session init_session();


int socket_fd;
int to_shell[2];
int from_shell[2];
int bufSize = 256;
int read_bytes;
int write_bytes;
int status;
pid_t ret;
z_stream stream_decomp;
z_stream stream_comp;
int log_flag=0;
int log_fd = 0;
char* log_filename;
int compress_flag = 0;
int port_flag = 0;

int main(int argc, char *argv[])
{
  int portNumber = 0;
  // int shell_flag = 0;
  //  int compress_flag = 0;
  char *shell_program = "/bin/bash";
  struct option args[] =
    {
      {"port", 1,  NULL,  'p'},
      {"shell", 1, NULL, 's'},
      {"compress", 0, NULL, 'c'},
      {0, 0, 0, 0}
    };

  int opt = 0;
  while((opt = getopt_long(argc, argv,"", args, NULL))!=-1)
    {
      switch(opt)
	{
	case 'p':
	  port_flag = 1;
	  portNumber = atoi(optarg);//convert ASCII to integers
	  break;
	case 's':
	  //shell_flag = 1;
	  shell_program = optarg;
	  break;
	case 'c':
	  compress_flag = 1;
	  break;
	default:
	  printf("Correct usage: ./lab1a --shell=program \n");
	  exit(1);
	}
    }
  socket_fd = server_connect(portNumber);
  if(socket_fd < 0)//failed to connect
    {
      fprintf(stderr, "Error to connect from server_connect: %s\n", strerror(errno));
      exit(1);
    }

  signal(SIGPIPE, sigpipe_handler);
  pipe(to_shell);
  pipe(from_shell);
  ret = fork();//ret return a child pid
  
  if(ret < 0)//failed to create a new process
  {
    fprintf(stderr, "Error to create new process from pork: %s\n", strerror(errno));
    exit(1);
  }
  else if(ret==0)//child process
    {
      close(to_shell[1]);//close the write end pipe 
      close(from_shell[0]);//close unused pipe
      close(0);//close the stdin
      dup(to_shell[0]);
      close(to_shell[0]);

      close(1);
      close(2);
      dup(from_shell[1]);
      dup(from_shell[1]);
      close(from_shell[1]);
      
      //int execlp(const char *file, const char *arg,(char*)NULL);
      if(execlp(shell_program, shell_program,NULL) < 0)
	{
	  fprintf(stderr, "Error to replace the current process image with a new process image. \n");
	  exit(1);
	}
    }
  else //parent process
    {
      close(to_shell[0]);
      close(from_shell[1]);
      
      struct pollfd pollfds[2];
      pollfds[0].fd = socket_fd;
      pollfds[0].events = POLLIN + POLLHUP + POLLERR;
      //POLLIN: data to read, POLLHUP: FD is closed by the other side, POLLERR: error occurred
      pollfds[1].fd = from_shell[0];
      pollfds[1].events = POLLIN + POLLHUP + POLLERR;

      while (1) 
	{
	  poll(pollfds,2,-1); //poll the socket_fd and from_shell[0]
	  if (pollfds[0].revents & POLLIN) //read from socket_fd, process special characters, send to to_shell[1]
	    read_from_socket();
	  if (pollfds[1].revents & POLLIN)//read from from_shell[0], process special characters, send to socket_fd
	    read_from_shell();
	  if (pollfds[1].revents & (POLLHUP | POLLERR))//Proceed to exit process: read every last byte from from_shell[0], write to socket_fd, get the exit status of the process and report to stderr.
	  {
	      read_from_shell();
	      waitpid(ret, &status,0);
	      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
	      exit(0);
	  }
	}
    }
}

void sigpipe_handler()
{
  close(to_shell[1]);
  close(from_shell[0]);
  kill(ret, SIGINT);
  waitpid(ret, &status,0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
  exit(0);
}
/*
MCRYPT session init_session()
{
  int keylen;
  int iv_size;
  char key_buf[SIZE_OF_KEY_BUF];
  // read key from the specified file, store key results in key_buf, length of key in keylen;
  session = mcrypt_module_open("twofish", NULL, "cfb", NULL);
  char *iv = malloc(mcrypt_enc_get_iv_size(session));  //iv = initial vector
  memset(iv, 0, mcrypt_enc_get_iv_size(session));
  mcrypt_generic_init(session, key_buf, keylen, iv);
  return session; 
}
*/

int server_connect(unsigned int port_num)
//port_num: which port server listens to, return socket for subsequent communication
{
  struct sockaddr_in serv_addr, cli_addr;  //server_address, client_address
  unsigned int cli_len = sizeof(struct sockaddr_in);
  int listenfd = socket(AF_INET, SOCK_STREAM, 0);
  int retfd = 0;
  memset(&serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr.sin_family = AF_INET; //ipv4 address
  serv_addr.sin_addr.s_addr = INADDR_ANY; //server can accept connection from any client IP
  serv_addr.sin_port = htons(port_num); //setup port number
  bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)); //bind socket to port
  listen(listenfd, 5); //let the socket listen, maximum 5 pending connections
  retfd = accept(listenfd, (struct sockaddr *)&cli_addr, &cli_len); //wait for client’s connection, cli_addr
  return retfd;
}


void init_compress_stream (z_stream * stream)
{
  int ret = 0;
  //stream->zalloc, stream->zfree: function pointers to malloc/free, Z_NULL to use the
  //default one. stream->opaque: data passed to customized heap allocator;
  stream->zalloc = Z_NULL;
  stream->zfree = Z_NULL;
  stream->opaque = Z_NULL;
  //Second arg: compression degress: 0-9, 9: most aggressive compression, slow.
  //Z_DEFAULT_COMPRESSION: default compression degree.
  ret = deflateInit(stream, Z_DEFAULT_COMPRESSION);

  if (ret != Z_OK)
    {
      fprintf(stderr,"Error from init_compress_stream fuction: %s\n",strerror(errno));
      exit(1);
    }
}

void init_decompress_stream (z_stream * stream)
{
  int ret = 0;
  //stream->zalloc, stream->zfree: function pointers to malloc/free, Z_NULL to use the
  //default one. stream->opaque: data passed to customized heap allocator;
  stream->zalloc = Z_NULL;
  stream-> zfree = Z_NULL;
  stream-> opaque = Z_NULL;
  //Second arg: compression degress: 0-9, 9: most aggressive compression, slow.
  //Z_DEFAULT_COMPRESSION: default compression degree.
  ret = inflateInit(stream);

  if (ret != Z_OK)
    {
      fprintf(stderr,"Error from init_compress_stream fuction: %s\n",strerror(errno));
      exit(1);
    }
}


void fini_decompress_stream(z_stream * stream)//close decompress stream
{
  inflateEnd(stream);
}


void compress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len)
{
  stream->next_in = orig_buf;
  stream->avail_in = orig_len;
  stream->next_out = out_buf;
  stream->avail_out = out_len;
  //deflate will update next_in, avail_in, next_out, avail_out accordingly.
  //while (stream->avail_in > 0)
    deflate(stream, Z_SYNC_FLUSH);
  //out_buf contains the compressed data.
  //out_len - stream->avail_out contains the size of the compressed data.
}


void fini_compress_stream(z_stream * stream)
{
  deflateEnd(stream);
}

void decompress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len)
{
  stream->next_in = orig_buf;
  stream->avail_in = orig_len;
  stream->next_out = out_buf;
  stream->avail_out = out_len;
  //FIXME: What if avail_out is not enough?
  //while (stream->avail_in > 0)
    inflate(stream, Z_SYNC_FLUSH);
  //out_buf contains the decompressed data.
  //out_len - stream->avail_out contains the size of the decompressed data.
}


void read_from_socket()
{
  char buffer[bufSize];
  memset(buffer,0,bufSize);
  read_bytes = read(socket_fd, &buffer, sizeof(buffer));
  if(read_bytes < 0)
    {
      fprintf(stderr, "Error to read from socket_fd to buffer: %s\n", strerror(errno));
      exit(1);
    }
  if(compress_flag==0)
    {
      for(int i = 0; i < read_bytes; i++)
	{
	  if(buffer[i]==0x4)//control+D
	    {
	      close(to_shell[1]);
	      waitpid(ret,&status,0);
	      fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
	      exit(0);
	    }
	  else if(buffer[i]==0x03) //ctrl+C
	    {
	      if(kill(ret,SIGINT)<0)
		fprintf(stderr, "Error to kill child process: %s\n", strerror(errno));
	    }
	  else if(buffer[i] == '\r' || buffer[i] == '\n') //detacting carrige return or new line
	    {
	      write_bytes = write(to_shell[1], "\n",1);
	      if ( write_bytes < 0)
		{
		  fprintf(stderr,"Error to write \n from buffer to fd: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else
	    if(write(to_shell[1], &buffer[i], 1)<0)
	      {
		fprintf(stderr,"Error to write character from buffer to to_shell[1]: %s\n",strerror(errno));
		exit(1);
	      }
	}
    }
  else//if compress_flag==1
    {
      int out_len = 2048;
      char out_buf[out_len];
      memset(out_buf,0,out_len);
      init_decompress_stream(&stream_decomp);
      decompress_stream(&stream_decomp, buffer, read_bytes, out_buf, out_len);
      fini_decompress_stream(&stream_comp);

      for(unsigned int i = 0; i < out_len - stream_decomp.avail_out; i++)
	{
	  /*
	  if(out_buf[i]==0x04)//control+D
            {
	    close(to_shell[1]);
              waitpid(ret,&status,0);
              fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n",WTERMSIG(status),WEXITSTATUS(status));
              exit(0);
            }
          else if(out_buf[i]==0x03) //ctrl+C
            {
              if(kill(ret,SIGINT)<0)
                fprintf(stderr, "Error to kill child process: %s\n", strerror(errno));
            }
          else if(out_buf[i] == '\r' || out_buf[i] == '\n') //detacting carrige return or new line
            {
              write_bytes = write(to_shell[1], "\n",1);
              if ( write_bytes < 0)
                {
                  fprintf(stderr,"Error to write \n from buffer to fd: %s\n",strerror(errno));
                  exit(1);
		  }
            }
          else
      */
	  if(write(to_shell[1], &out_buf[i], 1)<0)
	    {
	      fprintf(stderr,"Error to write character from buffer to to_shell[1]: %s\n",strerror(errno));
	      exit(1);
	    }
	}
    }
}

void read_from_shell()
{
  char buffer[bufSize];
  memset(buffer,0,bufSize);
  read_bytes = read(from_shell[0], &buffer, sizeof(buffer));
  if(read_bytes < 0)
    {
      fprintf(stderr,"Error to read from to_shell[0] to buffer: %s\n",strerror(errno));
      exit(1);
    }
  if(compress_flag==0)
    {
      for(int i = 0; i < read_bytes;i++)
	{
	  if( buffer[i] == '\n') //detacting carrige return or new line
	    {
	      if(write(socket_fd, "\r\n", 2) < 0)
		{
		  fprintf(stderr,"Error to write \n from buffer to fd: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else
	    if(write(socket_fd, &buffer[i], 1)<0)
	      {
		fprintf(stderr,"Error to write character from buffer to socket: %s\n",strerror(errno));
		exit(1);
	      }  
	}
    }
  else//compress_flag==1
    {
      int out_len = 2048;
      char out_buf[out_len];
      memset(out_buf,0,out_len);
      init_compress_stream(&stream_comp);
      compress_stream(&stream_comp, buffer, read_bytes, out_buf, out_len);
      fini_compress_stream(&stream_comp);
      for(unsigned int i = 0; i < out_len - stream_comp.avail_out;i++)
	{
	  /*
	  if( out_buf[i] == '\n') //detacting carrige return or new line
            {
              if(write(socket_fd, "\r\n", 2) < 0)
                {
                  fprintf(stderr,"Error to write \n from buffer to fd: %s\n",strerror(errno));
                  exit(1);
                }
            }
          else
	  */
	  if(write(socket_fd, &out_buf[i], 1)<0)
	    {
	      fprintf(stderr,"Error to write character from buffer to socket: %s\n",strerror(errno));
	      exit(1);
	    }
        }
    }
}

      
  















