/* 32 bit INA228 bus and shunt voltages */
#define I2C_REG_VBUS_A_MSW                    0
#define I2C_REG_VBUS_A_LSW                    1
#define I2C_REG_VSHUNT_A_MSW                  2
#define I2C_REG_VSHUNT_A_LSW                  3

#define I2C_REG_VBUS_B_MSW                    4
#define I2C_REG_VBUS_B_LSW                    5
#define I2C_REG_VSHUNT_B_MSW                  6
#define I2C_REG_VSHUNT_B_LSW                  7

/* 16 bit count of last (not current) second */
#define I2C_REG_COUNT_A_LAST_SECOND           8
#define I2C_REG_COUNT_B_LAST_SECOND           9

/* 32 bit count since reset */
#define I2C_REG_COUNT_A_LONG_MSW              10
#define I2C_REG_COUNT_A_LONG_LSW              11
#define I2C_REG_COUNT_B_LONG_MSW              12
#define I2C_REG_COUNT_B_LONG_LSW              13

/* 32 bit seconds since count reset */
#define I2C_REG_COUNT_LONG_SECONDS_MSW        14
#define I2C_REG_COUNT_LONG_SECONDS_LSW        15

/* 16 bit INA228 die temperatures */
#define I2C_REG_DIETEMP_A                     16
#define I2C_REG_DIETEMP_B                     17
