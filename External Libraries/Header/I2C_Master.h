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

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __I2C_MASTER_H
#define __I2C_MASTER_H

/* Includes ----------------------------------------------------------- */
/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>

/* Public defines ----------------------------------------------------- */
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


/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
uint32_t Master_init();
uint32_t Master_transmitter_init(uint8_t slave_address_w, uint8_t data);
uint32_t Master_continue_transmit_data(uint8_t data);
uint32_t Master_terminate();

uint32_t LCD_I2C_init();
uint32_t LCD_I2C_set();
uint32_t LCD_I2C_write_string(char *str, uint8_t row, uint8_t col);
uint32_t LCD_I2C_write_number(long int number, uint8_t row, uint8_t col);
uint32_t LCD_Clear();

#endif // __I2C_MASTER_H

/* End of file -------------------------------------------------------- */
