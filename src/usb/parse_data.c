#include <stdarg.h>
#include <usb/protocol.h>



#define PARSE_FLAG_EOF_REQUIRED 1
#define PARSE_FLAG_EXTEND_WHITESPACE 2

#define FLAG_INVERT 1
#define FLAG_HAS_FIRST_NUMBER 2
#define FLAG_HAS_SECOND_NUMBER 4
#define FLAG_HAS_NUMBER_DELIMETER 8



_Bool usb_parse_data(const char* data,unsigned int length,const char* fmt,...){
	if (!length){
		return !*fmt;
	}
	const char* base=data;
	va_list va;
	va_start(va,fmt);
	unsigned int parse_flags=PARSE_FLAG_EOF_REQUIRED;
	while (*fmt){
		if (*fmt=='%'){
			fmt++;
			if (!*fmt){
				usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Broken format code");
				goto _cleanup;
			}
			if (*fmt=='%'){
				fmt++;
				if (*data!='%'){
					goto _cleanup;
				}
				data++;
				length--;
			}
			else{
				unsigned int flags=0;
				unsigned long long int first_number=0;
				unsigned long long int second_number=0xffffffffffffffff;
				unsigned int size=32;
				while (*fmt!='u'&&*fmt!='d'&&*fmt!='x'&&*fmt!='?'&&*fmt!='c'&&*fmt!='s'&&*fmt!='l'&&*fmt!='f'&&*fmt!='$'&&*fmt!='#'){
					if (!*fmt){
						usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Broken format code");
						goto _cleanup;
					}
					if (*fmt=='!'){
						fmt++;
						if (flags&FLAG_INVERT){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Double flag inversion");
							goto _cleanup;
						}
						flags|=FLAG_INVERT;
					}
					else if (*fmt=='h'){
						fmt++;
						if (size!=32){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Multiple sizes specified");
							goto _cleanup;
						}
						if (*fmt=='h'){
							fmt++;
							size=8;
						}
						else{
							size=16;
						}
					}
					else if (*fmt=='l'){
						fmt++;
						if (size!=32){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Multiple sizes specified");
							goto _cleanup;
						}
						if (*fmt=='l'){
							fmt++;
						}
						size=64;
					}
					else if (*fmt==','){
						fmt++;
						if (!(flags&FLAG_HAS_FIRST_NUMBER)){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: First number required");
							goto _cleanup;
						}
						if (flags&FLAG_HAS_NUMBER_DELIMETER){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Too many delimeters");
							goto _cleanup;
						}
						flags|=FLAG_HAS_NUMBER_DELIMETER;
					}
					else if (*fmt=='-'||*fmt=='+'||(47<*fmt&&*fmt<58)){
						_Bool negative=0;
						if (*fmt=='-'){
							negative=1;
							fmt++;
						}
						else if (*fmt=='+'){
							fmt++;
						}
						if (*fmt<48||*fmt>57){
							usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: No number following sign");
							goto _cleanup;
						}
						unsigned long long int number=0;
						do{
							number=number*10+*fmt-48;
							fmt++;
						} while (47<*fmt&&*fmt<58);
						if (negative){
							number=-number;
						}
						if (flags&FLAG_HAS_NUMBER_DELIMETER){
							if (flags&FLAG_HAS_SECOND_NUMBER){
								usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Second number alread present");
								goto _cleanup;
							}
							flags|=FLAG_HAS_SECOND_NUMBER;
							second_number=number;
						}
						else{
							if (flags&FLAG_HAS_FIRST_NUMBER){
								usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: First number alread present");
								goto _cleanup;
							}
							flags|=FLAG_HAS_FIRST_NUMBER;
							first_number=number;
						}
					}
					else{
						usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Unknown format modifier character");
						goto _cleanup;
					}
				}
				switch (*fmt){
					case 'd':
						{
							_Bool negative=0;
							if (*data=='-'){
								negative=1;
								data++;
								length--;
							}
							else if (*data=='+'){
								data++;
								length--;
							}
							if (!length||*data<48||*data>57){
								goto _cleanup;
							}
							signed long long int number=0;
							while (length&&47<*data&&*data<58){
								length--;
								number=number*10+*data-48;
								data++;
							}
							if (negative){
								number=-number;
							}
							signed long long int first_number_sgn=first_number;
							signed long long int second_number_sgn=second_number;
							if (number<first_number_sgn){
								number=first_number_sgn;
							}
							else if (number>second_number_sgn){
								number=second_number_sgn;
							}
							switch (size){
								case 8:
									*va_arg(va,signed char*)=number;
									break;
								case 16:
									*va_arg(va,signed short int*)=number;
									break;
								case 64:
									*va_arg(va,signed long long int*)=number;
									break;
								default:
									*va_arg(va,signed int*)=number;
									break;
							}
							break;
						}
					case 'u':
						{
							if (*data<48||*data>57){
								goto _cleanup;
							}
							unsigned long long int number=0;
							while (length&&47<*data&&*data<58){
								length--;
								number=number*10+*data-48;
								data++;
							}
							if (number<first_number){
								number=first_number;
							}
							else if (number>second_number){
								number=second_number;
							}
							switch (size){
								case 8:
									*va_arg(va,unsigned char*)=number;
									break;
								case 16:
									*va_arg(va,unsigned short int*)=number;
									break;
								case 64:
									*va_arg(va,unsigned long long int*)=number;
									break;
								default:
									*va_arg(va,unsigned int*)=number;
									break;
							}
							break;
						}
					case 'x':
						{
							if ((*data<48||*data>57)&&(*data<65||*data>70)&&(*data<97||*data>102)){
								goto _cleanup;
							}
							unsigned long long int number=0;
							while (length&&((*data<48&&*data>57)||(*data<65&&*data>70)||(*data<97&&*data>102))){
								length--;
								number=number*16+*data-(*data>96?87:(*data>64?55:48));
								data++;
							}
							if (number<first_number){
								number=first_number;
							}
							else if (number>second_number){
								number=second_number;
							}
							switch (size){
								case 8:
									*va_arg(va,unsigned char*)=number;
									break;
								case 16:
									*va_arg(va,unsigned short int*)=number;
									break;
								case 64:
									*va_arg(va,unsigned long long int*)=number;
									break;
								default:
									*va_arg(va,unsigned int*)=number;
									break;
							}
							break;
						}
					case '?':
						{
							if (!first_number){
								first_number=1;
							}
							if (length<first_number){
								goto _cleanup;
							}
							length-=first_number;
							data+=first_number;
							break;
						}
					case 'c':
						{
							unsigned int value=*data;
							if (size==64){
								if ((value>>5)==0b110){
									if (length<2||(data[1]>>6)!=0b10){
										goto _cleanup;
									}
									value=((data[0]&0x1f)<<6)|(data[1]&0x3f);
									data++;
									length--;
								}
								else if ((value>>4)==0b1110){
									if (length<3||(data[1]>>6)!=0b10||(data[2]>>6)!=0b10){
										goto _cleanup;
									}
									value=((data[0]&0xf)<<6)|((data[1]&0x3f)<<6)|(data[2]&0x3f);
									data+=2;
									length-=2;
								}
								else if ((value>>3)==0b11110){
									if (length<4||(data[1]>>6)!=0b10||(data[2]>>6)!=0b10||(data[3]>>6)!=0b10){
										goto _cleanup;
									}
									value=((data[0]&0x7)<<6)|((data[1]&0x3f)<<12)|((data[2]&0x3f)<<6)|(data[3]&0x3f);
									data+=3;
									length-=3;
								}
							}
							data++;
							length--;
							*va_arg(va,unsigned int*)=value;
							break;
						}
					case 's':
						{
							if (flags&FLAG_INVERT){
								usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Dynamic memory allocation not implemented");
								goto _cleanup;
							}
							unsigned int string_length=0;
							if (!first_number){
								fmt++;
								char end_char=*fmt;
								if (!end_char){
									string_length=length;
								}
								else if (end_char=='%'){
									usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"[ERROR]: Lookahead character cannot be a format code");
									goto _cleanup;
								}
								else{
									while (string_length<length&&data[string_length]!=end_char){
										string_length++;
									}
								}
							}
							else{
								string_length=(length>first_number?first_number:length);
							}
							char* buffer=va_arg(va,char*);
							if (buffer){
								for (unsigned int i=0;i<string_length;i++){
									buffer[i]=*data;
									data++;
								}
							}
							else{
								data+=string_length;
							}
							length-=string_length;
							*va_arg(va,unsigned int*)=string_length;
							break;
						}
					case 'l':
						*va_arg(va,unsigned int*)=(unsigned int)(data-base);
						break;
					case 'f':
						{
							_Bool negative=0;
							if (*data=='-'){
								negative=1;
								data++;
								length--;
							}
							else if (*data=='+'){
								data++;
								length--;
							}
							if (!length||((*data<48||*data>57)&&*data!='.')){
								goto _cleanup;
							}
							float value=0.0f;
							unsigned int scale_power=-1;
							while (length){
								if (*data<48||*data>57){
									if (*data!='.'||scale_power!=-1){
										break;
									}
									scale_power=0;
								}
								else{
									value=value*10.0f+((float)(*data-48));
									if (scale_power!=-1){
										scale_power++;
									}
								}
								data++;
								length--;
							}
							if (scale_power!=-1){
								while (scale_power){
									scale_power--;
									value/=10.0f;
								}
							}
							if (negative){
								value=-value;
							}
							*va_arg(va,float*)=value;
							break;
						}
					case '$':
						if (flags&FLAG_INVERT){
							parse_flags&=~PARSE_FLAG_EOF_REQUIRED;
						}
						else{
							parse_flags|=PARSE_FLAG_EOF_REQUIRED;
						}
						break;
					case '#':
						if (flags&FLAG_INVERT){
							parse_flags&=~PARSE_FLAG_EXTEND_WHITESPACE;
						}
						else{
							parse_flags|=PARSE_FLAG_EXTEND_WHITESPACE;
						}
						break;
				}
				fmt++;
			}
		}
		else if ((parse_flags&PARSE_FLAG_EXTEND_WHITESPACE)&&(*fmt==' '||*fmt=='\t'||*fmt=='\n'||*fmt=='\r')){
			fmt++;
			while (length&&(*data==' '||*data=='\t'||*data=='\n'||*data=='\r')){
				length--;
				data++;
			}
		}
		else if (!length){
			if (!(parse_flags&PARSE_FLAG_EOF_REQUIRED)){
				va_end(va);
				return 1;
			}
			goto _cleanup;
		}
		else{
			if (*data!=*fmt){
				goto _cleanup;
			}
			fmt++;
			data++;
			length--;
		}
	}
_cleanup:
	va_end(va);
	return !length&&!*fmt;
}
