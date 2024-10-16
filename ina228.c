#define INA228_REG_CONFIG       0x00  // 16 bits, configuration
#define INA228_REG_ADC_CONFIG   0x01  // 16 bits, ADC configuration
#define INA228_REG_SHUNT_CAL    0x02  // 16 bits, shunt calibration
#define INA228_REG_SHUNT_TEMPCO 0x03  // 16 bits, shunt temperature coefficient
#define INA228_REG_VSHUNT       0x04  // 24 bits, shunt voltage measurement
#define INA228_REG_VBUS         0x05  // 24 bits, bus voltage measurement
#define INA228_REG_DIETEMP      0x06  // 16 bits, temperature measurement
#define INA228_REG_CURRENT      0x07  // 24 bits, current result
#define INA228_REG_POWER        0x08  // 24 bits, power result 
#define INA228_REG_ENERGY       0x09  // 40 bits, energy result
#define INA228_REG_CHARGE       0x0a  // 40 bits, charge result
#define INA228_REG_DIAG_ALRT    0x0b  // 16 bits, diagnostic flags and alert
#define INA228_REG_SOVL         0x0c  // 16 bits, shunt overvoltage threshold
#define INA228_REG_SUVL         0x0d  // 16 bits, shunt undervoltage threshold
#define INA228_REG_BOVL         0x0e  // 16 bits, bus overvoltage threshold
#define INA228_REG_BUVL         0x0f  // 16 bits, bus undervoltage threshold
#define INA228_REG_TEMP_LIMIT   0x10  // 16 bits, temperature over-limit threshold
#define INA228_REG_PWR_LIMIT    0x11  // 16 bits, power over-limit threshold
#define INA228_REG_MFG_ID       0x3e  // 16 bits, manufacturer ID
#define INA228_REG_DEVICE_ID    0x3f  // 16 bits, device ID

#define INA228_I2C_WRITE        0x00
#define INA228_I2C_READ         0x01 


int16 ina228_read16(int8 i2c_address, int8 regaddr) {
	int16 data;

	i2c_start(STREAM_MASTER);
	delay_us(15);
	i2c_write(STREAM_MASTER,i2c_address);
	i2c_write(STREAM_MASTER,regaddr);
	i2c_start(STREAM_MASTER);
	delay_us(15);
	i2c_write(STREAM_MASTER,i2c_address | INA228_I2C_READ);  // read cycle
	data=make16(i2c_read(STREAM_MASTER,1),i2c_read(STREAM_MASTER,0));                                 
	i2c_stop(STREAM_MASTER);

	return data;
}

int32 ina228_read24(int8 i2c_address, int8 regaddr) {
	int32 data;

	i2c_start(STREAM_MASTER);
	delay_us(15);
	i2c_write(STREAM_MASTER,i2c_address);
	i2c_write(STREAM_MASTER,regaddr);
	i2c_start(STREAM_MASTER);
	delay_us(15);
	i2c_write(STREAM_MASTER,i2c_address | INA228_I2C_READ);  // read cycle
	data=make32(0,i2c_read(STREAM_MASTER,1),i2c_read(STREAM_MASTER,1),i2c_read(STREAM_MASTER,0));
	i2c_stop(STREAM_MASTER);

	return data;
}

void ina228_write16(int8 i2c_address, int8 regaddr, int16 value) {
	i2c_start(STREAM_MASTER);
	delay_us(15);                                                 
	i2c_write(STREAM_MASTER,i2c_address); // write cycle                       
	i2c_write(STREAM_MASTER,regaddr);  // write cycle         
	i2c_write(STREAM_MASTER,make8(value,1));
	i2c_write(STREAM_MASTER,make8(value,0));
	i2c_stop(STREAM_MASTER);
}

void ina228_init(int8 i2c_address) {
	/* 
	INA228_REG_CONFIG (from table 7-5 in INA228 datasheet
		bit[15]            RST      (R/W) Reset bit
		bit[14]            RSTACC   (R/W) Resets the contents of accumulation registers ENERGY and CHARGE to 0
		bit[13] to bit[ 6] CONDLY   (R/W) Sets the Delay for initial ADC conversion in steps of 2 ms
                                          0h = 0 s
                                          1h = 2 ms
                                          FFh = 510 ms
		bit[ 5]            TEMPCOMP (R/W) Enables temperature compensation of an external shunt 
		bit[ 4]            ADCRANGE (R/W) Shunt full scale range selection across IN+ and IN–
                                          0h = ±163.84 mV
                                          1h = ± 40.96 mV
		bit[ 3] to bit[ 0] RESERVED (R)   Reserved. Always reads 0
 	*/

	/* note that issuing a reset bit to INA228_REG_CONFIG will cause it to reset and apparently ignore the rest of the config */

	/* no resets, no conversion delay, no shunt temperature compensation, +-40.96mV range */
#if ADCRANGE == 1
	ina228_write16(i2c_address,INA228_REG_CONFIG,0b0000000000010000);
//                                                 5432109876543210
//                                                 1     0
#else
	ina228_write16(i2c_address,INA228_REG_CONFIG,0x00);
#endif

	/* continuous measurements, longest conversion time for voltages, shortest for temperature, 128 averaging window */
	ina228_write16(i2c_address,INA228_REG_ADC_CONFIG,0b1111111111000100);
//                                                     5432109876543210
//                                                     1     0
//	/* N experimentation */
//	ina228_write16(i2c_address,INA228_REG_ADC_CONFIG,0b1111111111000000);
//                                                     5432109876543210
//                                                     1     0

}