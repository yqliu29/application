#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "device.h"

#define IPOD_INTERFACE_NUM		2
#define IPOD_REPORT_ID			0x11
#define IPOD_INITIAL_TID		0x0D

#define COMMAND_BUFFER_SIZE		0x800

/******************************************************** 
 * Lingo IDs
 *******************************************************/ 
#define GENERAL_LINGO_ID		0x00

/******************************************************** 
 * General Lingo Commands
 *******************************************************/ 
#define RequestTransportMaxPayloadSize		0x11
#define ReturnTransportMaxPayloadSize		0x12
#define StartIDPS							0x38
#define SetFIDTokenValues					0x39
#define AckFIDTokenValues					0x3A

/******************************************************** 
 * functions
 *******************************************************/
int ipod_protocol_identificate(ipod_device_t *dev);

#endif
