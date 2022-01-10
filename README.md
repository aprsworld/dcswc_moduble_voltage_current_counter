# DCSWC Module, Voltage Current Counter
## 2 x voltage, 2 x shunt, 2 x counter

PIC18F14K22 microcontroller with 2 x INA228A voltage / shunt to I2C converters

## I2C Interface

The microcontroller acts as a I2C slave on the DCSWC bus. It is a (software) I2C master for communicating with the INA228A converts.

## Hardware Notes

### 10 position screw terminal 

Pin | Function | Note
---|---|---
1|VBUS\_A|VOLTAGE INPUT A
2|VIN+\_A|SHUNT + INPUT A
3|VIN-\_A|SHUNT - INPUT A
4|VBUS\_A|VOLTAGE INPUT B
5|VIN+\_A|SHUNT + INPUT B
6|VIN-\_A|SHUNT - INPUT B
7|5V OUT|5 VOLT OUTPUT FOR SENSOR EXCITATION
8|COUNT\_A|COUNTER INPUT A
9|COUNT\_B|COUNTER INPUT B
10|COM|COM
