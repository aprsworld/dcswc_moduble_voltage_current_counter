#include "dcswc_module_voltage_current_counter.h"


typedef struct {
	int8 serial_prefix;
	int16 serial_number;
	int16 startup_power_on_delay;
} struct_config;



typedef struct {
	int32 vbus_a, vshunt_a;
	int32 vbus_b, vshunt_b;

	int16 count_a_last_second, count_b_last_second;

	int32 count_a_long;
	int32 count_b_long;
	int32 count_seconds_long;

	int16 dietemp_a;
	int16 dietemp_b;
} struct_current;

typedef struct {
	/* action flags */
	int1 now_millisecond;

	int1 now_ina;    // query ina registers
	int1 now_strobe; // copy next to current

	/* timers */
	int8 led_on_a;
} struct_time_keep;

/* global structures */
struct_current current={0};
struct_current next={0};
struct_time_keep timers={0};

#include "ina228.c"
#include "i2c_handler_dcswc_module_voltage_current_counter.c"
#include "interrupt_dcswc_module_voltage_current_counter.c"


void init(void) {
	setup_vref(VREF_OFF);
	setup_dac(DAC_OFF);
	setup_adc(ADC_OFF);
	setup_adc_ports(NO_ANALOGS);



	set_tris_a    (0b00111111);
	port_a_pullups(0b00110000);
//                   76543210

	set_tris_b    (0b01110000);
	port_b_pullups(0b00000000);
//                   76543210

	set_tris_c    (0b11110011);
//                   76543210


	/* data structure initialization */
	/* all initialized to 0 on declaration. Just do this if need non-zero */


	/* one periodic interrupt @ 1mS. Generated from system 16 MHz clock */
	/* prescale=16, match=249, postscale=1. Match is 249 because when match occurs, one cycle is lost */
	setup_timer_2(T2_DIV_BY_16,249,1);

	enable_interrupts(INT_TIMER2);
}

int8 read_dip_switch(void) {
	return input(PIC_ADDR_MSB)<<1 | input(PIC_ADDR_LSB);
}

void action_now_strobe(void) {
	output_high(TP2);
	timers.now_strobe=0;

	disable_interrupts(GLOBAL);
	current.vbus_a=next.vbus_a;
	current.vshunt_a=next.vshunt_a;
	current.dietemp_a=next.dietemp_a;	

	current.vbus_b=next.vbus_b;
	current.vshunt_b=next.vshunt_b;
	current.dietemp_b=next.dietemp_b;

	current.count_a_last_second=next.count_a_last_second;

	current.count_b_last_second=next.count_b_last_second;	

	current.count_a_long += current.count_a_last_second;
	current.count_b_long += current.count_b_last_second;
	
	current.count_seconds_long++;

	/* reset our counters */
	next.count_a_last_second=0;
	next.count_b_last_second=0;

	enable_interrupts(GLOBAL);

	output_low(TP2);
}

void action_now_ina(void) {
	timers.now_ina=0;

	/* sample INA228 at middle of 1 second window */
	next.vbus_a=ina228_read24(INA228_A_ADDR,INA228_REG_VBUS);
	next.vshunt_a=ina228_read24(INA228_A_ADDR,INA228_REG_VSHUNT);

	next.vbus_b=ina228_read24(INA228_B_ADDR,INA228_REG_VBUS);
	next.vshunt_b=ina228_read24(INA228_B_ADDR,INA228_REG_VSHUNT);

	next.dietemp_a=ina228_read16(INA228_A_ADDR,INA228_REG_DIETEMP);
	next.dietemp_b=ina228_read16(INA228_B_ADDR,INA228_REG_DIETEMP);
}


void periodic_millisecond(void) {
	timers.now_millisecond=0;

	/* LED control */
	if ( 0==timers.led_on_a ) {
		output_low(LED_A);
	} else {
		output_high(LED_A);
		timers.led_on_a--;
	}


}

int8 get_ack_status(int8 address) {
	int8 status;

	i2c_start(STREAM_MASTER);
	status = i2c_write(STREAM_MASTER,address);  // Status = 0 if got an ACK
	i2c_stop(STREAM_MASTER);

	if ( 0 == status )
		return(TRUE);

   return(FALSE);
}


void main(void) {
	int8 i;

	init();



	/* flash on startup */
	for ( i=0 ; i<5 ; i++ ) {
		restart_wdt();
		output_high(LED_A);
		delay_ms(200);
		output_low(LED_A);
		delay_ms(200);
	}

	delay_ms(1000);

	fprintf(STREAM_FTDI,"# dcswc_module_voltage_current_counter %s\r\n",__DATE__);

	delay_ms(1000);

	timers.led_on_a=500;

	enable_interrupts(GLOBAL);

	/* enable I2C slave interrupt */
	enable_interrupts(INT_SSP);

	for ( ; ; ) {
		restart_wdt();

		/* query INA228's for next */
		if ( timers.now_ina ) {
			action_now_ina();
		}

		/* copy next to current */
		if ( timers.now_strobe ) {
			action_now_strobe();
		}

		if ( timers.now_millisecond ) {
			periodic_millisecond();
		}

		if ( kbhit() ) {
			getc();

			fprintf(STREAM_FTDI,"# DIP SWITCHES: %d\r\n",
				read_dip_switch()
			);

			fprintf(STREAM_FTDI,"# A: 0x%08lx / 0x%08lx / 0x%04lu\r\n",
				current.vbus_a,
				current.vshunt_a,
				current.dietemp_a
			);
			fprintf(STREAM_FTDI,"# B: 0x%08lx / 0x%08lx / 0x%04lu\r\n",
				current.vbus_b,
				current.vshunt_b,
				current.dietemp_b
			);

			fprintf(STREAM_FTDI,"# current.count_a_last_second=%lu\r\n",
				current.count_a_last_second
			);
			fprintf(STREAM_FTDI,"# current.count_b_last_second=%lu\r\n",
				current.count_b_last_second
			);
			fprintf(STREAM_FTDI,"# current.count_a_long=%lu\r\n",
				current.count_a_long
			);
			fprintf(STREAM_FTDI,"# current.count_b_long=%lu\r\n",
				current.count_b_long
			);

			fprintf(STREAM_FTDI,"# current.count_seconds_long=%lu\r\n",
				current.count_seconds_long
			);

		}


	}


}