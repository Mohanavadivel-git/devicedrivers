#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
//#include <unistd.h>

#define DEVICE "/dev/MYTESTDEVICE"

int main(){
	int i,fd;
	char ch, write_buf[100], read_buf[100];

	fd = open(DEVICE, 0, O_RDWR); //open for reading and writing

	if(fd == -1){
		printf("file %s either doesnot exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}

	printf("r = read from device\nw = write to device\nenter command: ");
	scanf("%c", &ch);

	switch (ch){
		case 'w':
			printf("enter data: ");
			scanf("%[^\n]", write_buf);
			printf("input value: %s and the size: %ld\n", write_buf, sizeof(write_buf));
			ssize_t ret = write(fd, "test", sizeof(write_buf));
			printf("return value of write :%zd\n", ret);			
			if(ret < 0){
				printf("something went wrong during the write\n");
				//printf(strerror(errno));
			}
			//printf("write result: %ld", write(fd, write_buf, sizeof(write_buf)));
			break;
		case 'r':
			read(fd, read_buf, sizeof(read_buf));
			printf("device: %s\n", read_buf);
			break;
		default:
			printf("command not recognized\n");
			break;
	}

	close(fd);

	return 0;
}
