/**
 * @file       Microcontroller.c
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

/* Includes ----------------------------------------------------------- */
#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>

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


/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
volatile static uint32_t millis_count = 0;
volatile static uint32_t micros_count = 0;
/* Private function prototypes ---------------------------------------- */
uint32_t digital_write(volatile uint8_t *register_name, uint8_t position, uint8_t state)
{
	if (state == 1)
	{
		*register_name = *register_name | (1 << position);  
	}
	else if (state == 0)
	{
		*register_name = *register_name & (~(1 << position)); 
	}
	return SUCCESS;
}

uint32_t digital_read(volatile uint8_t register_name, uint8_t position)
{
	return (register_name >> position) & 1;
}

uint32_t input_set(volatile uint8_t *register_ddrx, volatile uint8_t *register_port, uint8_t position)
{
	digital_write(register_ddrx,position,0);
	digital_write(register_port,position,1);
	return SUCCESS;
}

uint32_t output_set(volatile uint8_t *register_ddrx, volatile uint8_t *register_port, uint8_t position)
{
	digital_write(register_ddrx,position,0);
	digital_write(register_port,position,1);
	return SUCCESS;
}

uint32_t init_systick_ms()
{
	digital_write(&TCCR0A,WGM01,1);
	OCR0A = 124;
	TCCR0B = 0x02;
	sei();
	digital_write(&TIMSK0,OCIE0A,1);
	return SUCCESS;
}

uint32_t systick_ms()
{
	return millis_count;
}

uint32_t init_systick_us()
{
	TCCR2A = (1 << WGM21);
	OCR2A = 19;        
	TCCR2B = (1 << CS21);  
	TIMSK2 = (1 << OCIE2A); 
	return SUCCESS;
}

uint32_t systick_us()
{
	uint32_t us;
	cli();
	us = micros_count;
	sei();
	return us;
}

uint32_t swap_high_low(uint8_t* byte_swap)
{
	uint8_t high = (0xF0 & *byte_swap) >> 4;
	uint8_t low = (0x0F & *byte_swap) << 4;
	*byte_swap = high | low;
	return SUCCESS;
}
/* ISR section -------------------------------------------------------- */
ISR(TIMER0_COMPA_vect)
{
	millis_count +=1;
}

ISR(TIMER2_COMPA_vect)
{
	micros_count += 10; 
}
/* Function definitions ----------------------------------------------- */

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */