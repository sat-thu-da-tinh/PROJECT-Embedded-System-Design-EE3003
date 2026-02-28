/**
 * @file       I2C_Master.h
 * @version    0.0.0
 * @date       yyyy-mm-dd
 * @author     Truong Tan Dat
 *
 * @brief      <A brief description of the content of the file>
 *
 * @note       none
 *
 * @example    none
 *
 */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif
/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
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

#define LCD_ADDRESS (0x27 << 1)
#define SUCCESS (0)    
#define ERROR_NACK_OR_ARBITRATION_LOST (-1)
#define INVALID_COL (-2)
#define INVALID_ROW (-3)
#define RS (0)
#define RW (1)
#define EN (2)
#define LCD_BACKLIGHT (0x08)

init_systick_ms();
init_systick_us();
/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
uint32_t Master_init()
{
	// I2C 100kHz
	TWSR = 0x00;                        
	TWBR = ((F_CPU / 100000UL) - 16) / 2;
	TWCR = (1 << TWEN);
	return SUCCESS;
}

uint32_t Transfer_polling(uint8_t value_check)
{
	uint32_t timeout = 100000;
	while (timeout--)
	{
		if ((TWCR >> TWINT) & 1)
		{
			uint8_t status = TWSR & 0xF8;
			ASSERT((status != value_check), ERROR_NACK_OR_ARBITRATION_LOST);
			return SUCCESS;
		}
	}
	return ERROR_NACK_OR_ARBITRATION_LOST;
}

uint32_t Master_START_init()
{
	// START condition
	TWCR = (1 << TWEN) | (1 << TWSTA) | (1 << TWINT);
	ASSERT((Transfer_polling(0x08) == ERROR_NACK_OR_ARBITRATION_LOST), ERROR_NACK_OR_ARBITRATION_LOST);
	return SUCCESS;
}

uint32_t Master_transmitter_init(uint8_t slave_address_w, uint8_t data)
{
	// Send SLA+W
	TWDR = slave_address_w;
	TWCR = (1 << TWEN) | (1 << TWINT);
	ASSERT((Transfer_polling(0x18) == ERROR_NACK_OR_ARBITRATION_LOST), ERROR_NACK_OR_ARBITRATION_LOST);

	// Send first data byte
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	ASSERT((Transfer_polling(0x28) == ERROR_NACK_OR_ARBITRATION_LOST), ERROR_NACK_OR_ARBITRATION_LOST);

	return SUCCESS;
}

uint32_t Master_continue_transmit_data(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWEN) | (1 << TWINT);
	ASSERT((Transfer_polling(0x28) == ERROR_NACK_OR_ARBITRATION_LOST), ERROR_NACK_OR_ARBITRATION_LOST);
	return SUCCESS;
}

uint32_t Master_terminate()
{
	// STOP condition
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO);
	_delay_us(10);
	return SUCCESS;
}

/* ----------------- LCD I2C DRIVER LAYER ----------------- */

uint32_t LCD_I2C_transmit_data(uint8_t data)
{
	uint8_t high = (0xF0 & data);
	uint8_t low = (0x0F & data) << 4;

	Master_START_init();

	// RS = 1, RW = 0
	high |= (1 << RS);
	high &= ~(1 << RW);
	low |= (1 << RS);
	low &= ~(1 << RW);

	Master_transmitter_init(LCD_ADDRESS, high | (1 << EN) | LCD_BACKLIGHT);
	Master_continue_transmit_data(high | LCD_BACKLIGHT);

	Master_continue_transmit_data(low | (1 << EN) | LCD_BACKLIGHT);
	Master_continue_transmit_data(low | LCD_BACKLIGHT);

	Master_terminate();
	return SUCCESS;
}

uint32_t LCD_I2C_transmit_cmd(uint8_t cmd)
{
	uint8_t high = (0xF0 & cmd);
	uint8_t low = (0x0F & cmd) << 4;

	Master_START_init();

	// RS = 0, RW = 0
	high &= ~((1 << RS) | (1 << RW));
	low &= ~((1 << RS) | (1 << RW));

	Master_transmitter_init(LCD_ADDRESS, high | (1 << EN) | LCD_BACKLIGHT);
	Master_continue_transmit_data(high | LCD_BACKLIGHT);
	Master_continue_transmit_data(low | (1 << EN) | LCD_BACKLIGHT);
	Master_continue_transmit_data(low | LCD_BACKLIGHT);

	Master_terminate();
	return SUCCESS;
}

uint32_t LCD_I2C_init()
{
	Master_init();
	Master_START_init();
	_delay_ms(15);

	Master_transmitter_init(LCD_ADDRESS,0x34);
	Master_continue_transmit_data(0x30);
	_delay_ms(5);
	Master_continue_transmit_data(0x34);
	Master_continue_transmit_data(0x30);
	_delay_us(150);
	Master_continue_transmit_data(0x34);
	Master_continue_transmit_data(0x30);
	_delay_us(150);
	Master_continue_transmit_data(0x24);
	Master_continue_transmit_data(0x20);; // 4-bit mode
	Master_terminate();
	_delay_us(150);

	return SUCCESS;
}

uint32_t LCD_I2C_set()
{
	LCD_I2C_transmit_cmd(0x28); // 4-bit, 2 lines, 5x8 font
	LCD_I2C_transmit_cmd(0x0C); // Display ON, cursor OFF
	LCD_I2C_transmit_cmd(0x01); // Clear display
	_delay_ms(2);
	LCD_I2C_transmit_cmd(0x06); // Entry mode set
	return SUCCESS;
}

uint32_t LCD_Clear()
{
	LCD_I2C_transmit_cmd(0x01);
	_delay_ms(2);
	return SUCCESS;
}

uint32_t LCD_I2C_write_string(char *str, uint8_t row, uint8_t col)
{
	ASSERT(((col < 1) | (col > 16)), INVALID_COL);

	switch (row)
	{
		case 1:
		LCD_I2C_transmit_cmd(0x80 + col - 1);
		break;
		case 2:
		LCD_I2C_transmit_cmd(0xC0 + col - 1);
		break;
		default:
		return INVALID_ROW;
	}

	while (*str)
	{
		LCD_I2C_transmit_data(*str++);
	}
	return SUCCESS;
}

uint32_t LCD_I2C_write_number(long int number, uint8_t row, uint8_t col)
{
	ASSERT(((col < 1) | (col > 16)), INVALID_COL);

	switch (row)
	{
		case 1:
		LCD_I2C_transmit_cmd(0x80 + col - 1);
		break;
		case 2:
		LCD_I2C_transmit_cmd(0xC0 + col - 1);
		break;
		default:
		return INVALID_ROW;
	}

	if (number == 0)
	{
		LCD_I2C_transmit_data('0');
		return SUCCESS;
	}

	uint8_t i = 0;
	uint8_t arr[10];

	if (number < 0)
	{
		number = -number;
		LCD_I2C_transmit_data('-');
	}

	while (number)
	{
		arr[i++] = (number % 10) + '0';
		number /= 10;
	}

	for (int j = i - 1; j >= 0; j--)
	{
		LCD_I2C_transmit_data(arr[j]);
	}

	return SUCCESS;
}

/* ISR section -------------------------------------------------------- */

/* Function definitions ----------------------------------------------- */

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */