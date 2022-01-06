#include "registers_dcswc_module_voltage_current_counter.h"

void write_i2c(int8 address, int16 value) {
	switch ( address ) {
#if 0
		case I2C_REG_LED_A: 
			timers.led_on_a=make8(value,0);
			break;
		case I2C_REG_LED_B: 
			timers.led_on_b=make8(value,0);
			break;
		case I2C_REG_TIME_WATCHDOG_WRITE_SECONDS:
			timers.write_watchdog_seconds=0;
			break;
		case I2C_REG_COMMAND_OFF:
			timers.command_off_seconds=value;
			break;
		case I2C_REG_CONFIG_SERIAL_PREFIX: 
			if ( current.factory_unlocked && value >= 'A' && value <='Z' ) 
				config.serial_prefix=value;
			break;
		case I2C_REG_CONFIG_SERIAL_NUMBER:
			if (  current.factory_unlocked  ) {
				config.serial_number=value;
			}
			break;
		case I2C_REG_CONFIG_PARAM_WRITE:
			if ( 1 == value ) {
				timers.now_write_config=1;
			} else if ( 2 == value ) {
				timers.now_reset_config=1;
			} else if ( 1802 == value ) {
				current.factory_unlocked =1;
			} else if ( 65535 == value ) {
				reset_cpu();
			}
			break;
		case I2C_REG_CONFIG_TICKS_ADC:
			config.adc_sample_ticks=value;
			break;
		case I2C_REG_CONFIG_STARTUP_POWER_ON_DELAY:
			config.startup_power_on_delay=value;
			break;
		case I2C_REG_CONFIG_COMMAND_OFF_HOLD_TIME:
			config.command_off_hold_time=value;
			break;
		case I2C_REG_CONFIG_READ_WATCHDOG_OFF_THRESHOLD:
			config.read_watchdog_off_threshold=value;
			break;
		case I2C_REG_CONFIG_READ_WATCHDOG_OFF_HOLD_TIME:
			config.read_watchdog_off_hold_time=value;
			break;
		case I2C_REG_CONFIG_WRITE_WATCHDOG_OFF_THRESHOLD:
			config.write_watchdog_off_threshold=value;
			break;
		case I2C_REG_CONFIG_WRITE_WATCHDOG_OFF_HOLD_TIME:
			config.write_watchdog_off_hold_time=value;
			break;
		case I2C_REG_CONFIG_LVD_DISCONNECT_VOLTAGE:
			config.lvd_disconnect_adc=value;
			break;
		case I2C_REG_CONFIG_LVD_DISCONNECT_DELAY:
			config.lvd_disconnect_delay=value;
			break;
		case I2C_REG_CONFIG_LVD_RECONNECT_VOLTAGE:
			config.lvd_reconnect_adc=value;
			break;
		case I2C_REG_CONFIG_HVD_DISCONNECT_VOLTAGE:
			config.hvd_disconnect_adc=value;
			break;
		case I2C_REG_CONFIG_HVD_DISCONNECT_DELAY:
			config.hvd_disconnect_delay=value;
			break;
		case I2C_REG_CONFIG_HVD_RECONNECT_VOLTAGE:
			config.hvd_reconnect_adc=value;
			break;
#endif
		default:
			/* do nothing */
	}

}


