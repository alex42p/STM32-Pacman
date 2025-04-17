/**
 * 
 * @file: i2c.h
 * @author: Alex Piet
 * 
 * @description: 
 *      Holds declarations for all I2C functions.
 * 
 */

#ifndef _I2C_H_
#define _I2C_H_ 1

/**
 * TODO:
 * - Add all i2c function declarations to this file
 * - Add descriptions to i2c function declarations
 */

void internal_clock();
void enable_i2c_ports();
void init_i2c();
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);
void i2c_stop();
void i2c_waitidle();
int i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size);
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size);
int i2c_checknack();
void i2c_clearnack();

#endif /* _I2C_H_ */