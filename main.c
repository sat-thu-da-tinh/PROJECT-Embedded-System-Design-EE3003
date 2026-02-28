#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "SPI_MASTER.h"
#include "I2C_Master.h"
#include "Microcontroller.h"
#include "EEPROOM.h"

#define CHOOSE_SERVO (PC3) //DIT ME, DOI DUOC
#define CHOOSE_SERVO_DR (DDRC)
#define CHOOSE_SERVO_PORT (PORTC) 
#define READ_STATE (PD2) //CHANGE TO SCAN AND SAVE NEW ID
#define LED_TEST1 (PC4)
#define LED_TEST2 (PC3)

#define BUZZER (PC2) //output
//#define TRIG (PD7)	 //delete
//#define ECHO (PB0)	 //delete

#define IN_SENSOR_1  (PD7) //IR SENSOR, SERVO IN
#define IN_SENSOR_2 (PD6) //IR SENSOR, SERVO OUT
#define IN_SERVO (1) 
#define OUT_SERVO (2)
#define Parked (2)

typedef enum {
	LCD_IDLE,
	LCD_SHOW_CONFIRM,
	LCD_UNINDENTIFIED_CARD,
	LCD_NEW_CARD_CONFIRM,
	LCD_CARD_INDENTITY_EXIST,
	LCD_CLEAR,
	LCD_PARK_AVAI,
	LCD_FULL
} lcd_state_t;

typedef enum {
	SERVO_IDLE,
	SERVO_IN_ON,
	SERVO_IN_OFF,
	SERVO_OUT_ON,
	SERVO_OUT_OFF,
	SERVO_CONFIRM
} servo_state_t;

typedef enum{
	HCSR04_IDLE,
	START_PULSE,
	HIGH_PULSE,
	LOW_PULSE,
	CHECK_ANY_VEHICLE
}HCSR04_state_t;

lcd_state_t lcd_state = LCD_IDLE;
uint32_t lcd_timer = 0;
uint32_t taken_spot = 0;
servo_state_t servo_in_state = SERVO_IDLE;
servo_state_t servo_out_state = SERVO_IDLE;
uint16_t eeprom_address = 0x01;
uint8_t data[4] = {};
	
void pwm_init(void)
{
	DDRB |= (1 << PB1); 

	TCCR1A = (1 << COM1A1) | (1 << WGM11);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);  // Prescaler = 8

	ICR1 = 39999;
}

void pwm_set_duty_ms(float pulse_ms)
{
	OCR1A = (uint16_t)(pulse_ms * 2000.0);
}

void pwm_set_angle(float angle)
{
	if (angle < 0) angle = 0;
	if (angle > 180) angle = 180;

	float pulse_ms = 1.0 + (angle / 180.0) * 1.0;  // 1 -> 2 ms
	pwm_set_duty_ms(pulse_ms);
}

	
	
void lcd_task()
{
	switch (lcd_state)
	{
		case LCD_IDLE:
		break;

		case LCD_NEW_CARD_CONFIRM:
		LCD_I2C_write_string("CARD DETECTED",1,1);
		break;
		
		case LCD_CARD_INDENTITY_EXIST:
		LCD_I2C_write_string("CARD EXIST",1,1);
		break;
		
		case LCD_UNINDENTIFIED_CARD:
		LCD_I2C_write_string("UNIDENTIFIED",1,1);
		break;
		
		case LCD_SHOW_CONFIRM:
		lcd_timer = systick_ms();
		lcd_state = LCD_CLEAR;
		break;
		
		case LCD_CLEAR:
		if (systick_ms() - lcd_timer >= 500)
		{
			lcd_timer = systick_ms();
			LCD_Clear();
			if(taken_spot > Parked)
			{
				lcd_state = LCD_FULL;
			}
			else
			{
				lcd_state = LCD_PARK_AVAI;
			}
		}
		break;
		
		case LCD_PARK_AVAI:
		if (systick_ms() - lcd_timer >= 2)
		{
			lcd_timer = systick_ms();
			LCD_I2C_write_string("Remaining park",1,1);
			LCD_I2C_write_number(Parked - taken_spot,2,1);
			lcd_state = LCD_IDLE;
		}
		break;
		
		case LCD_FULL:
		if (systick_ms() - lcd_timer >= 2)
		{
			lcd_timer = systick_ms();
			LCD_I2C_write_string("No space left",1,1);
			uint8_t dummy_state = 0;
			read_eeprom(data,&dummy_state);
			lcd_state = LCD_IDLE;
		}
		break;
		
		default:
		break;
	}
}


