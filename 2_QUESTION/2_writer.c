#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdlib.h>
#include <errno.h>
#include <unistd.h>

int main(int argc,const char *argv[])
{
	int fd;
	char U_buff[50]="THIS IS FROM USER SPACE BY WRITER 2..\n";
	ssize_t writing;

	fd = open("/dev/SYNCDRIV_2", O_RDWR, 0777);
	if(fd < 0)
	{
		printf("ERROR IN OPENING DEVICE DEVICE 2..\n");
		exit(1);
	}

	writing = write(fd, U_buff, sizeof(U_buff));
	if (writing < 0 )
	{
		perror("Error in writing Device 2\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		printf("WRITER_2 WRITING FROM USER TO KERNEL DEVICE 2 = %s\n", U_buff);
		printf("\nByte written successfully = %ld\n", writing);
	}
	close(fd);
}
