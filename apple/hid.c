#include "main.h"
#include "hid.h"

/******************************************************** 
 * get hid device descriptor
 *******************************************************/ 
static int ipod_get_hid_descriptor(int fd, hid_descriptor_t *hid)
{
	struct usbdevfs_ctrltransfer msg;
	int ret;

	msg.bRequestType        = 0x81;
	msg.bRequest            = USB_REQ_GET_DESCRIPTOR;
	msg.wValue              = HID_DT_HID << 8;
	msg.wIndex              = IPOD_HID_INTERFACE_NUM;
	msg.wLength             = sizeof (hid_descriptor_t);
	msg.data                = hid;
	msg.timeout             = USB_HID_TIMEOUT;

	memset(hid, 0, sizeof (hid_descriptor_t));
	ret = ioctl(fd, USBDEVFS_CONTROL, &msg);
#ifdef __HID_DEBUG__
	fprintf(stderr, "hid bLength:            %d\n", hid->bLength);
	fprintf(stderr, "hid bDescriptorType:    %d\n", hid->bDescriptorType);
	fprintf(stderr, "hid bcdHID:             %04x\n", hid->bcdHID);
	fprintf(stderr, "hid bCountryCode:       %d\n", hid->bCountryCode);
	fprintf(stderr, "hid bNumDescriptors:    %d\n", hid->bNumDescriptors);
	fprintf(stderr, "hid bDescriptorType2:   %d\n", hid->bDescriptorType2);
	fprintf(stderr, "hid wDescriptorLength:  %d\n", hid->wDescriptorLength);
#endif

	return ret;
}

/********************************************************
 * get report descriptor
 *******************************************************/ 
static int ipod_get_hid_raw_report_descriptor(int fd, unsigned char *buf, int size)
{
	struct usbdevfs_ctrltransfer msg;

	msg.bRequestType        = 0x81;
	msg.bRequest            = USB_REQ_GET_DESCRIPTOR;
	msg.wValue              = HID_DT_REPORT << 8;
	msg.wIndex              = IPOD_HID_INTERFACE_NUM;
	msg.wLength             = size;
	msg.data                = buf;
	msg.timeout             = USB_HID_TIMEOUT;

	memset(buf, 0, size);
	return ioctl(fd, USBDEVFS_CONTROL, &msg);
}

/******************************************************** 
 * set default size for report descriptor
 *******************************************************/ 
static void ipod_hid_set_default_report(hid_report_t *rpt)
{
	/* max packet size for IN and OUT */
	rpt->max_in_size = 63;
	rpt->max_out_size = 63;

	/* input report set */
	rpt->input_report_num = 4;
	rpt->input_desc[0].report_id = 1;
	rpt->input_desc[1].report_id = 2;
	rpt->input_desc[2].report_id = 3;
	rpt->input_desc[3].report_id = 4;
	rpt->input_desc[0].report_size = 13;
	rpt->input_desc[1].report_size = 15;
	rpt->input_desc[2].report_size = 21;
	rpt->input_desc[3].report_size = 64;

	/* output report set */
	rpt->output_report_num = 5;
	rpt->output_desc[0].report_id = 5;
	rpt->output_desc[1].report_id = 6;
	rpt->output_desc[2].report_id = 7;
	rpt->output_desc[3].report_id = 8;
	rpt->output_desc[4].report_id = 9;
	rpt->output_desc[0].report_size = 9;
	rpt->output_desc[1].report_size = 11;
	rpt->output_desc[2].report_size = 15;
	rpt->output_desc[3].report_size = 21;
	rpt->output_desc[4].report_size = 64;
}

/******************************************************** 
 * check if a valid item is get
 *******************************************************/ 
static void check_and_set_report(struct hid_item *it, hid_report_t *ipt)
{
	if (it->usage && it->input_output && it->report_size)
	{
		if (it->input_output == HID_ITEM_INPUT)
		{
			if (ipt->input_report_num < USB_HID_MAX_REPORT)
			{
				ipt->input_desc[ipt->input_report_num].report_id = it->report_id;
				ipt->input_desc[ipt->input_report_num].report_size = it->report_size + 1;
				if (ipt->max_in_size < (it->report_size + 1))
					ipt->max_in_size = it->report_size + 1;
				ipt->input_report_num++;
#ifdef __HID_DEBUG__
				fprintf(stderr, "Input Report %d#\n", ipt->input_report_num);
				fprintf(stderr, "report id:     %d\n", it->report_id);
				fprintf(stderr, "report size:   %d\n", it->report_size);
#endif
			}
		}
		else
		{
			if (ipt->output_report_num < USB_HID_MAX_REPORT)
			{
				ipt->output_desc[ipt->output_report_num].report_id = it->report_id;
				ipt->output_desc[ipt->output_report_num].report_size = it->report_size + 1;
				if (ipt->max_out_size < (it->report_size + 1))
					ipt->max_out_size = it->report_size + 1;
				ipt->output_report_num++;
#ifdef __HID_DEBUG__
				fprintf(stderr, "Output Report %d#\n", ipt->output_report_num);
				fprintf(stderr, "report id:     %d\n", it->report_id);
				fprintf(stderr, "report size:   %d\n", it->report_size);
#endif
			}
		}
	}
}

/******************************************************** 
 * parse an item
 *******************************************************/ 
