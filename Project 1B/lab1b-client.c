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
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <poll.h>
#include <zlib.h>
#include <string.h>
#include <ulimit.h>


void init_compress_stream (z_stream * stream);
void compress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len); 
void decompress_stream (z_stream * stream, void * orig_buf, int orig_len, void * out_buf, int out_len);
void fini_compress_stream(z_stream * stream); 
void fini_decompress_stream(z_stream * stream);
void init_decompress_stream (z_stream * stream);
void terminal_setup();
void read_from_socket();
void read_from_stdin();
int client_connect(char*host_name, unsigned int port);//e.g. host_name:"google.com\", port:80, return the socket for subsequent communication

int read_socket_bytes = 0;
int write_socket_bytes = 0;
int read_stdin_bytes = 0;
int write_stdin_bytes = 0;
int socket_fd;
int log_flag=0;
int port_flag=0;
int log_fd = 0;
char* log_filename;
int compress_flag = 0;
int bufSize = 256;
z_stream stream_decomp;
z_stream stream_comp;

struct termios old_tmp;
int main (int argc, char* argv[])
{
  
  unsigned int port;
  tcgetattr(0, &old_tmp);
  terminal_setup();
  ulimit(UL_SETFSIZE, 10000);
  struct option args[] =
    {
      {"port", 1,  NULL,  'p'},
      {"log", 1, NULL, 'l'},
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
	port = atoi(optarg);
	break;
      case 'l':
	log_flag = 1;
	log_filename = optarg;
	break;
      case 'c':
	compress_flag = 1;
	break;
      default:
	printf("Correct usage: ./lab1b-client --shell=program --port=port_num --log --compress\n");
	exit(1);
    }
  }
  if(port_flag==0)
  {
    fprintf(stderr,"Error from port_flag!");
    exit(1);
  }
  if(log_flag==1)
    log_fd = creat(log_filename, S_IRUSR | S_IWUSR | S_IRGRP);
  
  char * hostname="localhost";
  socket_fd=client_connect(hostname,port);//hostname,port number obtained from cmd_args
  // set the terminal to the non-canonical, no echo mode
  struct pollfd pollfds[2];
  pollfds[0].fd = 0;
  pollfds[0].events = POLLIN + POLLHUP + POLLERR;
  pollfds[1].fd = socket_fd;
  pollfds[1].events = POLLIN + POLLHUP + POLLERR;

  while (1)
  {
    poll(pollfds,2,-1);//poll the socket_fd and stdin
    if ((pollfds[1].revents & POLLIN))//read from socket, process special characters, send to stdout
      read_from_socket();
    if ((pollfds[0].revents & POLLIN))//read from stdin, process special characters, send to stdout and socket_fd
      read_from_stdin();
    if(pollfds[0].revents & (POLLHUP | POLLERR))
    {
      fprintf(stderr,"Error to read from read end/stdin. %s\n", strerror(errno));
      read_from_socket();
      tcsetattr(0,TCSANOW,&old_tmp); //restore the old terminal
      exit(0);
    }
  }
}

void terminal_setup()
{

  struct termios tmp;
  tcgetattr(0, &tmp); //get old terminal and modify it
  tmp.c_iflag = ISTRIP;
  tmp.c_oflag = 0;
  tmp.c_lflag = 0;
  tcsetattr(0,TCSANOW,&tmp); //set the modified terminal using tmp
}


int client_connect(char*host_name, unsigned int port)//e.g. host_name:"google.com", port:80, return the socket for subsequent communication
{
  struct sockaddr_in serv_addr;//encode the IP address and the port for the remote
  int sockfd=socket(AF_INET, SOCK_STREAM,0);//AF_INET:IPv4,SOCK_STREAM:TCP connection
  struct hostent*server=gethostbyname(host_name);//convert host_name to IP addr
  memset(&serv_addr, 0, sizeof(struct sockaddr_in));
  serv_addr.sin_family=AF_INET;//address is Ipv4
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);//copy ip address from server to serv_addr
  serv_addr.sin_port = htons(port); //setup the port
  connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));//initiate the connection to server
  return sockfd;

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


