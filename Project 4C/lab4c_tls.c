// NAME: Juan Bai
// EMAIL: Daisybai66@yahoo.com
// ID: 105364754

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mraa.h>
#include <signal.h>
#include <mraa/gpio.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <openssl/ssl.h>

char time_storage[9];
int log_flag = 0;
int log_fd;
char get_scale_arg = 'F';
mraa_aio_context temperature_pin;
float temperature;
int sample_rate = 1;
int stop_flag = 0; //command line does not have STOP command currently
int sockfd;
SSL *sslClient;

float CtoF(float temperature)
{
    return temperature / 5 * 9 + 32;
}

float get_temperature()
{
    const int B = 4275;
    const int R0 = 100000;
    int a = mraa_aio_read(temperature_pin); //get temperature
    if(a < 0)
    {
        fprintf(stderr, "Error reading from temperature sensor\n");
        exit(1);
    }
    float R = 1023.0 / a - 1.0;
    R = R0 * R;
    temperature = 1.0 / (log(R / R0) / B + 1 / 298.15) - 273.15; //initial temperature read is in celsius

    if(get_scale_arg == 'F')
    {
        temperature = CtoF(temperature);
    }

    return temperature;
}

void cmd_line_handler(char *buf_cmd_line)
{
    // buf_cmd_line[strlen(buf_cmd_line)-1] = '\0';

    if(strcmp(buf_cmd_line, "SCALE=F") == 0) //command line option is SCALE=F
    {
        get_scale_arg = 'F';
        if(log_flag == 1)
        {
            dprintf(log_fd, "SCALE=F\n");
        }
    }
    else if(strcmp(buf_cmd_line, "SCALE=C") == 0) //command line option is SCALE=C
    {
        get_scale_arg = 'C';
        if(log_flag == 1)
        {
            dprintf(log_fd, "SCALE=C\n");
        }
    }
    else if(strcmp(buf_cmd_line, "STOP") == 0) //command line otpion is STOP
    {
        stop_flag = 1;

        if(log_flag == 1)
        {
            dprintf(log_fd, "STOP\n");
        }
    }
    else if(strcmp(buf_cmd_line, "START") == 0) //command line option is START
    {
        if(stop_flag == 1)
        {
            stop_flag = 0;
        }

        if(log_flag == 1)
        {
            dprintf(log_fd, "START\n");
        }
    }
    else if(strcmp(buf_cmd_line, "OFF") == 0) //command line option is OFF
    {
        if(log_flag == 1)
        {
            dprintf(log_fd, "OFF\n");
        }

        char off_buf[20];
        sprintf(off_buf, "%s SHUTDOWN\n", time_storage); //SHUTDOWN when interrupted
        SSL_write(sslClient, off_buf, strlen(off_buf));	
        if(log_flag == 1)
        {
            dprintf(log_fd, "%s SHUTDOWN\n", time_storage);
        }

        int SSL_shutdown(SSL *sslClient);
        void SSL_free(SSL *sslClient);
        mraa_aio_close(temperature_pin);
        exit(0);
    }
    else if(strncmp (buf_cmd_line, "LOG", 3) == 0) //command line option is LOG
    {
        if(log_flag == 1)
        {
            dprintf(log_fd, "%s\n", buf_cmd_line);
        }
    }
    else if(strncmp (buf_cmd_line, "PERIOD=", 7) == 0) //command line option is PERIOD
    {
        char *ptr = buf_cmd_line + 7;
		if(*ptr != '\0')
        {
			int temp = atoi(ptr);
			while(*ptr != '\0')
            {
				if (isdigit(*ptr) == 0) 
                {
					return;
				}
				ptr++;
			}
			sample_rate = temp;
		}

        if(log_flag == 1)
        {
            dprintf(log_fd, "%s\n", buf_cmd_line);
        }
    }
}