static unsigned char *ipod_hid_parse_item(unsigned char item, int size, unsigned char *p, struct hid_item *it, hid_report_t *ipt)
{
	switch (item)
	{
		case HID_ITEM_INPUT:
		case HID_ITEM_OUTPUT:
			it->input_output = item;
			check_and_set_report(it, ipt);
			break;

		case HID_ITEM_FEATURE:
			break;

		case HID_ITEM_COLLECTION:
			break;

		case HID_ITEM_END_COLLECTION:
			break;

		case HID_ITEM_USAGE_PAGE:
			break;

		case HID_ITEM_LOGI_MINI:
			break;

		case HID_ITEM_LOGI_MAXI:
			break;

		case HID_ITEM_PHY_MINI:
			break;

		case HID_ITEM_PHY_MAXI:
			break;

		case HID_ITEM_UNIT_EXPT:
			break;

		case HID_ITEM_UNIT:
			break;

		case HID_ITEM_REPORT_SIZE:
			break;

		case HID_ITEM_REPORT_ID:
			it->report_id = *p;
			break;

		case HID_ITEM_REPORT_COUNT:
			if (size == 1)
				it->report_size = *p;
			else if (size == 2)
				it->report_size = *(unsigned short *)p;
			else if (size == 4)
				it->report_size = *(unsigned int *)p;
			break;

		case HID_ITEM_PUSH:
			break;

		case HID_ITEM_POP:
			break;

		case HID_ITEM_USAGE:
			memset(it, 0, sizeof (struct hid_item));
			it->usage = 1;
			break;
	}

	return p + size;
}

/******************************************************** 
 * parse raw descriptor
 *******************************************************/ 
static int ipod_hid_parse_raw_descriptor(unsigned char *buf, unsigned char *end, hid_report_t *rpt)
{
	struct hid_item hitem;
	unsigned char b;
	unsigned char *p = buf;
	int bSize;

	memset(&hitem, 0, sizeof (hitem));
	memset(rpt, 0, sizeof (hid_report_t));

	do
	{
		b = *p;
		bSize = HID_ITEM_SIZE(b);

		p = ipod_hid_parse_item(HID_ITEM_TAG(b), bSize, p+1, &hitem, rpt);
	} while (p < end);

	if ((rpt->input_report_num == 0) || (rpt->output_report_num == 0))
		return -1;

	return 0;
}

/******************************************************** 
 * get report descriptor from ipod
 *******************************************************/
int ipod_get_hid_report_descriptor(int fd, hid_report_t *rpt)
{
	int ret;
	hid_descriptor_t hid;
	unsigned char buffer[USB_HID_MAX_RAW_DESCRIPTOR];

	/* get raw descriptor length */
	ret = ipod_get_hid_descriptor(fd, &hid);
	if (ret < 0)
	{
		fprintf(stderr, "%s: Failed to get hid descriptor!\n", __func__);
		goto err_out;
	}

	/* check length */
	if (hid.wDescriptorLength > sizeof (buffer))
	{
		fprintf(stderr, "%s: Report descriptor too large!\n", __func__);
		goto err_out;
	}

	/* get raw report descriptor */
	ret = ipod_get_hid_raw_report_descriptor(fd, buffer, hid.wDescriptorLength);
	if (ret < 0)
	{
		fprintf(stderr, "%s: Failed to get raw report descriptor!\n", __func__);
		goto err_out;
	}

#ifdef __HID_DEBUG__
	for (ret = 0; ret < hid.wDescriptorLength; ret++)
		fprintf(stderr, "%02x ", buffer[ret]);
	fprintf(stderr, "\n");
#endif

	/* parse raw descriptor */
	ret = ipod_hid_parse_raw_descriptor(buffer, buffer + hid.wDescriptorLength, rpt);
	if (ret < 0)
	{
		fprintf(stderr, "%s: Failed to parse raw report descriptor!\n", __func__);
		goto err_out;
	}

	return 0;

err_out:
	fprintf(stderr, "%s: Use default hid report!\n", __func__);
	ipod_hid_set_default_report(rpt);
	return 0;
}

/******************************************************** 
 * get valid report id
 *******************************************************/ 
unsigned char hid_report_get_id(hid_report_t *rpt, int size)
{
	int i;

	for (i = 0; i < rpt->output_report_num; i++)
	{
		if (size == rpt->output_desc[i].report_size)
			return rpt->output_desc[i].report_id;
	}

	/* default report id for 64 bytes report if failed */
	return 9;
}

/******************************************************** 
 * get valid report size
 *******************************************************/
int hid_report_get_size(hid_report_t *rpt, int size)
{
	int i;

	if (size >= rpt->max_out_size)
		return rpt->max_out_size;

	for (i = 0; i < rpt->output_report_num; i++)
	{
		if (size <= rpt->output_desc[i].report_size)
			return rpt->output_desc[i].report_size;
	}

	/* default packet size if failed */
	return 64;
}

/******************************************************** 
 * get max packet size supported
 *******************************************************/ 
int hid_report_max_out_size(hid_report_t *rpt, int max_buf_size)
{
	int i;

	/* the most probally situation */
	if (max_buf_size >= rpt->max_out_size)
		return rpt->max_out_size;

	for (i = rpt->output_report_num -1; i >= 0; i--)
	{
		if (max_buf_size >= rpt->output_desc[i].report_size)
			return rpt->output_desc[i].report_size;
	}

	/* return default packet size if failed */
	return 64;
}