void read_from_socket() //read from server/socket
{
  char buffer[bufSize];
  memset(buffer,0,bufSize);
  sleep(1);
  read_socket_bytes = read(socket_fd,&buffer,sizeof(buffer));
  if (read_socket_bytes < 0)
  {
    fprintf(stderr,"Error to read from socket_fd to buffer: %s\n",strerror(errno));
    exit(1);
  }
  if(log_flag && (read_socket_bytes > 0) && (compress_flag == 0))
  { 
    char read_in_bytes[10];
    sprintf(read_in_bytes, "%d", read_socket_bytes);
    write(log_fd, "RECEIVED ", 9);
    write(log_fd,read_in_bytes, strlen(read_in_bytes));
    write(log_fd, " bytes: ", 8);
    write(log_fd, &buffer, read_socket_bytes);
    write(log_fd,"\n",1);
  }
  if(compress_flag == 0)
    {
      for(int i = 0; i < read_socket_bytes;i++)
	{
	  if(buffer[i]==0x04) //ctrl+D
	    {
	      // close(socket_fd);
	      write_socket_bytes = write(1,"^D",2);
	      if( write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write ^D to stdout from buffer: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else if(buffer[i] == '\n') //detacting carrige return or new line
	    {
	      write_socket_bytes = write(1, "\r\n", 2);
	      if (write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write \r\n to stdout: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else
	    {
	      write_socket_bytes = write(1, &buffer[i], 1);
	      if ( write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write a single character at time to stdout: %s\n", strerror(errno));
		  exit(1);
	
		}
	    }
	}
    }
  else//compress_flag==1
    {
      int out_len = 2048;
      char out_buf[out_len];
      memset(out_buf,0,out_len);
      init_decompress_stream(&stream_decomp);
      decompress_stream(&stream_decomp, buffer, bufSize, out_buf, out_len);
      fini_decompress_stream(&stream_comp);
      for(unsigned int i = 0; i < out_len - stream_decomp.avail_out; i++)
	{
	  if(out_buf[i]==0x04) //ctrl+D
	    {
	      // close(socket_fd);
	      write_socket_bytes = write(1,"^D",2);
	      if( write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write ^D to fd from buffer: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else if(out_buf[i] == '\n') //detacting carrige return or new line
	    {
	      write_socket_bytes = write(1, "\r\n", 2);
	      if (write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write \r\n to stdout: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else
	    {
	      write_socket_bytes = write(1, &out_buf[i], 1);
	      if ( write_socket_bytes < 0)
		{
		  fprintf(stderr,"Error to write a single character at time to stdout: %s\n", strerror(errno));
		  exit(1);
		}
	    }
	}
    }
}


void read_from_stdin()
{
  char buffer[bufSize];
  char read_in_bytes[10];
  memset(buffer,0,bufSize);
  read_stdin_bytes = read(0,&buffer,sizeof(buffer));
  if (read_stdin_bytes < 0)
    {
      fprintf(stderr,"Error to read from fd to buffer: %s\n",strerror(errno));
      exit(1);
    }
  if(log_flag && (read_stdin_bytes > 0) && (compress_flag == 0))
    {
      sprintf(read_in_bytes, "%d", read_stdin_bytes);
      write(log_fd, "SENT ", 5);
      write(log_fd,read_in_bytes, strlen(read_in_bytes));
      write(log_fd, " bytes: ", 8);
      write(log_fd, &buffer, read_stdin_bytes);
      write(log_fd,"\n",1);
    }
  if (compress_flag == 0)
    {
      
      for(int i = 0; i < read_stdin_bytes;i++)
	{
	  write(socket_fd,&buffer[i],1);
	  if(buffer[i]==0x04) //ctrl+D
	    {
	      write_stdin_bytes = write(1,"^D",2);
	      if( write_stdin_bytes < 0)
		{
		  fprintf(stderr,"Error to write ^D to fd from buffer: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else if(buffer[i]==0x03) //ctrl+C
	    {
	      if( write(1,"^C",2) < 0)
		{
		  fprintf(stderr,"Error to write ^C to stdout: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else if(buffer[i] == '\n') //detacting carrige return or new line
	    {
	      write_stdin_bytes = write(1, "\r\n", 2);
	      if (write_stdin_bytes < 0)
		{
		  fprintf(stderr,"Error to write \r\n to stdout: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	  else
	    {
	      write_stdin_bytes = write(1, &buffer[i], 1);
	      if ( write_stdin_bytes < 0)
		{
		  fprintf(stderr,"Error to write a single character at time to stdout: %s\n",strerror(errno));
		  exit(1);
		}
	    }
	}//end of for loop
    }
  else //compress_flag==1
    {
      int out_len = 2048;
      char out_buf[out_len];
      memset(out_buf,0,out_len);
      init_compress_stream(&stream_comp);
      compress_stream(&stream_comp,buffer,read_stdin_bytes,out_buf,out_len);
      fini_compress_stream(&stream_comp);
      for(unsigned int i = 0; i < out_len - stream_comp.avail_out; i++)
        {
	  write(socket_fd,&out_buf[i],1);
	}
      for(int i = 0; i < read_stdin_bytes;i++)
      {
	if(buffer[i]==0x04) //ctrl+D
	  {
	    write_stdin_bytes = write(1,"^D",2);
	    if( write_stdin_bytes < 0)
	      {
		fprintf(stderr,"Error to write ^D to stdout from buffer: %s\n",strerror(errno));
		exit(1);
	      }
	  }
	else if(buffer[i]==0x03) //ctrl+C
	  {
	    if( write(1,"^C",2) < 0)
	      {
		fprintf(stderr,"Error to write ^C to stdout: %s\n",strerror(errno));
		exit(1);
	      }
	  }
	else if(buffer[i] == '\n') //detacting carrige return or new line
	  {
	    write_stdin_bytes = write(1, "\r\n", 2);
	    if (write_stdin_bytes < 0)
	      {
		fprintf(stderr,"Error to write \r\n to stdout: %s\n",strerror(errno));
		exit(1);
	      }
	  }
	else
	  {
	    write_stdin_bytes = write(1, &buffer[i], 1);
	    if ( write_stdin_bytes < 0)
	      {
		fprintf(stderr,"Error to write a single character at time to stdout: %s\n",strerror(errno));
		exit(1);
	      }
	  }
      }
      //compress_stream(&stream_comp, buffer,bufSize ,out_buf, out_len);
      //for(unsigned int i=0; i < out_len - stream_comp.avail_out ;i++)
      //  write(socket_fd, &out_buf[i],1);
      //write to log_fd
      write(log_fd,"RECEIVED ",9);
      char read_in_bytes[10];
      sprintf(read_in_bytes,"%d",out_len - stream_comp.avail_out);
      write(log_fd,read_in_bytes,strlen(read_in_bytes));
      write(log_fd," bytes: ",8);
      write(log_fd, &out_buf,out_len - stream_comp.avail_out);
      write(log_fd,"\n",1);
    }//end else//compress_flag==1 
}












