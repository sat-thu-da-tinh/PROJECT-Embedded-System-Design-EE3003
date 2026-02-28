/**
 * @file       SPI.MASTER.c
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
#include "I2C_Master.h"
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

#define SUCCESS                 (0)
#define TRANSFER_SPI_FAILED     (1)
#define RFID_MISC_ERR           (2)
#define RFID_INVALID_BYTE_ERR   (3)
#define LENGTH_ATQA_ERR         (4)
#define CONTENT_ATQA_ERR        (5)
#define LENGTH_UID_BCC_ERR      (6)
#define CONTENT_UID_BCC_ERR     (7)

#define ERROR_REG_BITMASK       (0x13)
#define CONTROL_REG_BITMASK     (0x07)

#define SPI_DR                  (DDRB)
#define SPI_PORT                (PORTB)
#define SS                      (2)
#define MOSI                    (3)
#define MISO                    (4)
#define SCK                     (5)

#define FIFO_DATA_REG           (0x09)
#define REQA_CMD                (0x26)
#define BIT_FRAMING_REG         (0x0D)
#define COMMAND_REG             (0x01)
#define TRANSCEIVE_CMD          (0x0C)
#define ERROR_REG               (0x06)
#define CONTROL_REG             (0x0C)
#define FIFO_LEVEL_REG          (0x0A)
#define TxModeReg               (0x12)
#define RxModeReg               (0x13)
#define ModWidthReg             (0x24)
#define TxASKReg                (0x15)
#define ModeReg                 (0x11)
#define TxControlReg            (0x14)
#define ComIrqReg               (0x04)
#define TModeReg                (0x2A)
#define TPrescalerReg           (0x2B)
#define TReloadRegH             (0x2C)
#define TReloadRegL             (0x2D)
#define ComIEnReg               (0x02)
#define DivIEnReg               (0x03)
#define DivIrqReg               (0x05)
#define ErrorReg                (0x06)
#define Status1Reg              (0x07)
#define Status2Reg              (0x08)
#define CollReg                 (0x0E)

#define RFID_RST_DDR            (DDRD)
#define RFID_RST_PORT           (PORTD)
#define RFID_RST_PIN            (PB0 )


/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
uint32_t SPI_master_init()
{
	
	SPI_DR = SPI_DR | (1 << SS) | (1 << MOSI) | (1 << SCK);
	SPI_DR = SPI_DR & (~(1 << MISO));
	SPI_PORT |= (1 << SS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1); // SPI clock = F_CPU/64

	SPSR = SPSR & (~(1 << SPI2X));
	return SUCCESS;
}

uint32_t SPI_master_transfer(uint8_t data, uint8_t *received_data)
{
	SPI_PORT = SPI_PORT & (~(1 << SS));
	SPDR = data;
	uint32_t timeout = 100000;
	while (timeout--)
	{
		if ((SPSR >> SPIF) & 1)
		{
			*received_data = SPDR;
			break;
		}
	}
	ASSERT(timeout == 0, TRANSFER_SPI_FAILED); 
	SPI_PORT = SPI_PORT | (1 << SS);  
	return SUCCESS;
}

uint32_t SPI_writeReg(uint8_t regAddr, uint8_t data)
{
	uint8_t received_data = 0;
	SPI_PORT = SPI_PORT & (~(1 << SS));
	SPDR = regAddr;
	uint32_t timeout = 100000;
	while (timeout--)
	{
		if ((SPSR >> SPIF) & 1)
		{
			received_data = SPDR;
			break;
		}
	}
	ASSERT(timeout == 0, TRANSFER_SPI_FAILED); 
	SPDR = data;
	timeout = 100000;
	while (timeout--)
	{
		if ((SPSR >> SPIF) & 1)
		{
			received_data = SPDR;
			break;
		}
	}
	ASSERT(timeout == 0, TRANSFER_SPI_FAILED); 
	SPI_PORT = SPI_PORT | (1 << SS);
	received_data = 0;			// SUCCESS
	return received_data;
}

uint32_t RFID_writeReg(uint8_t regAddr, uint8_t data)
{
	uint8_t formattedRegAddr = (regAddr << 1) & 0x7E;
	SPI_writeReg(formattedRegAddr,data);
	return SUCCESS;
}

uint32_t SPI_readReg(uint8_t regAddr)
{
	uint8_t received_data = 0;
	uint8_t data = 0;
	SPI_PORT = SPI_PORT & (~(1 << SS));
	SPDR = regAddr;
	uint32_t timeout = 100000;
	while (timeout--)
	{
		if ((SPSR >> SPIF) & 1)
		{
			received_data = SPDR;
			break;
		}
	}
	ASSERT(timeout == 0, TRANSFER_SPI_FAILED);
	SPDR = data;
	timeout = 100000;
	while (timeout--)
	{
		if ((SPSR >> SPIF) & 1)
		{
			received_data = SPDR;
			break;
		}
	}
	ASSERT(timeout == 0, TRANSFER_SPI_FAILED);
	SPI_PORT = SPI_PORT | (1 << SS);
	return received_data;
}

uint32_t RFID_readReg(uint8_t regAddr)
{
	uint8_t formattedRegAddr = (regAddr << 1) | 0x80;
	uint8_t received_data = SPI_readReg(formattedRegAddr);
	return received_data;
}

void RFID_hard_reset()
{
	RFID_RST_DDR |= (1 << RFID_RST_PIN);
	RFID_RST_PORT &= ~(1 << RFID_RST_PIN); 
	_delay_ms(5);
	RFID_RST_PORT |= (1 << RFID_RST_PIN);  
	_delay_ms(50);                          
}

void rfid_init(void)
{
	RFID_writeReg(COMMAND_REG, 0x0F); 
	_delay_ms(50);

	RFID_writeReg(TModeReg, 0x80);       
	RFID_writeReg(TPrescalerReg, 0xA9);  
	RFID_writeReg(TReloadRegH, 0x03);
	RFID_writeReg(TReloadRegL, 0xE8);    

	RFID_writeReg(TxModeReg, 0x00);
	RFID_writeReg(RxModeReg, 0x00);
	RFID_writeReg(ModWidthReg, 0x26);
	RFID_writeReg(ModeReg, 0x3D);
	RFID_writeReg(TxASKReg, 0x40);       
	
	uint8_t value = RFID_readReg(TxControlReg);
	if (!(value & 0x03)) {
		RFID_writeReg(TxControlReg, value | 0x03);
	}
}

uint32_t READ_ATQA_CMD()
{
	RFID_writeReg(BIT_FRAMING_REG,0x07);
	RFID_writeReg(FIFO_DATA_REG,REQA_CMD);
	RFID_writeReg(COMMAND_REG, TRANSCEIVE_CMD);
	RFID_writeReg(BIT_FRAMING_REG,0x87);
	uint8_t irq = 0;
	uint16_t timeout = 50000;
	do {
		irq = RFID_readReg(ComIrqReg);
	} while ((!(irq & 0x20)) && (timeout--));
	uint8_t errorRegValue = RFID_readReg(ERROR_REG);
	uint8_t controlRegValue = RFID_readReg(CONTROL_REG);
	if (errorRegValue & ERROR_REG_BITMASK) return RFID_MISC_ERR;		if (controlRegValue & CONTROL_REG_BITMASK) return RFID_INVALID_BYTE_ERR;
	_delay_us(200);
	uint8_t numFIFObytes = RFID_readReg(FIFO_LEVEL_REG);  // <- LOI DANG O DAY
	ASSERT(numFIFObytes != 2, LENGTH_ATQA_ERR);
	uint8_t tagResponse[2] = {};
	for (int i = 0; i < 2; i++) {
		tagResponse[i] = RFID_readReg(FIFO_DATA_REG);
	}
	ASSERT((tagResponse[0] != 0x04) | (tagResponse[1] != 0x00), CONTENT_ATQA_ERR);
	return SUCCESS;
}

uint32_t READ_UID_BCC_CMD(uint8_t* uid_data)
{
	ASSERT(READ_ATQA_CMD() != 0, TRANSFER_SPI_FAILED);
	LCD_Clear();
	RFID_writeReg(BIT_FRAMING_REG,0x00);
	RFID_writeReg(FIFO_DATA_REG,0x93);
	RFID_writeReg(FIFO_DATA_REG,0x20);
	RFID_writeReg(BIT_FRAMING_REG,0x80);
	uint8_t irq = 0;
	uint16_t timeout = 50000;
	do {
		irq = RFID_readReg(ComIrqReg);
	} while ((!(irq & 0x20)) && (timeout--));

	
	uint8_t errorRegValue = RFID_readReg(ERROR_REG);
	uint8_t controlRegValue = RFID_readReg(CONTROL_REG);
	if (errorRegValue & ERROR_REG_BITMASK) return RFID_MISC_ERR;	if (controlRegValue & CONTROL_REG_BITMASK) return RFID_INVALID_BYTE_ERR;
	_delay_us(1000);
	
	uint8_t numFIFObytes = RFID_readReg(FIFO_LEVEL_REG);
	
	ASSERT(numFIFObytes != 5, LENGTH_UID_BCC_ERR);
	uint8_t tagResponse[5] = {};
	for (int i = 0; i < 5; i++) {
		tagResponse[i] = RFID_readReg(FIFO_DATA_REG);
	}
	uint8_t uid_check = tagResponse[0];
	for (int i = 1; i < 4; i++)
	{
		uid_check = uid_check^tagResponse[i];
	}
	ASSERT(uid_check != tagResponse[4], CONTENT_UID_BCC_ERR);

	for (int i = 0; i < 4; i++)
	{
		uid_data[i] = tagResponse[i];
		LCD_I2C_write_number(uid_data[i],2,1);
		_delay_ms(250);
		LCD_Clear();
	}

	return SUCCESS;
}

/* ISR section -------------------------------------------------------- */

/* Function definitions ----------------------------------------------- */

/* Private definitions ------------------------------------------------ */

/* End of file -------------------------------------------------------- */