#include "main.h"
#include "device.h"
#include "protocol.h"

/******************************************************** 
 * static data
 *******************************************************/ 
static ipod_device_t ipod_dev;

/******************************************************** 
 * The entry of the program
 *******************************************************/ 
int main(int argc, char **argv)
{
	char bus_path[20];
	int bus_num;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <bus num>\n", argv[0]);
		return 0;
	}

	bus_num = atoi(argv[1]);
	if ((bus_num != 1) && (bus_num != 2))
	{
		fprintf(stderr, "Bus number should be 1 or 2!\n");
		return -1;
	}

	sprintf(bus_path, "/proc/bus/usb/%03d", bus_num);
	
	ipod_dev.fd = bus_scan(bus_path, &ipod_dev);
	if (ipod_dev.fd < 0)
		return -1;

	INFO("Apple device found!\n");

	ipod_connect_device(&ipod_dev);
	ipod_dev.send_tid = IPOD_INITIAL_TID;

	ipod_protocol_identificate(&ipod_dev);

	close(ipod_dev.fd);

	return 0;
}
