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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f0xx.h"
#include "high_score.h"
#include "fifo.h"
#include "tty.h"


#define EEPROM_PAGE_SIZE 32

// Clock definition

void internal_clock();

// I2C setup functions

void enable_i2c_ports();
void init_i2c();
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);
void i2c_stop();
void i2c_waitidle();
int i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size);
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size);
int i2c_checknack();
void i2c_clearnack();
void init_usart5();
void enable_tty_interrupt();

#endif /* _I2C_H_ */