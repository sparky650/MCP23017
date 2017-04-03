/**
 * @file	MCP23017.h
 * @author	Keegan Morrow
 * @version 0.1.0
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
	IODIRA = 0x00,
	IODIRB = 0x01,
	IPOLA = 0x02,
	IPOLB = 0x03,
	GPINTENA = 0x04,
	GPINTENB = 0x05,
	DEFVALA = 0x06,
	DEFVALB = 0x07,
	INTCONA = 0x08,
	INTCONB = 0x09,
	IOCONA = 0x0A,
	IOCONB = 0x0B,
	GPPUA = 0x0C,
	GPPUB = 0x0D,
	INTFA = 0x0E,
	INTFB = 0x0F,
	INTCAPA = 0x10,
	INTCAPB = 0x11,
	GPIOA = 0x12,
	GPIOB = 0x13,
	OLATA = 0x14,
	OLATB = 0x15
};

enum MCP23017_RegisterGeneric_t
{
	IODIR = 0x00,
	IPOL = 0x02,
	GPINTEN = 0x04,
	DEFVAL = 0x06,
	INTCON = 0x08,
	IOCON = 0x0A,
	GPPU = 0x0C,
	INTF = 0x0E,
	INTCAP = 0x10,
	GPIO = 0x12,
	OLAT = 0x14
};

enum MCP23017_Port_t {A = 0x00, B = 0x01};

enum MCP23017_interruptPinMode_t {openDrain, lowOnInt, highOnInt};

static const uint8_t defaultAddress = 0x20;

class MCP23017: public wireUtil<MCP23017_Register_t>
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

	void setInterrupt(uint8_t, bool);
	void setInterrupt(uint16_t);
	void interruptMirror(bool);
	void setIntPinMode(MCP23017_interruptPinMode_t);

private:
	template<MCP23017_RegisterGeneric_t reg> void readSetWritePin(uint8_t, bool);
	template<MCP23017_Register_t reg> void readSetWriteBit(uint8_t, bool);
	inline MCP23017_Port_t pinToPort(uint8_t pin) {return (pin < 8) ? A : B;}
	inline uint8_t pinToBit(uint8_t pin) {return pin % 8;}
	inline uint8_t pinToMask(uint8_t pin) {return 1 << (pin % 8);}
	inline MCP23017_Register_t regAB(MCP23017_RegisterGeneric_t regG, MCP23017_Port_t port)
	{return (MCP23017_Register_t)((uint8_t)regG + (uint8_t)port);}
};

#endif // __MCP23017_h_
