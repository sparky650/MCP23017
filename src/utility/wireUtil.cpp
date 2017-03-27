#include "wireUtil.h"


/**
 * @brief Write a single register on an i2c device
 * 
 * @param reg Register address (from a device specific enum)
 * @param data Data to be written to the device
 * @return true on success, false if NACK
 */
template <typename REGTYPE>
boolean wireUtil<REGTYPE>::writeRegister(REGTYPE reg, uint8_t data)
{
	return writeRegisters(reg, &data, 1);
}

/**
 * @brief Write to a sequence of registers on an i2c device
 * 
 * @param reg First register address (from a device specific enum)
 * @param buffer Array containing the data to be written
 * @param len Number of bytes in the array
 * @return true on success, false if NACK
 */
template <typename REGTYPE>
boolean wireUtil<REGTYPE>::writeRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	for (uint8_t x = 0; x < len; x++)
	{
		Wire.write(buffer[x]);
	}

	uint8_t status;
	status = Wire.endTransmission();
	if (status == 0)	{ return true; }
	else if (NACKhandler != NULL)
	{
		(*NACKhandler)(status);
		return false;
	}
	return false;
}

/**
 * @brief Read a single register from an i2c device
 * 
 * @param reg Register address (from a device specific enum)
 * @return Data from the device register, 0 if there is a timeout
 */
template <typename REGTYPE>
uint8_t wireUtil<REGTYPE>::readRegister(REGTYPE reg)
{
	unsigned long abortTime;
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false);

	Wire.requestFrom(address, (uint8_t) 1);

	timeoutFlag = false;
	abortTime = millis() + timeoutTime;
	while (!Wire.available())
	{
		if (abortTime < millis())
		{
			timeoutFlag = true;
			if (timeOutHandler != NULL) { (*timeOutHandler)(); }
			return 0;
		}
	}

	return Wire.read();
}

/**
 * @brief Read a number of sequential registers from an i2c device
 * 
 * @param reg First register address (from a device specific enum)
 * @param buffer Array to contain the data read
 * @param len Number of bytes to read
 * @return true on success, false on timeout
 */
template <typename REGTYPE>
bool wireUtil<REGTYPE>::readRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	Wire.endTransmission(false); 

	Wire.requestFrom(address, len);

	timeoutFlag = false;
	unsigned long abortTime = millis() + timeoutTime;
	while (Wire.available() < len)
	{
		if (abortTime < millis())
		{
			timeoutFlag = true;
			memset(buffer, 0, len);
			if (timeOutHandler != NULL) { (*timeOutHandler)(); }
			return false; // timeout condition, return false
		}
	}

	for (uint8_t x = 0 ; x < len ; x++)
	{
		buffer[x] = Wire.read();
	}
	return true;
}