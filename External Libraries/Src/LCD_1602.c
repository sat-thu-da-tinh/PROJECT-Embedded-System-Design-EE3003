/**
 * @file       LCD_1602.c
 * @version    0.0.1
 * @date       2025-10-17
 * @author     Truong Tan Dat
 *
 * @brief      LCD 1602 library in 4-bit mode for microcontrollers.
 *             Provides functions to initialize the LCD, send commands, write data,
 *             and display strings and numbers.
 *
 * @note       Designed for 16x2 LCDs in 4-bit mode. 
 *             Control pins (RS, RW, EN) and data pins (B4-B7) should be connected 
 *             to appropriate microcontroller ports as configured in DATA_PORT and CONTROL_PORT.
 *             Busy flag (BF) is not checked; timing relies on delays.
 *
 * @example    LCD_init();                    // Initialize the LCD
 *             LCD_set();                     // Configure 4-bit mode, 2 lines, display ON
 *             LCD_write_string("Hello",1,1); // Write string to row 1, column 1
 *             LCD_write_number(123,2,5);    // Write number to row 2, column 5
 */

#ifndef F_CPU 
#define F_CPU 8000000UL 
#endif 
/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Microcontroller.h"

/* Private defines ---------------------------------------------------- */
#define ASSERT(_expr_, _rc_) \
    do                       \
    {                        \
        if (_expr_)          \
            return _rc_;     \
    } while(0) 

#define CHECK_ERROR(_expr_, _error_code_)              \
    do                                                 \
    {                                                  \
        if (_expr_)                                    \
            printf("Error code: %d\n", _error_code_);  \
    } while(0) 

#define SUCCESS (0)
#define INVALID_ROW (-1) 
#define INVALID_COL (-2) 
#define DATA_PORT (PORTB)
#define DATA_DR (DDRB)
#define CONTROL_PORT (PORTD)
#define CONTROL_DR (DDRD)
#define RS (7)
#define RW (6)
#define EN (5)


/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

uint32_t LCD_send_command_4bits(uint8_t cmd)
{
	uint8_t high = (0xF0 & cmd);
	uint8_t low = (0x0F & cmd) << 4;
	
	CONTROL_PORT = CONTROL_PORT & (~(1 << RS));
	CONTROL_PORT = CONTROL_PORT & (~(1 << RW));
	DATA_PORT = (DATA_PORT & 0x0F) | high;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	DATA_PORT = (DATA_PORT & 0x0F) | low;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	return SUCCESS;
}

uint32_t LCD_send_data_4bits(uint8_t data)
{
	uint8_t high = (0xF0 & data);
	uint8_t low = (0x0F & data) << 4;
	
	CONTROL_PORT = CONTROL_PORT | (1 << RS);
	CONTROL_PORT = CONTROL_PORT & (~(1 << RW));
	DATA_PORT = (DATA_PORT & 0x0F) | high;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));

	_delay_us(100);
	DATA_PORT = (DATA_PORT & 0x0F) | low;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	return SUCCESS;
}

uint32_t LCD_init()
{
	for (int i = 4; i < 8; i++)
	{
		DATA_DR = DATA_DR | (1 << i);
	}
	CONTROL_DR = CONTROL_DR | (1 << RS);
	CONTROL_DR = CONTROL_DR | (1 << RW);
	CONTROL_DR = CONTROL_DR | (1 << EN);
	
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	CONTROL_PORT = CONTROL_PORT & (~(1 << RW));
	CONTROL_PORT = CONTROL_PORT & (~(1 << RS)); 
	
	_delay_ms(15);
	DATA_PORT = (DATA_PORT & 0x0F) | 0x30;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_ms(4.1);
	DATA_PORT = (DATA_PORT & 0x0F) | 0x30;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	DATA_PORT = (DATA_PORT & 0x0F) | 0x30;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	DATA_PORT = (DATA_PORT & 0x0F) | 0x20;
	CONTROL_PORT = CONTROL_PORT | (1 << EN);
	CONTROL_PORT = CONTROL_PORT & (~(1 << EN));
	_delay_us(100);
	return SUCCESS;
}
uint32_t LCD_set()
{
	LCD_send_command_4bits(0x28);
	LCD_send_command_4bits(0x01);
	_delay_ms(2);
	LCD_send_command_4bits(0x0C);
	LCD_send_command_4bits(0x06);
	return SUCCESS;
}

uint32_t LCD_write_string(char *str, uint8_t row, uint8_t col)
{
	ASSERT(((col < 1) | (col > 16)),INVALID_COL);
	switch (row)
	{
		case 1:
		LCD_send_command_4bits(0x80 + col - 1);
		break;
		case 2:
		LCD_send_command_4bits(0xC0 + col - 1);
		break;
		default:
		return INVALID_ROW;
	}
	while(*str)
	{
		LCD_send_data_4bits(*str++);
	}
	return SUCCESS;
}

uint32_t LCD_write_number(int number, uint8_t row, uint8_t col)
{
	ASSERT(((col < 1) | (col > 16)),INVALID_COL);
	switch (row)
	{
		case 1:
		LCD_send_command_4bits(0x80 + col - 1);
		break;
		case 2:
		LCD_send_command_4bits(0xC0 + col - 1);
		break;
		default:
		return INVALID_ROW;
	}
	if(number == 0)
	{
		LCD_send_data_4bits(48);
		return SUCCESS;
	}
	uint8_t i = 0;
	uint8_t arr[10] = {};
	if (number < 0)
	{
		number = -number;
		LCD_send_data_4bits('-');
	}
	while(number)
	{
		arr[i] = number % 10 + 48;
		number = number / 10;
		i++;
	}
	for (int j = i - 1; j >= 0; j--)
	{
		LCD_send_data_4bits(arr[j]);
	}
	return SUCCESS;
}
/* ISR section -------------------------------------------------------- */

/* Function definitions ----------------------------------------------- */

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */