
/**
 * @file       EEPROOM.h
 * @version    0.0.0
 * @date       yyyy-mm-dd
 * @author     
 *
 * @brief      <A brief description of the content of the file>
 *
 * @note       none
 *
 * @example    none
 *
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef EEPROOM_H_
#define EEPROOM_H_

/* Includes ----------------------------------------------------------- */
/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <util/delay.h>
#include "Microcontroller.h"
#include "I2C_Master.h"
#include <stdio.h>
#include <avr/eeprom.h>

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
#define DATA_EXIST (5)
#define EEPROM_SIZE (1024)
#define DATA_NOT_EXIST (6)

extern uint16_t eeprom_address;
/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
uint32_t write_eeprom(uint16_t addr, uint8_t *data);
uint32_t read_eeprom(uint8_t *buffer, uint8_t *state);
void show_all_eeprom();
void clear_all_eeprom();
void reset_all_states();

#endif /* EEPROOM_H_ */

/* End of file -------------------------------------------------------- */









