#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

void generate_random_string(int size, char *s) {
	char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int i;
	for (i=0; i<size; i++) {
		s[i] = charset[rand()%(sizeof(charset)-1)];
	}
	s[size]=0;
}

int main(int argc, char **argv){
	
	if(argc<3){
		printf("Error: argument should be of type <filename> <no. of producers> <no. of strings per producer>\n");
		return 0;	
	}
	int argv1 = atoi(argv[1]);
	int argv2 = atoi(argv[2]);
	int pid, status;
	size_t size_of_string = 10, n;
	int fd = open("/dev/mypipe", O_WRONLY);
	if(fd == -1){
		printf("Error in opening /dev/mypipe.. %s\n",strerror(errno));
		return 0;
	}

	char string[500];
	int iterations = argv1, i=1;
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
			srand(time(NULL) ^ (getpid()<<16));
			for(j=1; j<=argv2; j++){
				sleep(1);
				generate_random_string(size_of_string, string);
				n = write(fd, string, size_of_string);
				if(n<0){
					printf("Error in wrtting to device buffer\n");
				}
				else{
					printf("The Producer %d generated the string %d: %s  (BUFFER FILLED:%d)\n",i,j,string,n);
				}
			}
			exit(0);
		}
		else{
			wait(NULL);
		}
		iterations--;
		i++;
	}	
	close(fd);
	return 0;
}
