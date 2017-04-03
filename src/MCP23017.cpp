#include "MCP23017.h"

/**
 * @brief Set the characteristic of the IO pin
 *
 * @param pin Pin number
 * @param mode IO type (INPUT, INPUT_PULLUP, OUTPUT)
 */
void MCP23017::pinMode(uint8_t pin, uint8_t mode)
{
	if (mode == INPUT)
	{
		readModifyWritePin<IODIR>(pin, true);
		readModifyWritePin<GPPU>(pin, false);
	}
	else if (mode == INPUT_PULLUP)
	{
		readModifyWritePin<IODIR>(pin, true);
		readModifyWritePin<GPPU>(pin, true);
	}
	else if (mode == OUTPUT)
	{
		readModifyWritePin<IODIR>(pin, false);
		readModifyWritePin<GPPU>(pin, false);
	}
}

/**
 * @brief Set the state of an output pin
 *
 * @param pin Pin number
 * @param state State to set the pin (HIGH, LOW)
 */
void MCP23017::digitalWrite(uint8_t pin, bool state)
{
	readModifyWritePin<OLAT>(pin, state);
}

/**
 * @brief Read the state of an input pin
 *
 * @param pin Pin number
 *
 * @return State of the pin
 */
bool MCP23017::digitalRead(uint8_t pin)
{
	return readRegister(regAB(GPIO, pinToPort(pin))) & pinToMask(pin);
}

/**
 * @brief Set the characteristic of a port
 *
 * @param port Port to set (A, B)
 * @param mode IO type (INPUT, INPUT_PULLUP, OUTPUT)
 */
void MCP23017::portMode(MCP23017_Port_t port, uint8_t mode)
{
	if (mode == INPUT)
	{
		writeRegister(regAB(IODIR, port), 0xFF);
		writeRegister(regAB(GPPU, port), 0x00);
	}
	else if (mode == INPUT_PULLUP)
	{
		writeRegister(regAB(IODIR, port), 0xFF);
		writeRegister(regAB(GPPU, port), 0xFF);
	}
	else if (mode == OUTPUT)
	{
		writeRegister(regAB(IODIR, port), 0x00);
		writeRegister(regAB(GPPU, port), 0x00);
	}
}

/**
 * @brief Write a byte to an output port
 *
 * @param port Port to output to (A, B)
 * @param state Byte to write
 */
void MCP23017::writePort(MCP23017_Port_t port, uint8_t state)
{
	writeRegister(regAB(OLAT, port), state);
}

/**
 * @brief Read a byte from an input port
 *
 * @param port Port to read from (A, B)
 * @return data from the port
 */
uint8_t MCP23017::readPort(MCP23017_Port_t port)
{
	return readRegister(regAB(GPIO, port));
}

/**
 * @brief Set the characteristic of all pins on the chip
 *
 * @param mode IO type (INPUT, INPUT_PULLUP, OUTPUT)
 */
void MCP23017::chipMode(uint8_t mode)
{
	if (mode == INPUT)
	{
		writeRegister(IODIRA, 0xFF);
		writeRegister(IODIRB, 0xFF);
		writeRegister(GPPUA, 0x00);
		writeRegister(GPPUB, 0x00);
	}
	else if (mode == INPUT_PULLUP)
	{
		writeRegister(IODIRA, 0xFF);
		writeRegister(IODIRB, 0xFF);
		writeRegister(GPPUA, 0xFF);
		writeRegister(GPPUB, 0xFF);
	}
	else if (mode == OUTPUT)
	{
		writeRegister(IODIRA, 0x00);
		writeRegister(IODIRB, 0x00);
		writeRegister(GPPUA, 0x00);
		writeRegister(GPPUB, 0x00);
	}
}

/**
 * @brief Write a word to the chip
 *
 * @param state Word to write
 */
void MCP23017::writeChip(uint16_t state)
{
	writeRegister(OLATA, (uint8_t)(state & 0xFF));
	writeRegister(OLATB, (uint8_t)(state >> 8));
}

/**
 * @brief Read a word from a chip
 *
 * @return A word from the chip
 */
uint16_t MCP23017::readChip()
{
	uint16_t state;
	state = readRegister(GPIOA);
	state &= (readRegister(GPIOB) << 8);
	return state;
}

/**
 * @brief Sets the input polarity of the chip
 *
 * @param state true = all input pins inverted
 */
void MCP23017::setInputPolarity(bool state)
{
	// uint8_t buffer[2];
	// memset(buffer, state ? 0xFF : 0x00, 2);
	// writeRegisters(IPOLA, buffer, 2);
	writeRegister(IPOLA, state ? 0xFF : 0x00);
	writeRegister(IPOLB, state ? 0xFF : 0x00);
}

