/**
 * @file       Microcontroller.h
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
#ifndef __MICROCONTROLLER_H
#define __MICROCONTROLLER_H

/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>

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
uint32_t digital_write(volatile uint8_t *register_name, uint8_t position, uint8_t state);

#endif // __MICROCONTROLLER_H

/* End of file -------------------------------------------------------- */