int16 map_i2c(int8 addr) {

	timers.led_on_a=100;



	switch ( addr ) {
#if 0
		/* analog channels */
		/* input voltage */
		case I2C_REG_VOLTAGE_INPUT_NOW: 
			return (int16) current.adc_buffer[0][current.adc_buffer_index];
		case I2C_REG_VOLTAGE_INPUT_AVG: 
			return (int16) adc_get(0);

		/* temperature sensor */
		case I2C_REG_TEMPERATURE_BOARD_NOW: 
			return (int16) current.adc_buffer[1][current.adc_buffer_index];
		case I2C_REG_TEMPERATURE_BOARD_AVG: 
			return (int16) adc_get(1);
#endif

#if 0
		/* switch channels */
		case I2C_REG_SWITCH_MAGNET_NOW: 
			return (int16) ! input(SW_MAGNET);
		case I2C_REG_SWITCH_MAGNET_LATCH: 
			return (int16) current.latch_sw_magnet;
#endif

#if 0		
		/* status */
		case I2C_REG_SEQUENCE_NUMBER: 
			return (int16) current.sequence_number++;
		case I2C_REG_TIME_INTERVAL_MILLISECONDS: 
			return (int16) current.interval_milliseconds; /* milliseconds since last query */
		case I2C_REG_TIME_UPTIME_MINUTES: 
			return (int16) current.uptime_minutes; 
		case I2C_REG_TIME_WATCHDOG_READ_SECONDS: 
			return (int16) timers.read_watchdog_seconds; 
		case I2C_REG_TIME_WATCHDOG_WRITE_SECONDS: 
			return (int16) timers.write_watchdog_seconds;
		case I2C_REG_DEFAULT_PARAMS_WRITTEN:
			return (int16) current.default_params_written;
		case I2C_REG_COMMAND_OFF:
			return (int16) timers.command_off_seconds;
		case I2C_REG_POWER_OFF_FLAGS:
			return (int16) current.power_off_flags;


		/* configuration */
		case I2C_REG_CONFIG_SERIAL_PREFIX: 
			return (int16) config.serial_prefix;
		case I2C_REG_CONFIG_SERIAL_NUMBER: 
			return (int16) config.serial_number;
		case I2C_REG_CONFIG_HARDWARE_MODEL: 
			return (int16) 'P';
		case I2C_REG_CONFIG_HARDWARE_VERSION: 
			return (int16) 'Z';
		case I2C_REG_CONFIG_SOFTWARE_MODEL: 
			return (int16) 'P';
		case I2C_REG_CONFIG_SOFTWARE_VERSION: 
			return (int16) 3;
		case I2C_REG_CONFIG_SOFTWARE_YEAR:
			return (int16) current.compile_year;
		case I2C_REG_CONFIG_SOFTWARE_MONTH:
			return (int16) current.compile_month;
		case I2C_REG_CONFIG_SOFTWARE_DAY:
			return (int16) current.compile_day;

		case I2C_REG_CONFIG_PARAM_WRITE:
			/* 1 if factory unlocked */ 
			return (int16) current.factory_unlocked; 	
		case I2C_REG_CONFIG_TICKS_ADC: 
			return (int16) config.adc_sample_ticks;
		case I2C_REG_CONFIG_STARTUP_POWER_ON_DELAY: 
			return (int16) config.startup_power_on_delay;
		case I2C_REG_CONFIG_COMMAND_OFF_HOLD_TIME:
			return (int16) config.command_off_hold_time;
		case I2C_REG_CONFIG_READ_WATCHDOG_OFF_THRESHOLD:
			return (int16) config.read_watchdog_off_threshold;
		case I2C_REG_CONFIG_READ_WATCHDOG_OFF_HOLD_TIME:
			return (int16) config.read_watchdog_off_hold_time;
		case I2C_REG_CONFIG_WRITE_WATCHDOG_OFF_THRESHOLD:
			return (int16) config.write_watchdog_off_threshold;
		case I2C_REG_CONFIG_WRITE_WATCHDOG_OFF_HOLD_TIME:
			return (int16) config.write_watchdog_off_hold_time;
		case I2C_REG_CONFIG_LVD_DISCONNECT_VOLTAGE:
			return (int16) config.lvd_disconnect_adc;
		case I2C_REG_CONFIG_LVD_DISCONNECT_DELAY:
			return (int16) config.lvd_disconnect_delay;
		case I2C_REG_CONFIG_LVD_RECONNECT_VOLTAGE:
			return (int16) config.lvd_reconnect_adc;
		case I2C_REG_CONFIG_HVD_DISCONNECT_VOLTAGE:
			return (int16) config.hvd_disconnect_adc;
		case I2C_REG_CONFIG_HVD_DISCONNECT_DELAY:
			return (int16) config.hvd_disconnect_delay;
		case I2C_REG_CONFIG_HVD_RECONNECT_VOLTAGE:
			return (int16) config.hvd_reconnect_adc;
#endif

		/* we should have range checked, and never gotten here ... or read unimplemented (future) register */
		default: return (int16) addr;
	}

}


