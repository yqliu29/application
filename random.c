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
#include <string.h>

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int 		uint32_t;

#define DEVICE	"/dev/urandom"
#define NUMBER	10
#define MAX_NUM	0x20000
#define MIN_NUM	100

int main(int argc, char **argv)
{
	int fd, i, ret;
	uint32_t random[NUMBER];
	int times = 0;

	fd = open(DEVICE, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Open file %s failed: %s(%d)!\n", DEVICE, strerror(errno), errno);
		return fd;
	}

	while (1)
	{
		for (i = 0; i < NUMBER; i++)
		{
			ret = read(fd, &random[i], sizeof (uint32_t));
			if (ret < 0)
			{
				fprintf(stderr, "Read file %s error: %s(%d)!\n", DEVICE, strerror(errno), errno);
				close(fd);
				return ret;
			}

			if (random[i] > MAX_NUM)
				random[i] = random[i]%MAX_NUM;
			if (random[i] < MIN_NUM)
				random[i] += MIN_NUM;
		}

		fprintf(stderr, "%04d: ", ++times);
		for (i = 0; i < NUMBER; i++)
			fprintf(stderr, "%05x ", random[i]);
		fprintf(stderr, "\n");

		sleep(1);
	}

	return 0;
}

