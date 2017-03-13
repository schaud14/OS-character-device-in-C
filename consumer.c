#include <stdio.h>
#include <errno.h>
#include <string.h>
#include<stdlib.h>
#include<fcntl.h>

int main(int argc, char **argv){
	if(argc<2){
		printf("Error: argument should be of type <filename> <no. of consumers> <no. of strings consumed per consumer>\n");
		return 0;	
	}
	int argv1 = atoi(argv[1]);
	int argv2 = atoi(argv[2]);
	int pid,status;
	size_t size_of_string = 10,n;
	int fd = open("/dev/mypipe", O_RDONLY);
	if(fd == -1){
		printf("Error in opening /dev/mypipe.. %s\n",strerror(errno));
		return 0;
	}
	
	char string[500];
	memset(string,0,500);
	int iterations=argv1;
	while(iterations){
		pid=fork();
		if( pid < 0 )
		{
			printf( "Error in child process fork\n" );
			exit(1);
		}
		else if( pid == 0 )
		{
			int j;
			for(j=1; j<=argv2; j++){
				sleep(1);
				n = read(fd, string, size_of_string);
				if(n<0){
					printf("Error in reading from device buffer");
					exit(1);
				}
				printf("The consumer consumed the string %d: %s  (BUFFER FILLED:%d)\n",j,string,n);
			}
			exit(0);
		}
		else{
			wait(NULL);
		}
		iterations--;
	}	
	close(fd);
	return 0;
}
