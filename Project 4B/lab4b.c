/*
NAME: Juan Bai
EMAIL: daisybai66@yahoo.com
ID: 105364754
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
//#include <mraa.h>
//#include <mraa/aio.h>
#include <sys/errno.h>
#include <string.h>

#ifdef DUMMY
	#define	MRAA_GPIO_IN 0
	#define MRAA_GPIO_EDGE_RISING 1
	typedef int mraa_aio_context;
	typedef int mraa_gpio_context;
	
	mraa_aio_context mraa_aio_init()
	{
	  return 1;
	}

	int mraa_aio_read() 
	{
	  return 650;
	}
	void mraa_aio_close()
	{
	}
	mraa_gpio_context mraa_gpio_init()
	{
	  return 1;
	}
	
	void mraa_gpio_dir()
	{
	}
	void mraa_gpio_isr()
	{
	}
	void mraa_gpio_close()
	{
       	}
#else
	#include <mraa.h>
#endif

char scale;
FILE *file_ptr = NULL;
mraa_gpio_context button;
mraa_aio_context sensor;
#define A0 0
#define GPIO_115 73

#define B 4275
#define R0 100000.0
//read and convert temperature
float convert_temperature_reading(int reading)
{	
	float R = 1023.0/((float) reading) - 1.0;
	R = R0*R;
	//C is the temperature in Celcious
	float C = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
	if(scale == 'F') //F is the temperature in Fahrenheit
		return ((C * 9)/5 + 32);
	else if(scale=='C')
		return C;
	return 0;
}

void print_current_time()
{
	struct timespec ts;
	struct tm * tm;
	clock_gettime(CLOCK_REALTIME, &ts);
	tm = localtime(&(ts.tv_sec));
	if(file_ptr != NULL)
		fprintf(file_ptr,"%d:%d:%d ", tm->tm_hour, tm->tm_min, tm->tm_sec);
	printf("%d:%d:%d ",  tm->tm_hour, tm->tm_min, tm->tm_sec);

}

void shutdown() 
{
  	#ifdef DUMMY
	mraa_gpio_close();
	mraa_aio_close();
  	#else
	mraa_gpio_close(button);
	mraa_aio_close(sensor);
	#endif
	print_current_time();
	printf("SHUTDOWN\n");
	if(file_ptr != NULL)
		fprintf(file_ptr,"SHUTDOWN\n");
	exit(0);
}

int period = 1;
int main(int argc, char * argv[]) 
{
	//period, scale, log = process_cmd_line_arg(argc, argv);
	struct option options[] = {
		{"period", 1, NULL, 'p'},
   		{"scale", 1, NULL, 's'},
		{"log", 1, NULL, 'l'},
		{0, 0, 0, 0}
	};

	int opt;
	while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1){
		switch (opt) {
			case 'p': 
				period = atoi(optarg);
				break;
			case 's':
				if (optarg[0] == 'F' || optarg[0] == 'C') 
				{
					scale = optarg[0];
				}
				break;
			case 'l':
				file_ptr = fopen(optarg, "w+");	
            	if(file_ptr == NULL) 
				{
            		fprintf(stderr, "Log file invalid\n");
					exit(1);
				}
				break;
		        default:
				printf("Correct usage: ./lab4b --period=[# command line parameter] --scale=C --scale=F --log=filename\n");
				exit(1);
				break;
		}
	}
	
	//Initialize the Analog Sensor
	#ifdef DUMMY
	mraa_aio_init();
	#else
	if((sensor = mraa_aio_init(A0)) == NULL)
	{
		fprintf(stderr, "Error come from analogy devices:sensor\n");
		exit(1);
	}
	#endif
	
	#ifdef DUMMY
	int ret_temp =mraa_aio_read();
	#else
	int ret_temp = mraa_aio_read(sensor);
	#endif
	print_current_time();
	if(file_ptr != NULL)
		fprintf(file_ptr,"%0.1f \n",convert_temperature_reading(ret_temp));
	printf("%0.1f \n",convert_temperature_reading(ret_temp));
	
	//Initialize GPIO for Button
        #ifdef DUMMY
        mraa_gpio_init();
	#else
	if((button = mraa_gpio_init(GPIO_115))== NULL)
	{
		fprintf(stderr, "Error come from analogy devices:button\n");
		exit(1);
	}
	#endif
	
	#ifdef DUMMY
	mraa_gpio_dir();
	mraa_gpio_isr();
	#else
	//Configure the button interface to be an input pin 
	mraa_gpio_dir(button, MRAA_GPIO_IN);
	//Setup ISR for button rising edge - Register ISR
	mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, shutdown, NULL);
	#endif
	struct pollfd pollfds;
    	pollfds.fd = 0;
    	pollfds.events = POLLIN + POLLHUP + POLLERR;
	
	/*
	char *input;
	input = (char *)malloc(1024 * sizeof(char));
	*/
	struct timespec start_time;
	struct timespec current_time;
	//Initialize times
	clock_gettime(CLOCK_REALTIME, &current_time);
	start_time = current_time;
	int report = 1;

	 while(1)
	 {
		if(poll(&pollfds,1,0)<0)//poll to see if there are inputs from stdin
		{
			fprintf(stderr,"Error from poll: %s\n",strerror(errno));
			exit(1);
		}
		else if(pollfds.revents & POLLIN)
		{   
			//read from stdin till encountering ‘\n’ 
			//(thus we get an command) process the command.
			char input[1024];
			memset(input,0,strlen(input));
			int readBytes = read(0,&input,sizeof(input));
			if (readBytes < 0)
			{
			  fprintf(stderr,"Error to read from stdin to input buffer: %s\n",strerror(errno));
			  exit(1);
			}

			if(strstr(input, "SCALE=F") != NULL) 
			{
				printf("SCALE=F\n");
				if(file_ptr != NULL)
					fprintf(file_ptr,"SCALE=F\n");
				scale = 'F'; 
			}							
			if(strstr(input, "SCALE=C") != NULL) 
			{
				printf("SCALE=C\n");
				if(file_ptr != NULL)
					fprintf(file_ptr,"SCALE=C\n");
				scale = 'C'; 
			} 
			if(strstr(input, "PERIOD=") != NULL)
			{	
				int digit_index=8;
				int digit_count=0;
				char digit[digit_index];
				char* ret_period=strstr(input, "PERIOD=");
				ret_period+=7;
				while(*ret_period != '\n')
				{
					digit[digit_count]= *ret_period;
					ret_period++;
					digit_count++;
				}
				period=atoi(digit);
				printf("PERIOD=%s\n",digit);
				if(file_ptr != NULL)
					fprintf(file_ptr,"PERIOD=%s\n",digit);
			}
			if(strstr(input, "STOP") != NULL) 
			{
				printf("STOP\n");
				if(file_ptr != NULL)
					fprintf(file_ptr,"STOP\n");
				report = 0;
			} 
			if(strstr(input, "START") != NULL) 
			{
				printf("START\n");
				if(file_ptr != NULL)
					fprintf(file_ptr,"START\n");
				report = 1;
			} 
			if(strstr(input, "LOG") != NULL) 
			{
				char* ret_log = strstr(input, "LOG");
				printf("LOG ");
				if(file_ptr != NULL)
					fprintf(file_ptr,"LOG ");
				ret_log+=4;
				while(*ret_log != '\n')
				{
				printf("%c",*ret_log);
				if(file_ptr != NULL)
					fprintf(file_ptr, "%c", *ret_log);
				ret_log++;
				}
				printf("\n");
				if(file_ptr != NULL)
				fprintf(file_ptr,"\n");
			} 
			if(strstr(input, "OFF") != NULL) 
			{
				printf("OFF\n");
				if(file_ptr != NULL)
					fprintf(file_ptr,"OFF\n");
				shutdown();
			} 

		}
		
		if(report && current_time.tv_sec >= start_time.tv_sec + period)
		{
		  	#ifdef DUMMY
		        ret_temp= mraa_aio_read();
		  	#else
			ret_temp= mraa_aio_read(sensor);//read from temperature sensor, convert and report  
			#endif
			print_current_time();
			if(file_ptr != NULL)
				fprintf(file_ptr,"%0.1f \n",convert_temperature_reading(ret_temp));
			printf("%0.1f \n",convert_temperature_reading(ret_temp));
			start_time = current_time;
		}
		clock_gettime(CLOCK_REALTIME, &current_time);
	 }
	#ifdef DUMMY
	mraa_gpio_close();
	mraa_aio_close();
	#else
	mraa_gpio_close(button);
	mraa_aio_close(sensor);
	#endif
	return 0;
}
