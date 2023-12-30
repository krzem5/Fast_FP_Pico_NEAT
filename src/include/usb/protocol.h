#ifndef _USB_PROTOCOL_H_
#define _USB_PROTOCOL_H_ 1



#define USB_PROTOCOL_LOG_FLAG_ERROR 1

#define USB_PACKET_TYPE_LOG 0x00
#define USB_PACKET_TYPE_INPUT 0x01
#define USB_PACKET_TYPE_RESET_SERVER 0xff

#define USB_MAX_LOG_LINE_LENGTH 255

#define USB_PROTOCOL_ERROR(str,...) \
	do{ \
		const char* __fp=__FILE__; \
		const char* __fp2=__fp; \
		unsigned int __i=0; \
		while (*__fp2){ \
			if (*__fp2=='/'){ \
				__i=__fp2-__fp; \
			} \
			__fp2++; \
		} \
		usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[%s:%u(%s)] "str,__FILE__+__i+1,__LINE__,__func__,##__VA_ARGS__); \
	} while (0)



typedef void (*usb_protocol_input_callback_t)(unsigned char,const char*);



void usb_protocol_set_input_callback(usb_protocol_input_callback_t callback);



void usb_protocol_update(void);



void usb_protocol_send_log(unsigned char flags,const char* format,...);



#endif
