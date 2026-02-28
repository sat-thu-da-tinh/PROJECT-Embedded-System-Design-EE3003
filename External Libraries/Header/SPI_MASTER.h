/**
 * @file       SPI_MASTER.h
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
#ifndef __SPI_MASTER_H
#define __SPI_MASTER_H

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
uint32_t SPI_master_init();
uint32_t SPI_master_transfer(uint8_t data, uint8_t *received_data);
uint32_t READ_UID_BCC_CMD(uint8_t* uid_data);
uint32_t READ_ATQA_CMD();
void rfid_init();
void RFID_hard_reset();
#endif // __SPI_MASTER_H

/* End of file -------------------------------------------------------- */
