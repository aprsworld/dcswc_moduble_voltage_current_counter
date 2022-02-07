#include <18F14K22.h>
#device ADC=10
#device *=16
#use delay(clock=16MHz)

/* hardware I2C port is slave and is connected to DCSWC bus */
#use i2c(stream=STREAM_SLAVE,SLAVE, I2C1, FORCE_HW)
/* slave address set based on dip switch in init() */
/* Linux / i2cdetect will use the CCS address >>1. So 0x34 becomes 0x1a */



/* important FUSE notes! 
MPLAB defaults to DEBUG mode and that will override fuses.
make sure it is on "BUILD CONFIGURATION" of "RELEASE" under
"PROJECT" menu.

CCS setup functions can override fuses. Check .LST file
to see if anything has been overriden!
*/

#fuses NODEBUG 
#fuses INTRC_IO
#fuses NOPCLKEN
#fuses NOPLLEN
#fuses NOFCMEN
#fuses NOIESO
#fuses BROWNOUT
#fuses BORV30
#fuses NOPUT
#fuses WDT
#fuses WDT128     /* this can be override by setup_wdt() */
#fuses NOHFOFST
#fuses NOMCLR
#fuses STVREN
#fuses NOLVP
#fuses NOXINST
#fuses NODEBUG
#fuses NOPROTECT
#fuses NOWRT
#fuses NOWRTC 
#fuses NOWRTB
#fuses NOWRTD
#fuses NOEBTR
#fuses NOEBTRB
#fuses BBSIZ1K


#use standard_io(ALL)

#use rs232(UART1,stream=STREAM_FTDI,baud=9600,errors)	

/* program config CRC of 0 and a serial_prefix of 'A' ... that will trigger a write default on first boot */
#ROM 0xF00000 = { 0x00, 0x00, 0x40, 0x00 }



#define COUNT_B              PIN_C5
#define COUNT_A              PIN_C4
#define LED_A                PIN_C3
#define PIC_ADDR_MSB         PIN_C6
#define PIC_ADDR_LSB         PIN_C7
#define SER_TO_PC            PIN_B7

#define INA_ALERT            PIN_A2
#define I2C_SW_SDA           PIN_C0
#define I2C_SW_SCL           PIN_C1
#define TP2                  PIN_C2
#define I2C_SDA              PIN_B4
#define SER_FROM_PC          PIN_B5
#define I2C_SCL              PIN_B6


/* software I2C port is maser and is connected to two INA228A */
#use i2c(stream=STREAM_MASTER, MASTER, FAST, FORCE_SW, scl=I2C_SW_SCL, sda=I2C_SW_SDA)



#define INA228_A_ADDR      0x80
#define INA228_B_ADDR      0x9a

