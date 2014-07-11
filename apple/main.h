#ifndef __MAIN_H__
#define __MAIN_H__

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/input.h>
#include <linux/hid.h>
#include <linux/hiddev.h>
#include <linux/usbdevice_fs.h>
#include <linux/usb/ch9.h>

#define DESCRIPTOR_DEBUG
#define COMMAND_SEND_DEBUG
#define INFO(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define ERROR(fmt, args...)			fprintf(stderr, fmt, ##args)

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;

#endif

