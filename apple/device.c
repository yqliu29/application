#include "main.h"
#include "device.h"
#include "protocol.h"
#include "hid.h"

/******************************************************** 
 * static data
 *******************************************************/ 
static hid_report_t hidrpt;

/******************************************************** 
 * show command received
 *******************************************************/ 
static void debug_command_receive(uint8_t *cmd, int size)
{
#ifdef COMMAND_SEND_DEBUG
	INFO("Command Received %d bytes#\n", size);
	while (size--)
		INFO("%02x ", *cmd++);
	INFO("\n");
#endif
}

/******************************************************** 
 * show command send
 *******************************************************/ 
static void debug_command_send(uint8_t *cmd, int size)
{
#ifdef COMMAND_SEND_DEBUG
	INFO("Command Send %d bytes#\n", size);
	while (size--)
		INFO("%02x ", *cmd++);
	INFO("\n");
#endif
}

/******************************************************** 
 * send packet to ipod control endpoint
 *******************************************************/ 
int ipod_send_command(ipod_device_t *dev, uint8_t *cmd, int size)
{
	static uint8_t tempbuf[MAX_PACKET_SIZE];
	struct usbdevfs_ctrltransfer msg;
	int ret, i = 0;
	int packets, temp_size;

	temp_size = hid_report_max_out_size(&hidrpt, MAX_PACKET_SIZE);
	packets = (size + temp_size - 3)/(temp_size - 2);

	while (packets > 0)
	{
		msg.bRequestType        = HID_DT_HID;
		msg.bRequest            = HID_REQ_SET_REPORT;
		msg.wValue              = (HID_REPORT_TYPE_OUTPUT << 8) | IPOD_REPORT_ID;
		msg.wIndex              = IPOD_INTERFACE_NUM;
		msg.wLength             = hid_report_get_size(&hidrpt, size - i + 2);
		msg.data                = tempbuf;
		msg.timeout             = USB_CONTROL_MSG_TIMEOUT;

		memset(tempbuf, 0, msg.wLength);
		tempbuf[0] = hid_report_get_id(&hidrpt, msg.wLength);
		if (packets > 1)
			tempbuf[1] |= 0x02;		/* more packets to be flowed */
		if (i > 0)
			tempbuf[1] |= 0x01;		/* is not the first packet */

		if (size - i >= msg.wLength - 2)
			temp_size = msg.wLength - 2;
		else
			temp_size = size - i;

		memcpy(&tempbuf[2], cmd + i, temp_size);
		debug_command_send(tempbuf, msg.wLength);

		i += temp_size;
		packets--;

		ret = ioctl(dev->fd, USBDEVFS_CONTROL, &msg);
		if (ret < 0)
		{
			ERROR("Failed to send USB control packets: %d\n", errno);
			return ret;
		}
	}

	return 0;
}

/******************************************************** 
 * receive packet from ipod hid endpoint
 *******************************************************/ 
int ipod_receive_command(ipod_device_t *dev, uint8_t *cmd, int timeout)
{
	struct usbdevfs_urb urb;
	struct usbdevfs_urb *context;
	struct timeval tv, tvref;
	fd_set fds;
	int ret;
	int waitting = 1;

	/* calculate timeout */
	gettimeofday(&tv, NULL);
	tvref.tv_sec = (timeout * 1000)/1000000;
	tvref.tv_usec = (timeout * 1000)%1000000;
	timeradd(&tv, &tvref, &tvref);

	/* fill URB */
	urb.type              = USBDEVFS_URB_TYPE_INTERRUPT;
	urb.endpoint          = dev->hidep.bEndpointAddress;
	urb.flags             = 0;
	urb.buffer            = cmd;
	urb.buffer_length     = COMMAND_BUFFER_SIZE;
	urb.signr             = 0;
	urb.actual_length     = 0;
	urb.number_of_packets = 0;
	urb.usercontext       = NULL;

	ret = ioctl(dev->fd, USBDEVFS_SUBMITURB, &urb);
	if (ret < 0)
	{
		ERROR("Failed to submit URB: %d\n", errno);
		return ret;
	}

	FD_ZERO(&fds);
	FD_SET(dev->fd, &fds);
	while (waitting)
	{
		ret = ioctl(dev->fd, USBDEVFS_REAPURBNDELAY, &context);
		if (ret == 0)
		{
			debug_command_receive(cmd, urb.actual_length);
			return urb.actual_length;
		}

		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		ret = select(dev->fd + 1, NULL, &fds, NULL, &tv);

		if (urb.actual_length > 0)
		{
			debug_command_receive(cmd, urb.actual_length);
			return urb.actual_length;
		}

		if (timeout)
		{
			gettimeofday(&tv, NULL);
			if (timercmp(&tv, &tvref, >=))
			{
				waitting = 0;
				ERROR("Timeout happened waitting for device!\n");
			}
		}
	}

	return -1;
}

