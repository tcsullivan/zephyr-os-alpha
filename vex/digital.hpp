/**
 * @file digital.hpp
 * Provides functionality to control the digital ports.
 */

#ifndef DIGITAL_HPP_
#define DIGITAL_HPP_

namespace digital {
	/**
	 * Sets the pin to either input or output
	 * @param pin pin to set mode of
	 * @param output input if 0, output if not
	 */
	void setMode(int pin, int output);

	/**
	 * Sets the state of a pin.
	 * DOESN'T CHECK FOR PIN MODE!
	 * @param pin pin to set
	 * @param val value to give it (true/false)
	 */
	void set(int pin, int val);

	/**
	 * Sets the state of an LED.
	 * Basically set(), but flips the value as LEDs act that way.
	 * @param pin pin to set
	 * @param val value to give it
	 * @see set
	 */
	inline void setLed(int pin, int val)
	{ set(pin, !(val)); }
}


#endif // DIGITAL_HPP_
