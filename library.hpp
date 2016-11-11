/**
 * @file library.hpp
 * Provides some std library functions so we don't have to include them all.
 */

#ifndef LIBRARY_HPP_
#define LIBRARY_HPP_

/**
 * Copies 'n' bytes of a string to a given location.
 * This function will stop copying once either 'cnt' bytes have been
 * copied, or if 'src' hits a '\0'.
 * @param dst the destination to copy to
 * @param src the source of the data
 * @param cnt the maximum # of chars to copy
 */
void strncpy(char *dst, const char *src, int cnt);

/**
 * Copies from one location to another.
 * The copy will stop once one of the strings hit a '\0'.
 * @param dst the destination to copy to
 * @param src the source of the data
 * @return total number of bytes copied
 */
int strcpy(char *dst, const char *src);

/**
 * Copies 'n' bytes of data to a given location.
 * Basically strncpy, but no types for 'dst' and 'src'.
 * @param dst the destination to copy to
 * @param src the source of the data
 * @param cnt the maximum # of chars to copy
 * @see strncpy
 */
inline void memncpy(void *dst, const void *src, int cnt)
{ strncpy(reinterpret_cast<char *>(dst), reinterpret_cast<const char *>(src), cnt); }

/**
 * Converts an integer to a string.
 * Sign is not handled, and base can go up to 16.
 * THERE'S NO ERROR HANDLING!
 * @param n number to convert
 * @param base the base to convert with (default decimal)
 * @return the string
 */
char *itoa(int n, int base = 10);

#endif // LIBRARY_HPP_