/******************************************************** 
 * show device descriptor
 *******************************************************/ 
static void debug_device_info(struct usb_device_descriptor *desc)
{
#ifdef DESCRIPTOR_DEBUG
	INFO("Device #:\n");
	INFO("    bLength              %d\n", desc->bLength);
	INFO("    bDescriptorType      %d\n", desc->bDescriptorType);
	INFO("    bcdUSB               %04x\n", desc->bcdUSB);
	INFO("    bDeviceClass         %d\n", desc->bDeviceClass);
	INFO("    bDeviceSubClass      %d\n", desc->bDeviceSubClass);
	INFO("    bDeviceProtocol      %d\n", desc->bDeviceProtocol);
	INFO("    bMaxPacketSize0      %d\n", desc->bMaxPacketSize0);
	INFO("    idVendor             %04x\n", desc->idVendor);
	INFO("    idProduct            %04x\n", desc->idProduct);
	INFO("    bcdDevice            %04x\n", desc->bcdDevice);
	INFO("    iManufacturer        %d\n", desc->iManufacturer);
	INFO("    iProduct             %d\n", desc->iProduct);
	INFO("    iSerialNumber        %d\n", desc->iSerialNumber);
	INFO("    bNumConfigurations   %d\n\n", desc->bNumConfigurations);
#endif
}

/******************************************************** 
 * show config descriptor
 *******************************************************/ 
static void debug_config_info(struct usb_config_descriptor *desc)
{
#ifdef DESCRIPTOR_DEBUG
	INFO("Config #:\n");
	INFO("    bLength              %d\n", desc->bLength);
	INFO("    bDescriptorType      %d\n", desc->bDescriptorType);
	INFO("    wTotalLength         %d\n", desc->wTotalLength);
	INFO("    bNumInterfaces       %d\n", desc->bNumInterfaces);
	INFO("    bConfigurationValue  %d\n", desc->bConfigurationValue);
	INFO("    iConfiguration       %d\n", desc->iConfiguration);
	INFO("    bmAttributes         %d\n", desc->bmAttributes);
	INFO("    bMaxPower            %d\n\n", desc->bMaxPower);
#endif
}

/******************************************************** 
 * show interface descriptor
 *******************************************************/ 
static void debug_interface_info(struct usb_interface_descriptor *desc)
{
#ifdef DESCRIPTOR_DEBUG
	INFO("Interface #:\n");
	INFO("    bLength              %d\n", desc->bLength);
	INFO("    bDescriptorType      %d\n", desc->bDescriptorType);
	INFO("    bInterfaceNumber     %d\n", desc->bInterfaceNumber);
	INFO("    bAlternateSetting    %d\n", desc->bAlternateSetting);
	INFO("    bNumEndpoints        %d\n", desc->bNumEndpoints);
	INFO("    bInterfaceClass      %d\n", desc->bInterfaceClass);
	INFO("    bInterfaceSubClass   %d\n", desc->bInterfaceSubClass);
	INFO("    bInterfaceProtocol   %d\n", desc->bInterfaceProtocol);
	INFO("    iInterface           %d\n\n", desc->iInterface);
#endif
}

/******************************************************** 
 * show endpoint descriptor
 *******************************************************/ 
static void debug_endpoint_info(struct usb_endpoint_descriptor *desc)
{
#ifdef DESCRIPTOR_DEBUG
	INFO("Endpoint #:\n");
	INFO("    bLength              %d\n", desc->bLength);
	INFO("    bDescriptorType      %d\n", desc->bDescriptorType);
	INFO("    bEndpointAddress     %02x\n", desc->bEndpointAddress);
	INFO("    bmAttributes         %02x\n", desc->bmAttributes);
	INFO("    wMaxPacketSize       %d\n", desc->wMaxPacketSize);
	INFO("    bInterval            %d\n", desc->bInterval);
	INFO("    bRefresh             %d\n", desc->bRefresh);
	INFO("    bSynchAddress        %d\n\n", desc->bSynchAddress);
#endif
}

