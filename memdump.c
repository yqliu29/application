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
#include <sys/mman.h>

int main(int argc, char **argv)
{
#define REGISTER_ADDR	0x10120010
#define REGISTER_VALUE	0x07200000
	int fd;
	unsigned int value;
	void *map_base, *virt_addr;
	int page_size;

	fd = open("/dev/mem", O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "%s: can not open file /dev/mem\n", __func__);
		return -1;
	}

	page_size = getpagesize();
	map_base = mmap(NULL, page_size, PROT_READ|PROT_WRITE, MAP_SHARED, 
			fd, (off_t)(REGISTER_ADDR & (~(page_size-1))));
	if (map_base == MAP_FAILED)
	{
		fprintf(stderr, "%s: can not mmap file /dev/mem\n", __func__);
		close(fd);
		return -1;
	}

	virt_addr = (char *)map_base + (REGISTER_ADDR & (page_size - 1));
	value = *(volatile unsigned int*)virt_addr;

	fprintf(stderr, "0x%08x\n", value);

	if (value != REGISTER_VALUE)
		*(volatile unsigned int*)virt_addr = REGISTER_VALUE;

	value = *(volatile unsigned int*)virt_addr;

	fprintf(stderr, "0x%08x\n", value);

	munmap(map_base, page_size);
	close(fd);

	return 0;
}

