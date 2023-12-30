#include <pico/bootrom.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <usb/protocol.h>
#include <usb/serial.h>



static const usb_data_length_t _usb_packet_type_to_length[256]={
	[USB_PACKET_TYPE_INPUT]=3,
	[USB_PACKET_TYPE_RESET_SERVER]=2
};



static usb_protocol_input_callback_t _usb_protocol_input_callback=NULL;



void usb_protocol_set_input_callback(usb_protocol_input_callback_t callback){
	_usb_protocol_input_callback=callback;
}



void usb_protocol_update(void){
	usb_read();
	while (usb_data_length()&&usb_get_data(0)!=0xff){
		usb_consume_data(1);
	}
	usb_data_length_t length=usb_data_length();
	if (length<2){
		return;
	}
	unsigned char type=usb_get_data(1);
	usb_data_length_t required_length=_usb_packet_type_to_length[type];
	if (length<required_length){
		return;
	}
	if (type==USB_PACKET_TYPE_INPUT){
		length=usb_get_data(2);
		if (length<required_length){
			return;
		}
	}
	switch (type){
		case USB_PACKET_TYPE_RESET_SERVER:
			reset_usb_boot(0,0);
			break;
		case USB_PACKET_TYPE_INPUT:
			if (_usb_protocol_input_callback){
				char buffer[256];
				for (usb_data_length_t i=3;i<length;i++){
					buffer[i-3]=usb_get_data(i);
				}
				buffer[length-3]=0;
				_usb_protocol_input_callback(length-3,buffer);
			}
			break;
	}
	usb_consume_data(required_length);
}



void usb_protocol_send_log(unsigned char flags,const char* format,...){
	va_list va;
	va_start(va,format);
	uint8_t buffer[USB_MAX_LOG_LINE_LENGTH]={0xff,USB_PACKET_TYPE_LOG,0,flags};
	uint16_t i=4;
	while (i<USB_MAX_LOG_LINE_LENGTH&&*format){
		if (*format!='%'){
			buffer[i]=*format;
			format++;
			i++;
			continue;
		}
		format++;
		switch (*format){
			case 's':
				{
					format++;
					const char* ptr=va_arg(va,const char*);
					while (i<USB_MAX_LOG_LINE_LENGTH&&*ptr){
						buffer[i]=*ptr;
						ptr++;
						i++;
					}
					break;
				}
			case 'c':
				format++;
				buffer[i]=(char)va_arg(va,int);
				i++;
				break;
			case 'x':
				{
					format++;
					uint8_t number=(uint8_t)va_arg(va,uint32_t);
					uint8_t j=8;
					while (j&&i<USB_MAX_LOG_LINE_LENGTH){
						j-=4;
						uint8_t nibble=(number>>j)&0xf;
						buffer[i]=nibble+(nibble>9?87:48);
						i++;
					}
					break;
				}
			case 'X':
				{
					format++;
					uint16_t number=(uint16_t)va_arg(va,uint32_t);
					uint8_t j=16;
					while (j&&i<USB_MAX_LOG_LINE_LENGTH){
						j-=4;
						uint8_t nibble=(number>>j)&0xf;
						buffer[i]=nibble+(nibble>9?87:48);
						i++;
					}
					break;
				}
			case 'u':
				{
					format++;
					uint32_t number=va_arg(va,uint32_t);
					uint8_t number_buffer[10];
					uint8_t j=0;
					do{
						number_buffer[j]=number%10;
						number/=10;
						j++;
					} while (number);
					while (i<USB_MAX_LOG_LINE_LENGTH&&j){
						j--;
						buffer[i]=number_buffer[j]+48;
						i++;
					}
					break;
				}
			case 'q':
				{
					format++;
					uint64_t number=va_arg(va,uint64_t);
					uint8_t number_buffer[20];
					uint8_t j=0;
					do{
						number_buffer[j]=number%10;
						number/=10;
						j++;
					} while (number);
					while (i<USB_MAX_LOG_LINE_LENGTH&&j){
						j--;
						buffer[i]=number_buffer[j]+48;
						i++;
					}
					break;
				}
			case 'f':
				{
					format++;
					double f=va_arg(va,double);
					if (f<0){
						f=-f;
						if (i<USB_MAX_LOG_LINE_LENGTH){
							buffer[i]='-';
							i++;
						}
					}
					uint64_t v=(uint64_t)(f*1000.0);
					char float_buffer[36];
					uint8_t j=0;
					while (j<36&&(v>=1.0||j<5)){
						float_buffer[j]=48+(v%10);
						v/=10;
						j++;
						if (j==3){
							float_buffer[j]='.';
							j++;
						}
					}
					uint8_t k=0;
					while (k<2&&float_buffer[k]=='0'){
						k++;
					}
					while (i<USB_MAX_LOG_LINE_LENGTH&&j>k){
						j--;
						buffer[i]=float_buffer[j];
						i++;
					}
					break;
				}
			case '%':
				format++;
			default:
				buffer[i]='%';
				i++;
				break;
		}
	}
	va_end(va);
	buffer[2]=i;
	usb_write(buffer,i);
}
