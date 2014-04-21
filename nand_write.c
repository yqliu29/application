#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <inttypes.h>

typedef unsigned char __u8;
typedef unsigned int __u32;

struct erase_info_user {
	__u32 start;
	__u32 length;
};

#define BLOCK_SIZE			0x40000
#define BLOCK_ROUND_UP(x)	(((x) + BLOCK_SIZE - 1)& (~(BLOCK_SIZE - 1)))

#define MEMERASE			_IOW('M', 2, struct erase_info_user)
#define MEMGETBADBLOCK      _IOW('M', 11, long long)
#define MEMSETBADBLOCK      _IOW('M', 12, long long)

char buffer[BLOCK_SIZE];

int main(int argc, char **argv)
{
	int fd;
	int ret;
	int len, i, j, offset;
	loff_t seek;
	struct timeval tv1, tv2;
	struct erase_info_user ei;

	if (argc < 4)
	{
		fprintf(stderr, "Usage: nand_write <dev> <offset> <size>\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "Can not open %s\n", argv[1]);
		return -1;
	}

	offset = strtoul(argv[2], NULL, 16);
	len = strtoul(argv[3], NULL, 16);

	len = BLOCK_ROUND_UP(len);
	i = 0;
	j = offset;
	while (i < len)
	{
		seek = (loff_t)j;
		ret = ioctl(fd, MEMGETBADBLOCK, &seek);
		if (ret == 1)
		{
			j += BLOCK_SIZE;
			continue;
		}

		ei.start = (unsigned int)(j);
		ei.length = BLOCK_SIZE;	
		j += BLOCK_SIZE;

		if (-1 == ioctl(fd, MEMERASE, &ei))
			continue;

		i += BLOCK_SIZE;
	}

	memset(buffer, 0xaa, sizeof (buffer));
	i = 0;
	j = offset;

	gettimeofday(&tv1, NULL);
	while (i < len)
	{
		seek = (loff_t)j;
		ret = ioctl(fd, MEMGETBADBLOCK, &seek);
		if (ret == 1)
		{
			j += BLOCK_SIZE;
			continue;
		}

		seek = (loff_t)j;
		lseek(fd, seek, SEEK_SET);
		ret = write(fd, buffer, BLOCK_SIZE);

		i += BLOCK_SIZE;
	}
	gettimeofday(&tv2, NULL);
	close(fd);



	i = tv2.tv_usec - tv1.tv_usec;
	if (i < 0)
	{
		j = tv2.tv_sec - tv1.tv_sec - 1;
		i = 1000000 + tv2.tv_usec - tv1.tv_usec;
	}
	else
	{
		j = tv2.tv_sec - tv1.tv_sec;
	}

	ret = len/(j*1000 + i/1000);
	ret = ret*1000;

	fprintf(stderr, "write %08x bytes, times used %d.%06d, speed %d.%dMB/s\n", len, j, i, ret/0x100000,
		  																		(ret%0x100000)*10/0x100000);

	return 0;
}

