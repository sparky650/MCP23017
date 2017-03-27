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
	inline bool getTimeoutFlag() {return timeoutFlag;}
protected:
	uint8_t address; ///< Hardware address of the device
	bool timeoutFlag; ///< Set to true if there is a timeout event, reset on the next read
	bool writeRegister(REGTYPE reg, uint8_t data);
	bool writeRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len);
	uint8_t readRegister(REGTYPE reg);
	bool readRegisters(REGTYPE reg, uint8_t *buffer, uint8_t len);
private:
	void (*timeOutHandler)(void);
	void (*NACKhandler)(uint8_t);
};

#endif // __wireUtil_h_