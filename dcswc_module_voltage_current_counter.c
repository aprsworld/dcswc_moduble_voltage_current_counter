#include "dcswc_module_voltage_current_counter.h"

#define ADCRANGE 0

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

	int1 now_dump;   // debugging

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

int8 read_dip_switch(void) {
	/* nomenclature is backwards on netlist. We actually want LSB on top */
	return ( ! input(PIC_ADDR_LSB)<<1 ) | ( ! input(PIC_ADDR_MSB) );
}

void init(void) {
	setup_vref(VREF_OFF);
	setup_dac(DAC_OFF);
	setup_adc(ADC_OFF);
	setup_adc_ports(NO_ANALOGS);

	setup_wdt(WDT_512MS);


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

	/* set I2C slave address, which is always an even number */
	i2c_slaveaddr(0x36 + (read_dip_switch()<<1) );


	/* initialize ina228 chips */
	ina228_init(INA228_A_ADDR);
	ina228_init(INA228_B_ADDR);

}


void action_now_ina(void) {
	timers.now_ina=0;

	/* sample INA228 at middle of 1 second window */
	next.vbus_a  =ina228_read24(INA228_A_ADDR,INA228_REG_VBUS);
	next.vshunt_a=ina228_read24(INA228_A_ADDR,INA228_REG_VSHUNT);

	next.vbus_b  =ina228_read24(INA228_B_ADDR,INA228_REG_VBUS);
	next.vshunt_b=ina228_read24(INA228_B_ADDR,INA228_REG_VSHUNT);

	next.dietemp_a=ina228_read16(INA228_A_ADDR,INA228_REG_DIETEMP);
	next.dietemp_b=ina228_read16(INA228_B_ADDR,INA228_REG_DIETEMP);

#if ADCRANGE == 1 
	/* set low bit of high word (bit 24) to indicate we have +-40.96mV shunt range */
	bit_set(next.vshunt_a,24);
	bit_set(next.vshunt_b,24);
#else
	/* systems with +-163.84mV shunt range have bit 24 cleared. Or older firmwares don't implement this and that bit
	will be 0 by default */
	bit_clear(next.vshunt_a,24);
	bit_clear(next.vshunt_b,24);
#endif
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


void main(void) {
	int8 restart_cause;
	int8 i;

	restart_cause=restart_cause();

	init();



	/* flash on startup */
	for ( i=0 ; i<5 ; i++ ) {
		restart_wdt();
		output_high(LED_A);
		delay_ms(200);
		output_low(LED_A);
		delay_ms(200);
	}

	fprintf(STREAM_FTDI,"# dcswc_module_voltage_current_counter %s\r\n# ",__DATE__);
	switch ( restart_cause ) {
		case WDT_TIMEOUT:       fprintf(STREAM_FTDI,"WDT TIMEOUT"); break;
		case MCLR_FROM_SLEEP:   fprintf(STREAM_FTDI,"MCLR FROM SLEEP"); break;
		case MCLR_FROM_RUN:     fprintf(STREAM_FTDI,"MCLR FROM RUN"); break;
		case NORMAL_POWER_UP:   fprintf(STREAM_FTDI,"NORMAL POWER UP"); break;
		case BROWNOUT_RESTART:  fprintf(STREAM_FTDI,"BROWNOUT RESTART"); break;
		case WDT_FROM_SLEEP:    fprintf(STREAM_FTDI,"WDT FROM SLEEP"); break;
		case RESET_INSTRUCTION: fprintf(STREAM_FTDI,"RESET INSTRUCTION"); break;
		default:                fprintf(STREAM_FTDI,"UNKNOWN!");
	}
	fprintf(STREAM_FTDI,"\r\n");

	restart_wdt();
	fprintf(STREAM_FTDI,"# our I2C address=0x%02x\r\n",0x36 + (read_dip_switch()<<1));

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

		if ( timers.now_millisecond ) {
			periodic_millisecond();
		}

#if 0
		if ( timers.now_dump ) {
			timers.now_dump=0;

			fprintf(STREAM_FTDI,"# A: 0x%08lx / 0x%08lx / 0x%04lu\r\n",
				current.vbus_a,
				current.vshunt_a,
				current.dietemp_a
			);
		}
#endif

#if 0
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

			fprintf(STREAM_FTDI,"# input(PIC_ADDR_MSB)=%u\r\n",input(PIC_ADDR_MSB));
			fprintf(STREAM_FTDI,"# input(PIC_ADDR_LSB)=%u\r\n",input(PIC_ADDR_LSB));
		}
#endif


	}


}