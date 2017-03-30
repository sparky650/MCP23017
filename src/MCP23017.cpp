#include "MCP23017.h"

/**
 * @brief Set the charicteristic of the IO pin
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
 * @param state State to se the pin (HIGH, LOW)
 */
void MCP23017::digitalWrite(uint8_t pin, bool state)
{
	uint8_t tempOLAT;
	tempOLAT = readRegister(regAB(OLAT, pinToPort(pin)));
	if (state) { tempOLAT |= pinToMask(pin); }
	else { tempOLAT &= ~pinToMask(pin); }
	writeRegister(regAB(OLAT, pinToPort(pin)), tempOLAT);
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
	uint8_t tempIODIR, tempGPPU;

	if (mode == INPUT)
	{
		tempIODIR = 0xFF;
		tempGPPU = 0x00;
	}
	else if (mode == INPUT_PULLUP)
	{
		tempIODIR = 0xFF;
		tempGPPU = 0xFF;
	}
	else if (mode == OUTPUT)
	{
		tempIODIR = 0x00;
		tempGPPU = 0x00;
	}

	writeRegister(regAB(IODIR, port), tempIODIR);
	writeRegister(regAB(GPPU, port), tempGPPU);
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
	uint8_t tempIODIR, tempGPPU;

	if (mode == INPUT)
	{
		tempIODIR = 0xFF;
		tempGPPU = 0x00;
	}
	else if (mode == INPUT_PULLUP)
	{
		tempIODIR = 0xFF;
		tempGPPU = 0xFF;
	}
	else if (mode == OUTPUT)
	{
		tempIODIR = 0x00;
		tempGPPU = 0x00;
	}

	writeRegister(IODIRA, tempIODIR);
	writeRegister(IODIRB, tempIODIR);
	writeRegister(GPPUA, tempGPPU);
	writeRegister(GPPUB, tempGPPU);
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

void MCP23017::setInputPolarity(bool state)
{

}

void MCP23017::setInputPolarity(MCP23017_Port_t port, bool state)
{

}

/**
 * @brief Set the input polarity of an individual pin
 * @details [long description]
 *
 * @param pin [description]
 * @param state [description]
 */
void MCP23017::setInputPolarity(uint8_t pin, bool state)
{

}
