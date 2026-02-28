/**
 * @file       EEPROM.c
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
#include "EEPROOM.h"
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



/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

void show_all_eeprom()
{
	uint8_t last_addr = eeprom_read_byte((uint8_t*)0x00);
	if (last_addr == 0xFF || last_addr < 0x01)
	{
		LCD_I2C_write_string("EEPROM trong", 1, 1);
		_delay_ms(500);
		LCD_Clear();
		return;
	}

	LCD_Clear();
	LCD_I2C_write_string("EEPROM data:", 1, 1);
	_delay_ms(1000);
	LCD_Clear();

	char line[17];
	uint8_t index = 1;

	for (uint16_t addr = 0x01; addr < last_addr; addr += 5)
	{
		uint8_t b0 = eeprom_read_byte((uint8_t*)(addr + 0));
		uint8_t b1 = eeprom_read_byte((uint8_t*)(addr + 1));
		uint8_t b2 = eeprom_read_byte((uint8_t*)(addr + 2));
		uint8_t b3 = eeprom_read_byte((uint8_t*)(addr + 3));
		uint8_t b4 = eeprom_read_byte((uint8_t*)(addr + 4));

		snprintf(line, sizeof(line), "%02u:%02X %02X %02X %02X %02X", index, b0, b1, b2, b3, b4);
		LCD_I2C_write_string(line, 1, 1);

		_delay_ms(500);
		LCD_Clear();

		index++;
	}

	LCD_I2C_write_string("Het du lieu", 1, 1);
	_delay_ms(400);
	LCD_Clear();
}

uint32_t read_eeprom(uint8_t *buffer, uint8_t *state)
{
	for (uint16_t i = 0x01; i < eeprom_address; i += 5)
	{
		uint8_t cnt = 0;
		for (uint8_t j = 0; j < 4; j++) {
			uint8_t tmp = eeprom_read_byte((uint8_t*)(i + j));
			if (buffer[j] == tmp) cnt++;
		}
		if (cnt == 4)
		{
			*state = eeprom_read_byte((uint8_t*)(i + 4));
			uint8_t new_state = !(*state);
			eeprom_write_byte((uint8_t*)(i + 4), new_state);
			return 0;
		}
	}
	return 1;
}

uint32_t write_eeprom(uint16_t addr, uint8_t *data)
{
	uint8_t dummy_state = 0;
	ASSERT(read_eeprom(data,&dummy_state) == 0, DATA_EXIST);
	read_eeprom(data,&dummy_state);
	for (uint8_t i = 0; i < 4; i++) {
		eeprom_write_byte((uint8_t*)(addr + i), data[i]);
	}
	eeprom_write_byte((uint8_t*)(addr + 4), 0);
	eeprom_address += 5;
	eeprom_write_byte((uint8_t*)0x00, (uint8_t)eeprom_address);
	return SUCCESS;
}


void clear_all_eeprom()
{
	LCD_Clear();
	LCD_I2C_write_string("Dang xoa EEPROM...", 1, 1);

	for (uint16_t addr = 0; addr < EEPROM_SIZE; addr++)
	{
		eeprom_write_byte((uint8_t*)addr, 0xFF);
		while (!eeprom_is_ready());
	}

	eeprom_write_byte((uint8_t*)0x00, 0x01);
	eeprom_address = 0x01;

	LCD_Clear();
	LCD_I2C_write_string("Da xoa EEPROM", 1, 1);
	_delay_ms(500);
	show_all_eeprom();
	LCD_Clear();
}




/* ISR section -------------------------------------------------------- */

/* Function definitions ----------------------------------------------- */

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */

