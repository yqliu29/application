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
#define StartIDPS				0x38
#define SetFIDTokenValues		0x39

/******************************************************** 
 * functions
 *******************************************************/
int ipod_protocol_identificate(ipod_device_t *dev);

#endif
