#ifndef __HID_H__
#define __HID_H__

#include <linux/hid.h>
#include <linux/hiddev.h>
#include <linux/usb/ch9.h>

/******************************************************** 
 * macroes
 *******************************************************/
#define IPOD_HID_INTERFACE_NUM		2
#define USB_HID_TIMEOUT				1000
#define USB_HID_MAX_RAW_DESCRIPTOR	512
#define USB_HID_MAX_REPORT			15

/******************************************************** 
 * HID protocol definations
 *******************************************************/ 
#define HID_ITEM_SIZE(x) 			((((x)&0x03) == 3)?4:(x)&0x03)

#define HID_ITEM_TYPE(x)			(((x) >> 2) & 0x03)
#define HID_ITEM_TYPE_MAIN			0
#define HID_ITEM_TYPE_GLOBAL		1
#define HID_ITEM_TYPE_LOCAL			2

#define HID_ITEM_TAG(x)				((x)&0xFC)
#define HID_ITEM_INPUT				0x80
#define HID_ITEM_OUTPUT				0x90
#define HID_ITEM_FEATURE			0xB0
#define HID_ITEM_COLLECTION			0xA0
#define HID_ITEM_END_COLLECTION		0xC0
#define HID_ITEM_USAGE_PAGE			0x04
#define HID_ITEM_LOGI_MINI			0x14
#define HID_ITEM_LOGI_MAXI			0x24
#define HID_ITEM_PHY_MINI			0x34
#define HID_ITEM_PHY_MAXI			0x44
#define HID_ITEM_UNIT_EXPT			0x54
#define HID_ITEM_UNIT				0x64
#define HID_ITEM_REPORT_SIZE		0x74
#define HID_ITEM_REPORT_ID			0x84
#define HID_ITEM_REPORT_COUNT		0x94
#define HID_ITEM_PUSH				0xa4
#define HID_ITEM_POP				0xb4
#define HID_ITEM_USAGE				0x08

/******************************************************** 
 * data structure
 *******************************************************/ 
struct hid_descriptor
{
	unsigned char		bLength;
	unsigned char 		bDescriptorType;
	unsigned short		bcdHID;
	unsigned char		bCountryCode;
	unsigned char 		bNumDescriptors;
	unsigned char		bDescriptorType2;
	unsigned short		wDescriptorLength;
}__attribute__((packed));

struct hid_item
{
	int usage;
	unsigned char input_output;
	unsigned char report_id;
	int report_size;
};

struct report_desc
{
	unsigned char report_id;
	int report_size;
};

struct hid_report
{
	int max_in_size;			/* max size of input packet */
	int max_out_size;			/* max size of output packet */
	int input_report_num;
	int output_report_num;
	struct report_desc input_desc[USB_HID_MAX_REPORT];
	struct report_desc output_desc[USB_HID_MAX_REPORT];
};

typedef struct hid_descriptor hid_descriptor_t;
typedef struct hid_report hid_report_t;

/******************************************************** 
 * functions
 *******************************************************/ 
int ipod_get_hid_report_descriptor(int fd, hid_report_t *rpt);
int hid_report_max_out_size(hid_report_t *rpt, int max_buf_size);
unsigned char hid_report_get_id(hid_report_t *rpt, int size);
int hid_report_get_size(hid_report_t *rpt, int size);

#endif

