/**
 * @file lcd.hpp
 * Provides functionality to mess with the LCD (...on UART2).
 */

#ifndef LCD_HPP_
#define LCD_HPP_

namespace lcd {
	/**
	 * Initializes the LCD.
	 */
	void init(void);

	/**
	 * Prints a string to the given line and column.
	 * NO ERROR CHECKING DONE!
	 * @param line the line to write to
	 * @param x the column to start on
	 * @param s the string to write
	 */
	void prints(int line, int x, const char *s);

	/**
	 * Prints a decimal number to the given line and column.
	 * NO ERROR CHECKING DONE!
	 * @param line the line to write to
	 * @param x the column to start on
	 * @param val the number to write
	 */
	void printn(int line, int x, int val);

	/**
	 * Clears the LCD text buffer.
	 * Called at the top of the loop lcd printing is done in.
	 * TODO make something better
	 */
	void flush(void);
}


#endif // LCD_HPP_