/******************************************************** 
 * active the dest inferface
 *******************************************************/ 
int ipod_connect_device(ipod_device_t *dev)
{
	int i, ret, interface_num = IPOD_INTERFACE_NUM;
	char str[200], *p;
	struct usbdevfs_ioctl msg;
	struct usb_descriptor_header *header;
	struct usb_interface_descriptor *inf;
	struct usb_endpoint_descriptor *endp;
	int hid_interface_found = 0;

	for (i = 0; i < 3; i++)
	{
		ret = ioctl(dev->fd, USBDEVFS_CLAIMINTERFACE, &interface_num);
		if (ret < 0)
		{
			ERROR("Failed to claim ipod interface, errno %d\n", errno);

			msg.ifno = IPOD_INTERFACE_NUM;
			msg.ioctl_code = USBDEVFS_DISCONNECT;
			msg.data = NULL;

			ret = ioctl(dev->fd, USBDEVFS_IOCTL, &msg);
			if (ret < 0)
				ERROR("Failed to disconnect device from port!\n");
		}
	}

	for (i = 0; i < dev->dev_desc.bNumConfigurations; i++)
	{
		ret = read(dev->fd, &dev->cfg_desc, sizeof (struct usb_config_descriptor));
		if (ret < 0)
		{
			ERROR("Failed to get config descriptor!\n");
			return ret;
		}

		debug_config_info(&dev->cfg_desc);

		ret = read(dev->fd, str, dev->cfg_desc.wTotalLength - sizeof (struct usb_config_descriptor));
		if (ret < 0)
		{
			ERROR("Failed to get interface descriptor!\n");
			return ret;
		}

		p = str;
		while (p < (str + ret))
		{
			header = (struct usb_descriptor_header *)p;
			p += header->bLength;

			if (header->bDescriptorType == USB_DT_INTERFACE)
			{
				inf = (struct usb_interface_descriptor *)header;
				debug_interface_info(inf);

				if (inf->bInterfaceClass == USB_CLASS_HID)
					hid_interface_found = 1;
			}
			else if (header->bDescriptorType == USB_DT_ENDPOINT)
			{
				endp = (struct usb_endpoint_descriptor *)header;
				debug_endpoint_info(endp);

				if (hid_interface_found && ((endp->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT))
				{
					ipod_get_hid_report_descriptor(dev->fd, &hidrpt);
					memcpy(&dev->hidep, header, header->bLength);
					INFO("Get HID endpoint ok!\n");
					return 0;
				}
			}
		}
	}

	return -1;
}

/******************************************************** 
 * check if a device is Apple device
 *******************************************************/ 
static int check_device(int fd, struct usb_device_descriptor *desc)
{
	int ret;

	ret = read(fd, desc, sizeof (*desc));
	if (ret < 0)
	{
		INFO("Failed to read device descriptor!\n");
		return 0;
	}

	debug_device_info(desc);

	if (desc->idVendor == VENDOR_ID_APPLE)
		return 1;

	return 0;
}

/******************************************************** 
 * scan devices under a specific bus
 *******************************************************/ 
int bus_scan(char *bus, ipod_device_t *dev)
{
	int fd;
	struct dirent *child;
	char nodename[24];
	int ret = -EINVAL;
	DIR *dir = opendir(bus);

	if (NULL == dir)
	{
		ERROR("Directory %s does not exist!\n", bus);
		return ret;
	}

	while ((child = readdir(dir)) != NULL)
	{
		/* skip all items un-wanted */
		if (child->d_name[0] == '.')
			continue;

		/* get the child node name */
		sprintf(nodename, "%s/%s", bus, child->d_name);

		/* open the child node */
		fd = open(nodename, O_RDWR);
		if (fd < 0)
			continue;

		/* check if it is a Apple device */
		if (check_device(fd, &dev->dev_desc))
			return fd;

		/* not Apple device, close the handle */
		close(fd);
	}

	return ret;
}

