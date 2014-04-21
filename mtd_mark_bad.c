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

#define MEMSETBADBLOCK      _IOW('M', 12, long long)

int main(int argc, char **argv)
{
	int fd;
	loff_t seek;

	if (argc < 3)
	{
		printf("Usage: mtd_mark_bad <mtd_dev> <offset>\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		printf("Can not open %s\n", argv[1]);
		return -1;
	}

	seek = (loff_t)strtol(argv[2], NULL, 16);
	if (-1 == ioctl(fd, MEMSETBADBLOCK, &seek))
		printf("Can not mark block start at 0x%08x as bad!\n", (unsigned int)seek);
	else
		printf("Mark block start at 0x%08x as bad ok!\n", (unsigned int)seek);

	return 0;
}

