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

typedef unsigned char 			__u8;
typedef unsigned short 			__u16;
typedef unsigned int 			__u32;
typedef unsigned long long 		__u64;
struct mtd_info_user {
	__u8 type;
	__u32 flags;
	__u32 size; /* Total size of the MTD */
	__u32 erasesize;
	__u32 writesize;
	__u32 oobsize;  /* Amount of OOB data per block (e.g. 16) */
	__u64 padding;  /* Old obsolete field; do not use */
};

#define SZ_1MB	0x100000
#define SZ_1KB	0x400

#define MEMGETINFO      	_IOR('M', 1, struct mtd_info_user)
#define MEMGETBADBLOCK      _IOW('M', 11, long long)

int main(int argc, char **argv)
{
	int fd;
	int ret;
	loff_t seek;
	struct mtd_info_user mtd_info;
	int badblocks = 0;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: nand_scan <mtd_dev>\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "Can not open %s\n", argv[1]);
		return -1;
	}

	if ((ioctl(fd, MEMGETINFO, &mtd_info)) < 0)
	{
		fprintf(stderr, "Can not get device info.\n");
		close(fd);
		return -1;
	}	

	fprintf(stderr, "devide size:    %08x(%dMiB)\n", mtd_info.size, mtd_info.size/SZ_1MB);
	fprintf(stderr, "block size:     %08x(%dKiB)\n", mtd_info.erasesize, mtd_info.erasesize/SZ_1KB);
	fprintf(stderr, "page size:      %08x(%dKiB)\n", mtd_info.writesize, mtd_info.writesize/SZ_1KB);
	fprintf(stderr, "oob size:       %08x(%dBytes)\n", mtd_info.oobsize, mtd_info.oobsize);
	fprintf(stderr, "total blocks:   %d\n", mtd_info.size/mtd_info.erasesize);

	for (seek = 0; seek < mtd_info.size; seek += mtd_info.erasesize)
	{
		ret = ioctl(fd, MEMGETBADBLOCK, &seek);
		if (ret < 0)
			fprintf(stderr, "Scan error at offset %08x\n", seek);
		else if (ret == 1)
		{
			int temp = seek;
			fprintf(stderr, "Bad block found at offset %08x(block %d)\n", temp, temp/mtd_info.erasesize);
			badblocks++;
		}
	}

	close(fd);

	fprintf(stderr, "bad blocks:     %d\n", badblocks);

	return 0;
}