void servo_in_task()
{
	switch (servo_in_state)
	{
		case SERVO_IDLE:
		break;

		case SERVO_IN_ON:
		digital_write(&CHOOSE_SERVO_PORT,CHOOSE_SERVO,1);
		pwm_set_duty_ms(2.2);
		digital_write(&PORTC,LED_TEST1,1);
		servo_in_state = SERVO_CONFIRM;
		break;
		
		case SERVO_CONFIRM:
		if(digital_read(PIND,IN_SENSOR_1) == 0)   //xai cam bien chuyen ve 1
		{
			servo_in_state = SERVO_IN_OFF;
		}
		break;

		case SERVO_IN_OFF:
		digital_write(&CHOOSE_SERVO_PORT,CHOOSE_SERVO,1);
		pwm_set_duty_ms(1);
		servo_in_state = SERVO_IDLE;
		break;
		
		default:
		break;
	}
}

void servo_out_task()
{
	switch (servo_out_state)
	{
		case SERVO_IDLE:
		break;
		
		case SERVO_OUT_ON:
		digital_write(&CHOOSE_SERVO_PORT,CHOOSE_SERVO,0);
		pwm_set_duty_ms(2);
		servo_out_state = SERVO_CONFIRM;
		break;
		
		case SERVO_CONFIRM:
		if(digital_read(PIND,IN_SENSOR_2) == 0)   //xai cam bien chuyen ve 1
		{
			servo_out_state = SERVO_OUT_OFF;
		}
		break;

		case SERVO_OUT_OFF:
		digital_write(&CHOOSE_SERVO_PORT,CHOOSE_SERVO,0);
		pwm_set_duty_ms(1);
		servo_out_state = SERVO_IDLE;
		break;
		
		default:
		break;
	}
}

int main(void)
{
	//------------------------------------------
	//initialize
	LCD_I2C_init();
	LCD_I2C_set();
	SPI_master_init();
	RFID_hard_reset();
	rfid_init();
	init_systick_us();
	init_systick_ms();
	eeprom_address = eeprom_read_byte((uint8_t*)0x00);
	if (eeprom_address == 0xFF) eeprom_address = 0x01;

	CHOOSE_SERVO_DR = CHOOSE_SERVO_DR | (1 << CHOOSE_SERVO);
	DDRD = DDRD & (~(1 << READ_STATE));
	DDRD = DDRD & (~(1 << IN_SENSOR_1)) & (~(1 << IN_SENSOR_2));	
	DDRC |= (1 << BUZZER);
	
	//------------------------------------------
	uint32_t rfid_time = systick_ms();
	uint32_t write_eeproom_time = systick_ms();
	uint32_t count_tag = 0;
	uint8_t check_state = 0;
	pwm_init();
	clear_all_eeprom();
	while (1)
	{
		lcd_task();
		servo_in_task();
		servo_out_task();

		if(digital_read(PIND,READ_STATE) == 0)
		{
			if(systick_ms() - write_eeproom_time > 2026)
			{
				write_eeproom_time = systick_ms();
				if(READ_UID_BCC_CMD(data) == SUCCESS)
				{
					uint8_t check_exist = write_eeprom(eeprom_address,data);
					if(check_exist == 0)
					{
						lcd_state = LCD_NEW_CARD_CONFIRM;
						count_tag++;
					}
					else if(check_exist == DATA_EXIST)
					{
						lcd_state = LCD_CARD_INDENTITY_EXIST;
					}
				}
			}
		}
		else
		{
			if(systick_ms() - rfid_time >= 2026)
		{
			rfid_time = systick_ms();
			if(READ_UID_BCC_CMD(data) == SUCCESS)
			{
				if(read_eeprom(data,&check_state) == SUCCESS)
				{
					lcd_state = LCD_SHOW_CONFIRM;
					if(check_state == 0)
					{
						taken_spot++;
						if(taken_spot == (Parked + 1))
						{
							servo_in_state = SERVO_IDLE;
							servo_out_state = SERVO_IDLE;
							taken_spot--;
						}
						else
						{
							servo_in_state = SERVO_IN_ON;
						}
					}
					else if(check_state == 1)
					{
						servo_out_state = SERVO_OUT_ON;
						if (taken_spot == 0)
						{
							LCD_I2C_write_string("ERROR...",2,1);
							while(1)
							{
								
							}
						}
						taken_spot--;
					}
				}
				else
				{
					lcd_state = LCD_UNINDENTIFIED_CARD;
				}
			}
		}
		}
	}
}


