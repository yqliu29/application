#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/input.h>
#include <pthread.h>

#define DEVICE_NAME	"/dev/knob"
#define THREAD_NUMS	32

void *child_thread(void *data)
{
	int fd;
	fd_set set;
	int ret;
	char value = 0;

	fd = open(DEVICE_NAME, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Failed to open device %s in thread %d\n", DEVICE_NAME, *(int *)data);	
		return NULL;
	}

	while (1)
	{
		FD_ZERO(&set);
		FD_SET(fd, &set);

		ret = select(fd+1,&set,NULL,NULL,NULL);
		if((ret > 0) && (FD_ISSET(fd, &set)))
		{
			ret = read(fd, &value, sizeof(value));
			fprintf(stderr, "Thread %d read %d\n", *(int *)data, value);
		}
		else
		{
			fprintf(stderr, "Thread %d select error!\n", *(int *)data);
		}
	}

	return NULL;
}

int main(int argc, char **argv)
{
	int i;
	pthread_t threads[THREAD_NUMS];
	int param[THREAD_NUMS];

	for (i = 0; i < THREAD_NUMS; i++)
	{
		param[i] = i + 1;
		
		if (pthread_create(&threads[i], NULL, child_thread, &param[i]))
		{
			fprintf(stderr, "Create thread %d error!\n", i+1);
			return 1;
		}

		fprintf(stderr, "Create thread %d ok!\n", i+1);
	}

	for (i = 0; i < THREAD_NUMS; i++)
	{
		if (pthread_join(threads[i], NULL))
		{
			fprintf(stderr, "Join thread %d error!\n", i+1);
			return 2;
		}
	}

	return 0;
}
