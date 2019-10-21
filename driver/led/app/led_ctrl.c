/*************************************************************************
  @Author: Jason
  @Created Time : Sun 20 Oct 2019 09:15:34 PM CST
  @File Name: led_ctrl.c
  @Description:
 ************************************************************************/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h> 
/*
  *  ledtest <dev> <on|off>
  */

void print_usage(char *file)
{
    printf("Usage:\n");
    printf("%s <dev> <on|off>\n",file);
    printf("eg. \n");
    printf("%s /dev/led1 on\n", file);
    printf("%s /dev/led1 off\n", file);
}

int main(int argc, char **argv)
{
	int fd;
	int val = 1;
	char *filename;	

	

	filename = argv[1];
	fd = open(filename, O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		printf("strerror:%s\n", strerror(errno));  
	}
	if (argc != 3)
	{
		print_usage(argv[0]);
		return 0;
	}
	if (strcmp(argv[2], "on") == 0)
	{
		val  = 1;
	}
	else
	{
		val = 0;
	}

	write(fd, &val, 1);
	return 0;
}

