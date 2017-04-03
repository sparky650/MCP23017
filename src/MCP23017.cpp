#include "MCP23017.h"

/**
 * @brief Set the characteristic of the IO pin
 *
 * @param pin Pin number
 * @param mode IO type (INPUT, INPUT_PULLUP, OUTPUT)
 */
void MCP23017::pinMode(uint8_t pin, uint8_t mode)
{
	uint8_t tempIODIR, tempGPPU, mask;

	tempIODIR = readRegister(regAB(IODIR, pinToPort(pin)));
	tempGPPU = readRegister(regAB(GPPU, pinToPort(pin)));
	mask = pinToMask(pin);

	if (mode == INPUT)
	{
		tempIODIR |= mask;
		tempGPPU &= ~mask;
	}
	else if (mode == INPUT_PULLUP)
	{
		tempIODIR |= mask;
		tempGPPU |= mask;
	}
	else if (mode == OUTPUT)
	{
		tempIODIR &= ~mask;
		tempGPPU &= ~mask;
	}

	writeRegister(regAB(IODIR, pinToPort(pin)), tempIODIR);
	writeRegister(regAB(GPPU, pinToPort(pin)), tempGPPU);
}

/**
 * @brief Set the state of an output pin
 *
 * @param pin Pin number
 * @param state State to set the pin (HIGH, LOW)
 */
void MCP23017::digitalWrite(uint8_t pin, bool state)
{
	readSetWritePin<OLAT>(pin, state);
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
	readSetWritePin<IPOL>(pin, state);
}

/**
 * @brief Set if an input pin will trigger an interrupt on change
 *
 * @param pin Pin to enable or disable interrupt
 * @param state true = enable, false = disable
 */
void MCP23017::setInterrupt(uint8_t pin, bool state)
{
	readSetWritePin<GPINTEN>(pin, state);
}

/**
 * @brief Directly set the interrupt mask for the whole chip
 *
 * @param mask 16 bit mask 1's = enable
 */
void MCP23017::setInterrupt(uint16_t mask)
{
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
void MCP23017::readSetWritePin(uint8_t pin, bool state)
{
	uint8_t tempReg;
	tempReg = readRegister(regAB(reg, pinToPort(pin)));
	if (state) { tempReg |= pinToMask(pin); }
	else { tempReg &= ~pinToMask(pin); }
	writeRegister(regAB(reg, pinToPort(pin)), tempReg);
}
