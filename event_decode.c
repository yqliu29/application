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

#define DEVICE_NAME	"/dev/input/event1"

static char *decode_type(int type)
{
	if (type == 0)
		return "SYNC";
	else if (type == 1)
		return "KEY ";
	else if (type == 3)
		return "ABS ";
	else
		return "UKNW";
}

static char *decode_code(int code)
{
	if (code == 0)
		return "X    ";
	else if (code == 1)
		return "Y    ";
	else if (code == 24)
		return "PRESS";
	else if (code == 330)
		return "TOUCH";
	else
		return "UNKNW";
}

int main(int argc, char **argv)
{
	char name[30] = DEVICE_NAME;
	int fd, ret;
	struct input_event event;

	if (argc >= 2)
		sprintf(name, "%s", argv[1]);

	fd = open(name, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "Can not open %s\n", name);
		return -1;
	}

	while (1)
	{
		ret = read(fd, &event, sizeof (event));
		if (ret < 0)
		{
			fprintf(stderr, "read file %s error: %d\n", name, ret);
			continue;
		}

		if (event.type == 0)
		{
			fprintf(stderr, "%08d.%06d ----------------\n", event.time.tv_sec, event.time.tv_usec);
		}
		else
		{
			fprintf(stderr, "%08d.%06d %s %s %04d\n", event.time.tv_sec, event.time.tv_usec, 
					decode_type(event.type), decode_code(event.code), event.value);
		}
	}

	return 0;
}

