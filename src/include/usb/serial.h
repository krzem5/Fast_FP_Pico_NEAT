#ifndef _USB_SERIAL_H_
#define _USB_SERIAL_H_ 1



#define PACKET_BUFFER_MASK 63



typedef unsigned short int usb_data_length_t;



extern unsigned char _usb_read_buffer[(PACKET_BUFFER_MASK+1)<<1];
extern usb_data_length_t _usb_read_buffer_start;
extern usb_data_length_t _usb_read_buffer_end;



static inline usb_data_length_t usb_data_length(void){
	return (_usb_read_buffer_end-_usb_read_buffer_start+PACKET_BUFFER_MASK+1)&PACKET_BUFFER_MASK;
}



static inline unsigned char usb_get_data(usb_data_length_t offset){
	return _usb_read_buffer[(offset+_usb_read_buffer_start)&PACKET_BUFFER_MASK];
}



static inline void usb_consume_data(usb_data_length_t count){
	_usb_read_buffer_start=(_usb_read_buffer_start+count)&PACKET_BUFFER_MASK;
}



void usb_init(void);



void usb_update(void);



void usb_read(void);



void usb_write(const unsigned char* data,usb_data_length_t length);



#endif
