#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    int fd;
    char K_buff[50];
    ssize_t reading;
    
    fd=open("/dev/SYNCDRIV",O_RDWR,0777);

    if(fd<0)
    {
        printf("ERROR IN OPENING DEVICE 1..\n");
        exit(1);
    }
    reading = read(fd, K_buff, sizeof(K_buff));

    if(reading < 0)
    {
	   perror("Error in reading device 1\n");
	   exit(EXIT_FAILURE);
    }
    printf("\nREADER_1 READING FROM KERNEL DEVICE 1 = %s\n", K_buff);
    printf("Byte written successfully = %ld\n", reading);
    close(fd);
    return 0;
}
