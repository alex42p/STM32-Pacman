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

void enable_i2c_ports();
void setup_i2c();
void i2c_start(uint8_t targadr, int size, bool dir);
void i2c_stop();
void i2c_idle();
int i2c_recvdata(); 
int i2c_checknack();
int i2c_clearnack();

#endif /* _I2C_H_ */