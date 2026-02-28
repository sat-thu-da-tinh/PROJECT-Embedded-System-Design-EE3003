/**
 * @file       LCD_1602.h
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
#ifndef __LCD_1602_H
#define __LCD_1602_H

/* Includes ----------------------------------------------------------- */
/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Microcontroller.h"

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
uint32_t LCD_send_command_4bits(uint8_t cmd);
uint32_t LCD_send_data_4bits(uint8_t data);
uint32_t LCD_init();
uint32_t LCD_set();
uint32_t LCD_write_number(int number, uint8_t row, uint8_t col);
uint32_t LCD_write_string(char *str, uint8_t row, uint8_t col);
#endif // __LCD_1602_H

/* End of file -------------------------------------------------------- */