/**
 * @brief Set the input polarity of a port
 *
 * @param port Port to set (A, B)
 * @param state true = inverted
 */
void MCP23017::setInputPolarity(MCP23017_Port_t port, bool state)
{
	writeRegister(regAB(IPOL, port), state ? 0xFF : 0x00);
}

/**
 * @brief Set the input polarity of an individual pin
 *
 * @param pin Pin to set
 * @param state true = inverted
 */
void MCP23017::setInputPolarity(uint8_t pin, bool state)
{
	readModifyWritePin<IPOL>(pin, state);
}

/**
 * @brief Get the pin that caused an interrupt
 *
 * @return Pin number
 */
uint8_t MCP23017::getInterrupt()
{
	uint8_t tempA, tempB;
	tempA = readRegister(INTFA);
	tempB = readRegister(INTFB);

	if (tempA & 0x01) { return 0; }
	else if (tempA & 0x02) { return 1; }
	else if (tempA & 0x04) { return 2; }
	else if (tempA & 0x08) { return 3; }
	else if (tempA & 0x10) { return 4; }
	else if (tempA & 0x20) { return 5; }
	else if (tempA & 0x40) { return 6; }
	else if (tempA & 0x80) { return 7; }
	else if (tempB & 0x01) { return 8; }
	else if (tempB & 0x02) { return 9; }
	else if (tempB & 0x04) { return 10; }
	else if (tempB & 0x08) { return 11; }
	else if (tempB & 0x10) { return 12; }
	else if (tempB & 0x20) { return 13; }
	else if (tempB & 0x40) { return 14; }
	else if (tempB & 0x80) { return 15; }

	return 255;
}

/**
 * @brief Get a snapshot of all of the input pins at the last interrupt
 * 
 * @return Snapshot of the input register
 */
uint16_t MCP23017:: getInterruptCapture()
{
	uint8_t result[2];
	readRegisters(INTCAPA, result, 2);
	return result[0] + (result[1] << 8);
}

/**
 * @brief 
 * 
 * @param port [description]
 * 
 * @return [description]
 */
uint8_t MCP23017::getInterruptCapture(MCP23017_Port_t port)
{
	return readRegister(regAB(INTCAP, port));
}

/**
 * @brief Set if an input pin will trigger an interrupt on change
 *
 * @param pin Pin to enable or disable interrupt
 * @param state true = enable, false = disable
 */
void MCP23017::setInterrupt(uint8_t pin, bool state)
{
	readModifyWritePin<GPINTEN>(pin, state);
}

/**
 * @brief Directly set the interrupt mask for the whole chip
 *
 * @param mask 16 bit mask 1's = enable
 */
void MCP23017::setInterrupt(uint16_t mask)
{
	// uint8_t buffer[2];
	// buffer[0] = (uint8_t)(mask & 0xFF);
	// buffer[1] = (uint8_t)(mask >> 8);
	// writeRegisters(GPINTENA, buffer, 2);
	writeRegister(GPINTENA, (uint8_t)(mask & 0xFF));
	writeRegister(GPINTENB, (uint8_t)(mask >> 8));
}

/**
 * @brief Set the interrupt pins to mirror each other
 *
 * @param state true = both int pins mirror, false = int by port
 */
void MCP23017::interruptMirror(bool state)
{
	setRegisterBit(IOCONA, 6, state); // bit 6 is MIRROR
}

/**
 * @brief Set the electrical characteristic of the interrupt pins
 *
 * @param interruptPinMode (openDrain, lowOnInt, highOnInt)
 */
void MCP23017::setIntPinMode(MCP23017_interruptPinMode_t interruptPinMode)
{
	if (interruptPinMode == openDrain)
	{
		setRegisterBit(IOCONA, 2, true); // bit 2 is ODR
	}
	else if (interruptPinMode == lowOnInt)
	{
		setRegisterBit(IOCONA, 2, false); // bit 2 is ODR
		setRegisterBit(IOCONA, 1, false); // bit 1 is INTPOL
	}
	else if (interruptPinMode == highOnInt)
	{
		setRegisterBit(IOCONA, 2, false); // bit 2 is ODR
		setRegisterBit(IOCONA, 1, true); // bit 1 is INTPOL
	}
}

template<MCP23017_RegisterGeneric_t reg>
void MCP23017::readModifyWritePin(uint8_t pin, bool state)
{
	uint8_t tempReg;
	tempReg = readRegister(regAB(reg, pinToPort(pin)));
	if (state) { tempReg |= pinToMask(pin); }
	else { tempReg &= ~pinToMask(pin); }
	writeRegister(regAB(reg, pinToPort(pin)), tempReg);
}
