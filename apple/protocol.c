#include "main.h"
#include "device.h"
#include "protocol.h"
#include "hid.h"

/******************************************************** 
 * static data
 *******************************************************/ 
static uint8_t buffer[COMMAND_BUFFER_SIZE];

/******************************************************** 
 * build basic command
 *******************************************************/ 
int ipod_protocol_build_basic_command(ipod_device_t *dev, uint8_t *buf, uint8_t lingoID, 
										uint8_t commandID, uint8_t *Payload, int payloadsize)
{
	uint8_t checksum = 0;
	int length = 4 + payloadsize;
	int i = 0, j;

	/* start field */
	buf[i++] = 0x55;

	/* length field */
	if (length <= 252)
	{
		buf[i++] = (uint8_t)length;
	}
	else
	{
		buf[i++] = 0;
		buf[i++] = (uint8_t)((length >> 8)&0xFF);
		buf[i++] = (uint8_t)(length & 0xFF);
	}

	/* lingo id field */
	buf[i++] = lingoID;

	/* command id field */
	buf[i++] = commandID;

	/* transaction id field */
	buf[i++] = (uint8_t)((dev->send_tid >> 8) & 0xFF);
	buf[i++] = (uint8_t)(dev->send_tid & 0xFF);
	dev->send_tid++;

	/* Payload field */
	if (payloadsize > 0)
	{
		memcpy(&buf[i], Payload, payloadsize);
		i += payloadsize;
	}

	/* checksum field */
	for (j = 1; j < i; j++)
		checksum += buf[j];

	buf[i++] = ~checksum + 1;

	return i;
}

/******************************************************** 
 * send StartIDPS
 *******************************************************/ 
static int ipod_protocol_StartIDPS(ipod_device_t *dev)
{
	int i;
	uint16_t tid1 = dev->send_tid;
	uint16_t tid2;

	/* fill and send StartIDPS */
	i = ipod_protocol_build_basic_command(dev, buffer, GENERAL_LINGO_ID, StartIDPS, NULL, 0);
	i = ipod_send_command(dev, buffer, i);
	if (i < 0)
	{
		ERROR("Failed to send StartIDPS: %d\n", errno);
		return i;
	}

	/* watting for iPodAck */
	i = ipod_receive_command(dev, buffer, 1000);
	if (i < 0)
	{
		ERROR("Failed to receive iPodAck for StartIDPS: %d\n", errno);
		return i;
	}

	return 0;
}

/******************************************************** 
 * Identification process
 *******************************************************/ 
int ipod_protocol_identificate(ipod_device_t *dev)
{
	int ret;
	hid_report_t report;

	ret = ipod_get_hid_report_descriptor(dev->fd, &report);
	if (ret < 0)
		return ret;

	ret = ipod_protocol_StartIDPS(dev);
	if (ret < 0)
		return -1;
}
