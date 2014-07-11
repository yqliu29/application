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
 * send RequestTransportMaxPayloadSize
 *******************************************************/ 
static int ipod_protocol_RequestTransportMaxPayloadSize(ipod_device_t *dev)
{
	int i;

	/* fill and send RequestTransportMaxPayloadSize */
	i = ipod_protocol_build_basic_command(dev, buffer, GENERAL_LINGO_ID,
			RequestTransportMaxPayloadSize, NULL, 0);
	i = ipod_send_command(dev, buffer, i);
	if (i < 0)
	{
		ERROR("Failed to send RequestTransportMaxPayloadSize: %d\n", errno);
		return i;
	}

	/* watting for ReturnTransportMaxPayloadSize */
	i = ipod_receive_command(dev, buffer, 1000);
	if (i < 0)
	{
		ERROR("Failed to receive ReturnTransportMaxPayloadSize: %d\n", errno);
		return i;
	}

	return 0;
}

/******************************************************** 
 * send SetFIDTokenValues
 *******************************************************/ 
static int ipod_protocol_SetFIDTokenValues(ipod_device_t *dev)
{
	int i;
	uint8_t tokens[512];
	uint8_t *p;
	uint32_t *p32;

	/* fill SetFIDTokenValues */
	tokens[0] = 2;
	p = &tokens[1];	

	/* fill IdentifyToken */
	p[0] = 14;				/* length */
	p[1] = 0;				/* FIDType */
	p[2] = 0;				/* FIDSubtype */
	p[3] = 3;				/* numLingos */
	p[4] = 0;
	p[5] = 2;
	p[6] = 4;
	p32 = (uint32_t *)&p[7];
	p32[0] = 0x00000002;	/* DeviceOptions */
	p32[1] = 0x01020304;	/* DeviceID */

	p += p[0] + 1;

	/* fill AccessoryCapsToken */
	p[0] = 10;				/* length */
	p[1] = 0;				/* FIDType */
	p[2] = 1;				/* FIDSubtype */
	p32 = (uint32_t *)&p[3];
	p32[0] = 0;
	p32[1] = 0;

	p += p[0] + 1;

	/* fill SetFIDTokenValues */
	i = ipod_protocol_build_basic_command(dev, buffer, GENERAL_LINGO_ID,
			SetFIDTokenValues, tokens, p - tokens);
	i = ipod_send_command(dev, buffer, i);
	if (i < 0)
	{
		ERROR("Failed to send SetFIDTokenValues: %d\n", errno);
		return i;
	}

	/* watting for AckFIDTokenValues */
	i = ipod_receive_command(dev, buffer, 1000);
	if (i < 0)
	{
		ERROR("Failed to receive AckFIDTokenValues: %d\n", errno);
		return i;
	}
}

/******************************************************** 
 * Identification process
 *******************************************************/ 
int ipod_protocol_identificate(ipod_device_t *dev)
{
	int ret;

	ret = ipod_protocol_StartIDPS(dev);
	if (ret < 0)
		return -1;

	ret = ipod_protocol_RequestTransportMaxPayloadSize(dev);
	if (ret < 0)
		return -1;

	ret = ipod_protocol_SetFIDTokenValues(dev);
	if (ret < 0)
		return -1;
}
