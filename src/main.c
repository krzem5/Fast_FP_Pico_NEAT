#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/structs/systick.h>
#include <hardware/watchdog.h>
#include <neat/neat.h>
#include <pico/bootrom.h>
#include <pico/time.h>
#include <usb/data_parser.h>
#include <usb/protocol.h>
#include <usb/serial.h>



#define TEST_COUNT 1500000



const unsigned int neat_model[]={
	0x6e1ab2d2,0x1c000007,0x80060103,0x278c1ed7,0x2def21fd,0x40004efa,0x3e1c33d1,0x39f127c7,
	0x400041f9,
};



static inline void watchdog_disable(void){
	hw_clear_bits(&(watchdog_hw->ctrl),WATCHDOG_CTRL_ENABLE_BITS);
}



static void _input_callback(unsigned char length,const char* data){
	if (usb_parse_data(data,length,"test")){
		usb_protocol_send_log(0,"Starting test...");
		watchdog_disable();
		uint64_t start=time_us_64();
		for (unsigned int i=0;i<TEST_COUNT;i++){
			neat_evaluate();
		}
		uint64_t diff=time_us_64()-start;
		watchdog_enable(500,0);
		usb_protocol_send_log(0,"Time: %f s (%f µs/eval, ≈%f cycles/eval)",diff/1000000.0f,diff/((float)TEST_COUNT),diff/((float)TEST_COUNT)*clock_get_hz(clk_sys)/1000000.0f);
	}
	else if (usb_parse_data(data,length,"test_tick")){
		usb_protocol_send_log(0,"Starting test (systick counter)...");
		watchdog_disable();
		uint64_t total=0;
		systick_hw->csr=0x5;
		systick_hw->rvr=0xffffff;
		for (unsigned int i=0;i<TEST_COUNT;i++){
			systick_hw->cvr=0xffffff;
			asm("":::"memory");
			neat_evaluate();
			unsigned int end=systick_hw->cvr;
			asm("":::"memory");
			total+=(0xffffff-end)&0xffffff;
		}
		watchdog_enable(500,0);
		usb_protocol_send_log(0,"%q cycles, %f cycles/eval",total,total/((float)TEST_COUNT));
	}
	else if (usb_parse_data(data,length,"id")){
		usb_protocol_send_log(0,"Current model ID: %X%X",neat_hash>>16,neat_hash&0xffff);
	}
	else if (usb_parse_data(data,length,"reload")){
		usb_protocol_send_log(0,"Reloading model...");
		if (neat_init(neat_model)){
			usb_protocol_send_log(0,"Successfully reloaded model");
		}
		else{
			usb_protocol_send_log(USB_PROTOCOL_LOG_FLAG_ERROR,"Unable to reload model: Checksum mismatch");
		}
	}
	else if (usb_parse_data(data,length,"%f,%f,%f",neat_io,neat_io+1,neat_io+2)){
		float args[3]={neat_io[0],neat_io[1],neat_io[2]};
		neat_evaluate();
		usb_protocol_send_log(0,"%f^%f^%f = %f",args[0],args[1],args[2],neat_io[0]*0.5f+0.5f);
	}
	else{
		for (unsigned int i=0;i<8;i++){
			neat_io[0]=(i&1)*1.0f;
			neat_io[1]=((i>>1)&1)*1.0f;
			neat_io[2]=(i>>2)*1.0f;
			neat_evaluate();
			usb_protocol_send_log(0,"%u^%u^%u = %f",i&1,(i>>1)&1,i>>2,neat_io[0]*0.5f+0.5f);
		}
	}
}



int main(void){
	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);
	gpio_put(PICO_DEFAULT_LED_PIN,1);
	neat_init(neat_model);
	usb_init();
	usb_protocol_set_input_callback(_input_callback);
	watchdog_enable(500,0);
	while (1){
		usb_update();
		usb_protocol_update();
		watchdog_update();
	}
	reset_usb_boot(0,0);
}
