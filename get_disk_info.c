#include <sys/statvfs.h>
#include <stdio.h>

typedef unsigned int uint32_t;
#define PATH_TO_CHECK	"/data"

#define SIZE_1M			0x100000

/******************************************************** 
 * Name
 * 	get_disk_info
 * Return 
 * 	-1 if failed to get infomation
 * 	else return 0
 * Params:
 * 	capacity		The total bytes of this disk.
 * 	free			Free bytes of this disk.
 *******************************************************/ 
int get_disk_info(uint32_t *capacity, uint32_t *free)
{
	struct statvfs vfs;

	if (statvfs(PATH_TO_CHECK, &vfs))
	{
		fprintf(stderr, "Get disk information error for %s\n", PATH_TO_CHECK);
		return -1;
	}

	*capacity = (uint32_t)vfs.f_bsize * vfs.f_blocks;
	*free = (uint32_t)vfs.f_bsize * vfs.f_bfree;

	return 0;
}

/******************************************************** 
 * Name
 * 	get_disk_free_space
 * Return 
 * 	-1 if failed to get infomation
 * 	else return 0
 * Params:
 * 	free			Free bytes of this disk.
 *******************************************************/ 
int get_disk_free_space(uint32_t *free)
{
	struct statvfs vfs;

	if (statvfs(PATH_TO_CHECK, &vfs))
	{
		fprintf(stderr, "Get disk information error for %s\n", PATH_TO_CHECK);
		return -1;
	}

	*free = (uint32_t)vfs.f_bsize * vfs.f_bfree;

	return 0;
}

/******************************************************** 
 * Name
 * 	get_disk_capacity
 * Return 
 * 	-1 if failed to get infomation
 * 	else return 0
 * Params:
 * 	capacity		The total bytes of this disk.
 *******************************************************/ 
int get_disk_capacity(uint32_t *capacity)
{
	struct statvfs vfs;

	if (statvfs(PATH_TO_CHECK, &vfs))
	{
		fprintf(stderr, "Get disk information error for %s\n", PATH_TO_CHECK);
		return -1;
	}

	*capacity = (uint32_t)vfs.f_bsize * vfs.f_blocks;

	return 0;
}

int main(int argc, char **argv)
{
	uint32_t capacity, free;
	
	if (get_disk_info(&capacity, &free))
		return -1;

	fprintf(stderr, "Capacity: %08x(%d.%dMiB)\n", capacity, capacity/SIZE_1M, (capacity%SIZE_1M)*10/SIZE_1M);
	fprintf(stderr, "Free:     %08x(%d.%dMiB)\n", free, free/SIZE_1M, (free%SIZE_1M)*10/SIZE_1M);

	if (get_disk_capacity(&capacity))
		return -1;

	if (get_disk_free_space(&free))
		return -1;

	fprintf(stderr, "Capacity: %08x(%d.%dMiB)\n", capacity, capacity/SIZE_1M, (capacity%SIZE_1M)*10/SIZE_1M);
	fprintf(stderr, "Free:     %08x(%d.%dMiB)\n", free, free/SIZE_1M, (free%SIZE_1M)*10/SIZE_1M);

	return 0;
}

