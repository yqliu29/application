#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <linux/limits.h>
#include <sys/times.h>

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

#define SPACE_CHAR	' '

#define COMM_NAME_POS	2
#define UTIME_POS		14
#define STIME_POS		15
#define CHILD_UTIME_POS	16
#define CHILD_STIME_POS	17

const char *show_message[5] = 
{
	"command          ",
	"usr time         ",
	"kernel time      ",
	"child user time  ",
	"child kernel time"
};

char buffer[200];

int process_stat(char *buf, int len)
{
	int i,j,show_index;
	int index = 1;
	char temp[30];

	while (i < len)
	{
		if (buf[i] == SPACE_CHAR)
		{
			index++;
			if (index == COMM_NAME_POS)
				show_index = 0;
			else if (index == UTIME_POS)
				show_index = 1;
			else if (index == STIME_POS)
				show_index = 2;
			else if (index == CHILD_UTIME_POS)
				show_index = 3;
			else if (index == CHILD_STIME_POS)
				show_index = 4;
			else
				show_index = -1;

			if (show_index >= 0)
			{
				memset((void*)temp, 0, sizeof(temp));
				j = 0;
				while (i < len)
				{
					if (buf[++i] == SPACE_CHAR)
						break;
					else
						temp[j++] = buf[i];
				}

				printf("%s: %s\n", show_message[show_index], temp);

				if (show_index >= 4)
					break;

				continue;
			}
			else
			{
				while (i < len)
					if (buf[++i] == SPACE_CHAR)
						break;
			}
		}
		else
		{
			while (i < len)
				if (buf[++i] == SPACE_CHAR)
					break;
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	int fd;
	int ret;
	char filename[20];

	sprintf(filename, "/proc/%s/stat", argv[1]);
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		printf("Can not open file %s\n", filename);
		return -1;
	}

	ret = read(fd, buffer, sizeof(buffer));
	if (ret < 0)
	{
		printf("Read file %s error!\n", filename);
		goto main_out;
	}	

	ret = process_stat(buffer, ret);
main_out:
	close(fd);
	return ret;
}

