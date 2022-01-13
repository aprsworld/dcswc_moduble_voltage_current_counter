#include "registers_dcswc_module_voltage_current_counter.h"

/* test scratchpad registers for validating I2C communications */
//int16 rtest[8] = { 0x0001, 0x0203, 0x0405, 0x0607, 0x0809, 0x0a0b, 0x0c0d, 0x0e0f };


void write_i2c(int8 address, int16 value) {
	
	switch ( address ) {
#if 0
		case 0: rtest[0]=value; break;
		case 1: rtest[1]=value; break;
		case 2: rtest[2]=value; break;
		case 3: rtest[3]=value; break;
		case 4: rtest[4]=value; break;
		case 5: rtest[5]=value; break;
		case 6: rtest[6]=value; break;
		case 7: rtest[7]=value; break;
#endif

		/* write anything to either of these addresses and we reset the long counter */
		case I2C_REG_COUNT_LONG_SECONDS_MSW:
		case I2C_REG_COUNT_LONG_SECONDS_LSW:
			current.count_a_long=0;
			current.count_b_long=0;
			current.count_seconds_long=0;
			break;
		default:
			/* do nothing */
	}

}


int16 map_i2c(int8 addr) {
	static int16 lsw=0xffff;

	timers.led_on_a=100;





	switch ( addr ) {
#if 0
		case 0: return rtest[0];
		case 1: return rtest[1];
		case 2: return rtest[2];
		case 3: return rtest[3];
		case 4: return rtest[4];
		case 5: return rtest[5];
		case 6: return rtest[6];
		case 7: return rtest[7];
#endif

		/* 32 bit variables have the most significant word read first and that sets the
		   least sinificant word which can be read next. _LSW registers are only valid if
		   they are preceeded by a read on the matching _MSW register
		 */
		case I2C_REG_VBUS_A_MSW:
			lsw = make16(make8(current.vbus_a,1),make8(current.vbus_a,0));
			return (int16) make16(make8(current.vbus_a,3),make8(current.vbus_a,2));
		case I2C_REG_VBUS_A_LSW:
			return (int16) lsw;
		case I2C_REG_VSHUNT_A_MSW:
			timers.now_dump=1;
			lsw = make16(make8(current.vshunt_a,1),make8(current.vshunt_a,0));
			return (int16) make16(make8(current.vshunt_a,3),make8(current.vshunt_a,2));
		case I2C_REG_VSHUNT_A_LSW:
			return (int16) lsw;    

		case I2C_REG_VBUS_B_MSW:
			lsw = make16(make8(current.vbus_a,1),make8(current.vbus_a,0));
			return (int16) make16(make8(current.vbus_b,3),make8(current.vbus_b,2));
		case I2C_REG_VBUS_B_LSW:
			return (int16) lsw;
		case I2C_REG_VSHUNT_B_MSW:
			lsw = make16(make8(current.vshunt_b,1),make8(current.vshunt_b,0));
			return (int16) make16(make8(current.vshunt_b,3),make8(current.vshunt_b,2));
		case I2C_REG_VSHUNT_B_LSW:
			return (int16) lsw;    


		case I2C_REG_COUNT_A_LAST_SECOND:
			return (int16) current.count_a_last_second;
		case I2C_REG_COUNT_B_LAST_SECOND:
			return (int16) current.count_b_last_second;

		case I2C_REG_COUNT_A_LONG_MSW:
			lsw = make16(make8(current.count_a_long,1),make8(current.count_a_long,0));
			return (int16) make16(make8(current.count_a_long,3),make8(current.count_a_long,2));
		case I2C_REG_COUNT_A_LONG_LSW:
			return (int16) lsw;
		case I2C_REG_COUNT_B_LONG_MSW:
			lsw = make16(make8(current.count_b_long,1),make8(current.count_b_long,0));
			return (int16) make16(make8(current.count_b_long,3),make8(current.count_b_long,2));
		case I2C_REG_COUNT_B_LONG_LSW:
			return (int16) lsw;
		case I2C_REG_COUNT_LONG_SECONDS_MSW:
			lsw = make16(make8(current.count_seconds_long,1),make8(current.count_seconds_long,0));
			return (int16) make16(make8(current.count_seconds_long,3),make8(current.count_seconds_long,2));
		case I2C_REG_COUNT_LONG_SECONDS_LSW:
			return (int16) lsw;

		case I2C_REG_DIETEMP_A:
			return (int16) current.dietemp_a;
		case I2C_REG_DIETEMP_B:
			return (int16) current.dietemp_b;

		/* we should have range checked, and never gotten here ... or read unimplemented (future) register */
		default: return (int16) addr;
	}

}


