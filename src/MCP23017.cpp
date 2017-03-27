#include "MCP23017.h"


void MCP23017::begin()
{
	begin(defaultPort);
}

void MCP23017::begin(uint8_t address)
{
	this->address = address;
	Wire.begin();
}

#if defined(ARDUINO_ARCH_ESP8266)
void MCP23017::begin(uint8_t address, uint8_t SDApin, uint8_t SCLpin)
{
	this->address = address;
	Wire.begin(SDApin, SCLpin);
}
#endif // ARDUINO_ARCH_ESP8266

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

void MCP23017::digitalWrite(uint8_t pin, bool state)
{
	tempOLAT = readRegister(regAB(OLAT, pinToPort(pin)));
	if (state) { tempOLAT |= pinToMask(pin); }
	else { tempOLAT &= ~pinToMask(pin); }
	writeRegister(regAB(OLAT, pinToPort(pin)), tempOLAT);
}

bool MCP23017::digitalRead(uint8_t pin)
{
	return readRegister(regAB(GPIO, pinToPort(pin))) & pinToMask(pin);
}

void MCP23017::portMode(MCP23017_Port port, uint8_t mode)
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

void MCP23017::writePort(MCP23017_Port port, uint8_t state)
{

}

uint8_t MCP23017::readPort(MCP23017_Port port)
{
	return 0;
}

void MCP23017::chipMode(uint8_t mode)
{

}

void MCP23017::writeChip(uint16_t state)
{

}

uint16_t MCP23017::readChip()
{
	return 0;
}

// MCP23017_Register MCP23017::regAB(MCP23017_RegisterGeneric regG, MCP23017_Port port)
// {
// 	return (MCP23017_Register)((uint8_t)regG + (uint8_t)port);
// }