int main(int argc, char *argv[])
{
    temperature_pin = mraa_aio_init(1);

    static struct option long_options[] = 
    {
        {"period", required_argument, NULL, 'p'},
        {"scale", required_argument, NULL, 's'},
        {"log", required_argument, NULL, 'l'},
        {"id", required_argument, NULL, 'i'},
        {"host", required_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };
    int ch;
    int option_index = 0;
    char* get_log_arg;
    char* get_id_num;
    char* get_host_name;
    while((ch = getopt_long(argc, argv, "p:s:l:i:h:", long_options, &option_index)) != -1)
    {
        switch(ch)
        {
            case 'p':
                sample_rate = atoi(optarg);
                break;
            case 's':
                get_scale_arg = optarg[0];
                break;
            case 'l':
                log_flag = 1;
                get_log_arg = optarg;
                break;
            case 'i':
                if(strlen(optarg) != 9)
                {
                    fprintf(stderr, "Error, id number should be 9-digit long\n");
                    exit(1);
                }
                get_id_num = optarg;
                break;
            case 'h':
                get_host_name = optarg;
                break;
            default:
                fprintf(stderr, "Error, correct usage is: --scale=F/C, --period=seconds, --log=filename, --id=9-digit-number, --host=name or address, port number\n");
                exit(1);
        }
    }

    if(log_flag == 1)
    {
        log_fd = open(get_log_arg, O_RDWR|O_CREAT, 0666);//read and write
        if(log_fd < 0)
        {
            fprintf(stderr, "Error opening/creating log file: %s\n", strerror(errno));
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "Error log option is mandatory\n");
        exit(1);
    }

    //create TCP connection
    if(argv[optind] == NULL) //there is no nonoption argument (port number)
    {
        fprintf(stderr, "Error, no port number found in command line\n");
        exit(1);
    }
    int portno = atoi(argv[optind]); //first nonoption
    struct sockaddr_in serv_addr;
    struct hostent *server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fprintf(stderr, "ERROR opening socket\n");
        exit(2);
    }
    server = gethostbyname(get_host_name);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(2);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        fprintf(stderr, "ERROR during socket connection step\n");
        exit(2);
    }

    //SSL Setup
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    //SSL Initialization
    SSL_CTX *newContext = SSL_CTX_new(TLSv1_client_method()); 
    if (newContext == NULL) 
    {
        fprintf(stderr, "Unable to get SSL context\n");
        exit(2);
    }
    sslClient = SSL_new(newContext); 
    if (sslClient == NULL) 
    {
        fprintf(stderr, "Unable to complete SSL initialization\n");
        exit(2);
    }

    //SSL Connection
    SSL_set_fd(sslClient, sockfd);
    SSL_connect(sslClient);

    //send (and log) an ID terminated with a newline
    char id_buf[20];
    sprintf(id_buf, "ID=%s\n", get_id_num);
    SSL_write(sslClient, id_buf, strlen(id_buf));
    if(log_flag == 1)
    {
        dprintf(log_fd, "ID=%s\n", get_id_num);
    }

    struct pollfd fd[1];
    fd[0].fd = sockfd; //the one describes socket
    fd[0].events = POLLIN|POLLHUP|POLLERR;

    time_t curtime;
    struct tm *loc_time;
    while(1)
    {
        get_temperature();

        curtime = time(NULL);
        loc_time = localtime(&curtime);
        strftime(time_storage, 9, "%I:%M:%S", loc_time); //get current time & space
        
        char time_buf[20];
        if(stop_flag == 0)
        {
            sprintf(time_buf, "%s %0.1f\n", time_storage, temperature); //precision: 1 digit after dot
            SSL_write(sslClient, time_buf, strlen(time_buf));
            if(log_flag == 1)
            {
                dprintf(log_fd, "%s %0.1f\n", time_storage, temperature);
            }
        }

        char buf_cmd_line[20];
        int val_returned = poll(fd, 1, 0);
        if(val_returned < 0)
        {
            fprintf(stderr, "Error running poll system call: %s\n", strerror(errno));
            exit(1);
        }
        if(val_returned > 0)
        {
            int ret = SSL_read(sslClient, buf_cmd_line, 20);
            if(ret > 0)
            {
                buf_cmd_line[ret] = 0; //remove last bit's "next line"
            }
            char *temp = buf_cmd_line;
            char *last_bit = &buf_cmd_line[ret];
            while(temp < last_bit)
            {
                char* ptr = temp;
                while (ptr < last_bit && *ptr != '\n') //check "next line" in between
                {
                    ptr++;
                }
                *ptr = 0; //remove "next line"
                cmd_line_handler(temp);
                temp = &ptr[1];
            }
        }

        sleep(sample_rate);
    }

    int SSL_shutdown(SSL *sslClient);
    void SSL_free(SSL *sslClient);

    mraa_aio_close(temperature_pin);
    return 0;
}
