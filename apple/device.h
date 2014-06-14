#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "main.h"

#define VENDOR_ID_APPLE				0x05AC
#define USB_CONTROL_MSG_TIMEOUT		50000		/* ms */
#define MAX_PACKET_SIZE				64

/******************************************************** 
 * data structures
 *******************************************************/ 
typedef struct {
	int								fd;
	uint16_t						send_tid;
	uint16_t						recv_tid;
	struct usb_device_descriptor	dev_desc;
	struct usb_config_descriptor	cfg_desc;
	struct usb_interface_descriptor	inf_desc;
	struct usb_endpoint_descriptor	hidep;
} ipod_device_t;

/******************************************************** 
 * functions proto type
 *******************************************************/ 
int bus_scan(char *bus, ipod_device_t *dev);
int ipod_connect_device(ipod_device_t *dev);
int ipod_send_command(ipod_device_t *dev, uint8_t *cmd, int size);
int ipod_receive_command(ipod_device_t *dev, uint8_t *cmd, int timeout);


#endif

