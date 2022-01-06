#include "dcswc_module_voltage_current_counter.h"


typedef struct {
	int8 serial_prefix;
	int16 serial_number;
	int16 startup_power_on_delay;
} struct_config;



typedef struct {
	/* circular buffer for ADC readings */
	int16 sequence_number;
	int16 uptime_minutes;
	int16 interval_milliseconds;

	int8 factory_unlocked;


	int8 compile_year;
	int8 compile_month;
	int8 compile_day;

	int8 default_params_written;
} struct_current;

typedef struct {
	/* action flags */
	int1 now_millisecond;

	int1 now_write_config;
	int1 now_reset_config;

	/* timers */
	int8 led_on_a;

} struct_time_keep;

/* global structures */
struct_config config={0};
struct_current current={0};
struct_time_keep timers={0};

#include "param_dcswc_module_voltage_current_counter.c"
#include "i2c_handler_dcswc_module_voltage_current_counter.c"
#include "interrupt_dcswc_module_voltage_current_counter.c"

void init(void) {
	int8 buff[32];
//	setup_oscillator(OSC_16MHZ);

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

	set_tris_c    (0b11110000);
//                   76543210


//                   76543210

	/* data structure initialization */
	/* all initialized to 0 on declaration. Just do this if need non-zero */

	/* get our compiled date from constant */
	strcpy(buff,__DATE__);
	current.compile_day =(buff[0]-'0')*10;
	current.compile_day+=(buff[1]-'0');
	/* determine month ... how annoying */
	if ( 'J'==buff[3] ) {
		if ( 'A'==buff[4] )
			current.compile_month=1;
		else if ( 'N'==buff[5] )
			current.compile_month=6;
		else
			current.compile_month=7;
	} else if ( 'A'==buff[3] ) {
		if ( 'P'==buff[4] )
			current.compile_month=4;
		else
			current.compile_month=8;
	} else if ( 'M'==buff[3] ) {
		if ( 'R'==buff[5] )
			current.compile_month=3;
		else
			current.compile_month=5;
	} else if ( 'F'==buff[3] ) {
		current.compile_month=2;
	} else if ( 'S'==buff[3] ) {
		current.compile_month=9;
	} else if ( 'O'==buff[3] ) {
		current.compile_month=10;
	} else if ( 'N'==buff[3] ) {
		current.compile_month=11;
	} else if ( 'D'==buff[3] ) {
		current.compile_month=12;
	} else {
		/* error parsing, shouldn't happen */
		current.compile_month=255;
	}
	current.compile_year =(buff[7]-'0')*10;
	current.compile_year+=(buff[8]-'0');


	/* one periodic interrupt @ 1mS. Generated from system 16 MHz clock */
	/* prescale=16, match=249, postscale=1. Match is 249 because when match occurs, one cycle is lost */
	setup_timer_2(T2_DIV_BY_16,249,1);

	enable_interrupts(INT_TIMER2);
}

int8 read_dip_switch(void) {
	int16 adc;

	set_adc_channel(9);
	delay_ms(1);
	adc=read_adc();

	/* (note that table is sorted by vout reading 
	SW3.1 (LSB) SW3.2 (MSB) VALUE ADC
    OFF         OFF         0     1023
	OFF         ON          2     682
    ON          OFF         1     511
	ON          ON          3     409
	*/

	return adc;

	if ( adc > (1023-64) )
		return 0;
	if ( adc > (682-64) )
		return 2;
	if ( adc > (511-64) )
		return 1;

	return 3;
}



void periodic_millisecond(void) {
	static int8 uptimeticks=0;
	static int16 ticks=0;


	timers.now_millisecond=0;


	/* LED control */
	if ( 0==timers.led_on_a ) {
		output_low(LED_A);
	} else {
		output_high(LED_A);
		timers.led_on_a--;
	}

	/* some other random stuff that we don't need to do every cycle in main */
	if ( current.interval_milliseconds < 65535 ) {
		current.interval_milliseconds++;
	}

	/* seconds */
	ticks++;
	if ( 1000 == ticks ) {
		ticks=0;

		
		/* uptime counter */
		uptimeTicks++;
		if ( 60 == uptimeTicks ) {
			uptimeTicks=0;
			if ( current.uptime_minutes < 65535 ) 
				current.uptime_minutes++;
		}
	}

}


void main(void) {
	int8 i;

	init();


	/* read parameters from EEPROM and write defaults if CRC doesn't match */
	read_param_file();

	if ( config.startup_power_on_delay > 100 )
		config.startup_power_on_delay=100;

	/* flash on startup */
	for ( i=0 ; i<config.startup_power_on_delay ; i++ ) {
		restart_wdt();
		output_high(LED_A);
		delay_ms(200);
		output_low(LED_A);
		delay_ms(200);
	}

	delay_ms(1000);

	fprintf(STREAM_FTDI,"# dcswc_module_voltage_current_counter\r\n");

	timers.led_on_a=500;

	enable_interrupts(GLOBAL);

	/* enable I2C slave interrupt */
	enable_interrupts(INT_SSP);

	for ( ; ; ) {
		restart_wdt();

		if ( timers.now_millisecond ) {
			periodic_millisecond();
		}

		if ( kbhit() ) {
			getc();
#if 0
			fprintf(STREAM_FTDI,"# read_dip_switch()=%u\r\n",read_dip_switch());
			fprintf(STREAM_FTDI,"#    vin adc=%lu\r\n",adc_get(0));
			fprintf(STREAM_FTDI,"#   temp adc=%lu\r\n",adc_get(1));
			fprintf(STREAM_FTDI,"# dip sw adc=%lu\r\n",adc_get(2));
#endif
		}

		if ( timers.now_write_config ) {
			timers.now_write_config=0;
			write_param_file();
		}
		if ( timers.now_reset_config ) {
			timers.now_reset_config=0;
			write_default_param_file();
		}


	}


}