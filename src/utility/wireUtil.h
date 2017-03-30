/**
 * @file	wireUtil.h
 * @author	Keegan Morrow
 * @version	1.0.0
 * @breif Utility base class for reading and writing registers on i2c devices
 *
 */

#ifndef __wireUtil_h_
#define __wireUtil_h_

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief Utility base class for reading and writing registers on i2c devices
 *
 * @tparam REGTYPE An initialized enum type that lists the valad registers for the device
 */
template <typename REGTYPE>
class wireUtil
{
public:
	/**
	 * @brief Attach a function to be called on a read timeout
	 *
	 * @param timeOutHandler Pointer to a 'void f(void)' function
	 */
	void attachTimeoutHandler(void (*timeOutHandler)(void)) {this->timeOutHandler = timeOutHandler;}
	/**
	 * @brief Attach a function to be called on a write NACK
	 *
	 * @param NACKhandler Pointer to a 'void f(uint8_t)' function. This will be passed the Wire status.
	 */
	void attachNACKhandler(void (*NACKhandler)(uint8_t)) {this->NACKhandler = NACKhandler;}

	unsigned long timeoutTime; ///< Amout of time to wait for a succesful read
	bool timeoutFlag; ///< Set to true if there is a timeout event, reset on the next read

	inline bool getTimeoutFlag() {return timeoutFlag;}

	virtual void begin();
	virtual void begin(uint8_t);
#if defined(ARDUINO_ARCH_ESP8266)
	virtual void begin(uint8_t, uint8_t, uint8_t);
#endif // ARDUINO_ARCH_ESP8266

	bool writeRegister(REGTYPE, uint8_t);
	bool writeRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len);
	uint8_t readRegister(REGTYPE);
	bool readRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len);

protected:
	uint8_t address; ///< Hardware address of the device

private:
	void (*timeOutHandler)(void);
	void (*NACKhandler)(uint8_t);
};

/*
* Note: Member functions that use template must be
* defined in this file.
*/

/**
 * @brief Initialize the chip at a specific address
 * 
 * @param address  Address of the chip
 */
template <typename REGTYPE>
void wireUtil<REGTYPE>::begin(uint8_t address)
{
	this->address = address;
	Wire.begin();
}

/**
 * @brief Initialize the chip at a specific address and pins
 * @details This is only available on archatectures that support arbitrary SDA and SCL pins.
 *
 * @param address Address of the chip
 * @param SDApin Pin number for the SDA signal
 * @param SCLpin Pin number for the SCL signal
 */
#if defined(ARDUINO_ARCH_ESP8266)
template <typename REGTYPE>
void wireUtil<REGTYPE>::begin(uint8_t address, uint8_t SDApin, uint8_t SCLpin)
{
	this->address = address;
	Wire.begin(SDApin, SCLpin);
}
#endif // ARDUINO_ARCH_ESP8266

/**
 * @brief Write a single register on an i2c device
 *
 * @param reg Register address (from a device specific enum)
 * @param data Data to be written to the device
 * @return true on success, false if NACK
 */
template <typename REGTYPE>
bool wireUtil<REGTYPE>::writeRegister(REGTYPE reg, uint8_t data)
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
bool wireUtil<REGTYPE>::writeRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len)
{
	Wire.beginTransmission(address);
	Wire.write(reg);
	for (uint8_t x = 0; x < len; x++)
	{
		Wire.write(buffer[x]);
	}

	uint8_t status;
	status = Wire.endTransmission();
	if (status == 0) { return true; }
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
	Wire.write((uint8_t)reg);
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
	Wire.write((uint8_t)reg);
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

#endif // __wireUtil_h_