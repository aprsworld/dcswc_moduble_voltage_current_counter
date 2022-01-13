#int_timer2
void isr_timer2() {
	static int16 ticks=0;
	static short count_a_last=0;
	static short count_b_last=0;
	short count_a_now;
	short count_b_now;


	/* external input counting */
	count_a_now=input(COUNT_A);
	if ( 0 == count_a_now && 1 == count_a_last ) {
		next.count_a_last_second++;
	}
	count_a_last=count_a_now;

	count_b_now=input(COUNT_B);
	if ( 0 == count_b_now && 1 == count_b_last ) {
		next.count_b_last_second++;
	}
	count_b_last=count_b_now;


	/* timing */

	timers.now_millisecond=1;

	ticks++;
	if ( 500 == ticks ) {
		timers.now_ina=1;
	} else if ( 1000 == ticks ) {
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

		ticks=0;
	}
}


/* I2C slave interrupt */
#byte SSPSTAT=GETENV("SFR:SSPSTAT")
#INT_SSP
void ssp_interrupt () {
	static int8 sstate;
	int8 state;
	int8 incoming;
	static int16 lastValue;
	static int8 lastMSB;

	static int8 address; 

	/* 
	our implementation of i2c_isr_state() that won't overflow and switch states
	but it will quit counting at 127 bytes. 
	If more bytes are needed, external counting variables can be used or the size of state can be made larger
	*/


	if ( ! bit_test(SSPSTAT,5) ) {
		/* address */
		sstate=0;

		if ( bit_test(SSPSTAT,2) ) {
			/* set high bit if read */
			bit_set(sstate,7);
		}
	} else {
		/* data */
	}

	/* state variable used below is not incremented */
	state=sstate;

	/* increment the state counter for next pass through unless it will overflow us into another state */
	if ( sstate != 0x7f && sstate != 0xff) {
		sstate++;
	}

	
	/* i2c_isr_state() return an 8 bit int
		0 - Address match received with R/W bit clear, perform i2c_read( ) to read the I2C address.
		1-0x7F - Master has written data; i2c_read() will immediately return the data
		0x80 - Address match received with R/W bit set; perform i2c_read( ) to read the I2C address,
		and use i2c_write( ) to pre-load the transmit buffer for the next transaction (next I2C read
		performed by master will read this byte).
		0x81-0xFF - Transmission completed and acknowledged; respond with i2c_write() to pre-load
		the transmit buffer for the next transition (the next I2C read performed by master will read this
		byte).
		Function:
		Returns the state of I2C communications in I2C slave mode after an SSP interrupt. The return
		value increments with each byte received or sent.
		If 0x00 or 0x80 is returned, an i2C_read( ) needs to be performed to read the I2C address that
		was sent (it will match the address configured by #USE I2C so this value can be ignored)
	*/

	if ( state <= 0x80 ) {                      
		/* I2C master is sending us data */
		if ( 0x80 == state ) {
			/* i2c_read(2) casues the function to read the SSPBUF without releasing the clock */
			incoming = i2c_read(STREAM_SLAVE,2);
		} else {
			incoming = i2c_read(STREAM_SLAVE);
		}

		if ( 1 == state ) {             
			address = incoming<<1;
		} else if ( state >= 2 && 0x80 != state ) {
			/* received byte is data */
		
			/* save MSB and we'll process it on next */
			if ( 2 == state ) {
				lastMSB=incoming;
			} else if ( 3 == state ) {
				/* 16 bit value made of previous byte and this byte */
				write_i2c(address>>1,make16(lastMSB,incoming));

				/* this write only works for a single register per I2C transaction */
				/* this is not a BUG, but it would need to be implemented if this functionality is needed */
			}
		}
	}


	if ( state >= 0x80 ) {
		/* I2C master is requesting data from us */

		if ( ! bit_test(address,0) ) {
			/* read 16 bit register (register address half of I2C address) on even address */
			lastValue=map_i2c(address>>1);

			/* send the MSB */
			i2c_write(STREAM_SLAVE,make8(lastValue,1));
		} else {
			/* send LSB of 16 bit register on odd address */
			i2c_write(STREAM_SLAVE,make8(lastValue,0));
		}
		address++;
	}
}
