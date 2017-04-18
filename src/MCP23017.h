/**
 * @file	MCP23017.h
 * @author	Keegan Morrow
 * @version 0.1.1
 * @brief Arduino library for the Microchip MCP23017 IO Expander
 *
 */

#ifndef __MCP23017_h_
#define __MCP23017_h_

#include <Arduino.h>
#include <Wire.h>
#include "utility/wireUtil.h"

enum MCP23017_Register_t
{
	IODIRA_r = 0x00,
	IODIRB_r = 0x01,
	IPOLA_r = 0x02,
	IPOLB_r = 0x03,
	GPINTENA_r = 0x04,
	GPINTENB_r = 0x05,
	DEFVALA_r = 0x06,
	DEFVALB_r = 0x07,
	INTCONA_r = 0x08,
	INTCONB_r = 0x09,
	IOCONA_r = 0x0A,
	IOCONB_r = 0x0B,
	GPPUA_r = 0x0C,
	GPPUB_r = 0x0D,
	INTFA_r = 0x0E,
	INTFB_r = 0x0F,
	INTCAPA_r = 0x10,
	INTCAPB_r = 0x11,
	GPIOA_r = 0x12,
	GPIOB_r = 0x13,
	OLATA_r = 0x14,
	OLATB_r = 0x15 
};

enum MCP23017_RegisterGeneric_t
{
	IODIR_r = 0x00,
	IPOL_r = 0x02,
	GPINTEN_r = 0x04,
	DEFVAL_r = 0x06,
	INTCON_r = 0x08,
	IOCON_r = 0x0A,
	GPPU_r = 0x0C,
	INTF_r = 0x0E,
	INTCAP_r = 0x10,
	GPIO_r = 0x12,
	OLAT_r = 0x14 
};

enum MCP23017_Port_t {PORT_A = 0x00, PORT_B = 0x01};

enum MCP23017_interruptPinMode_t {openDrain, lowOnInt, highOnInt};

static const uint8_t defaultAddress = 0x20;

class MCP23017: public wireUtil<MCP23017_Register_t, uint8_t>
{
public:
	using wireUtil::begin;

	MCP23017()
	{
		timeoutTime = 1000UL;
		timeoutFlag = false;
	}

	/**
	* @brief Initialize the chip at the default address
	*
	*/
	void begin() {begin(defaultAddress);}

	/**
	 * @brief Get the hardware address from the logical address of the chip
	 * 
	 * @param a Logical address of the chip
	 * @return Hardware address of the chip
	 */
	uint8_t addressIndex(uint8_t a) {return a + defaultAddress;}

	void pinMode(uint8_t, uint8_t);
	void digitalWrite(uint8_t, bool);
	bool digitalRead(uint8_t);

	void portMode(MCP23017_Port_t, uint8_t);
	void writePort(MCP23017_Port_t, uint8_t);
	uint8_t readPort(MCP23017_Port_t);

	void chipMode(uint8_t);
	void writeChip(uint16_t);
	uint16_t readChip();

	void setInputPolarity(bool);
	void setInputPolarity(MCP23017_Port_t, bool);
	void setInputPolarity(uint8_t, bool);

	uint8_t getInterrupt();
	uint16_t getInterruptCapture();
	uint8_t getInterruptCapture(MCP23017_Port_t);
	void setInterrupt(uint8_t, bool);
	void setInterrupt(MCP23017_Port_t, bool);
	void setInterrupt(uint16_t);
	void interruptMirror(bool);
	void setIntPinMode(MCP23017_interruptPinMode_t);

private:
	template<MCP23017_RegisterGeneric_t reg> void readModifyWritePin(uint8_t, bool);
	inline MCP23017_Port_t pinToPort(uint8_t pin) {return (pin < 8) ? PORT_A : PORT_B;}
	inline uint8_t pinToBit(uint8_t pin) {return pin % 8;}
	inline uint8_t pinToMask(uint8_t pin) {return 1 << (pin % 8);}
	inline MCP23017_Register_t regAB(MCP23017_RegisterGeneric_t regG, MCP23017_Port_t port)
	{return (MCP23017_Register_t)((uint8_t)regG + (uint8_t)port);}
};

#endif // __MCP23017_h_
