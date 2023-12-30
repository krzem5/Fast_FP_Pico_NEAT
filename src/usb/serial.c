#include <usb/serial.h>
#include <tusb.h>



unsigned char _usb_read_buffer[(PACKET_BUFFER_MASK+1)<<1];
usb_data_length_t _usb_read_buffer_start=0;
usb_data_length_t _usb_read_buffer_end=0;



void usb_init(void){
	tusb_init();
}



void usb_update(void){
	tud_task();
}



void usb_read(void){
	usb_data_length_t space=(_usb_read_buffer_start-_usb_read_buffer_end-1)&PACKET_BUFFER_MASK;
	if (space&&tud_cdc_connected()){
		usb_data_length_t count=tud_cdc_available();
		if (count>space){
			count=space;
		}
		count=tud_cdc_read(_usb_read_buffer+_usb_read_buffer_end,count);
		_usb_read_buffer_end+=count;
		for (usb_data_length_t i=PACKET_BUFFER_MASK+1;i<_usb_read_buffer_end;i++){
			_usb_read_buffer[i-PACKET_BUFFER_MASK-1]=_usb_read_buffer[i];
		}
		_usb_read_buffer_end&=PACKET_BUFFER_MASK;
	}
}



void usb_write(const unsigned char* data,usb_data_length_t length){
	while (tud_cdc_connected()&&length){
		usb_data_length_t count=tud_cdc_write_available();
		if (count>length){
			count=length;
		}
		if (count){
			count=tud_cdc_write(data,count);
			data+=count;
			length-=count;
		}
		tud_task();
		tud_cdc_write_flush();
	}
}
