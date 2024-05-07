#ifndef _STRING_H
#define _STRING_H

#include "common.h"

/* @brief Initialize and clear the terminal */
void terminal_init();

/* @brief Output a character on the terminal
 *
 * @param x:     The x coordinate to output to
 * @param y:     The y coordinate to output to
 * @param c:     The character to output
 * @param color: The color of the output
 */
void terminal_putchar(int x, int y, char c, char color);

/* @brief Convert a character to lowercase
 *
 * @param c: The character to convert
 */
char to_lower(unsigned char c);

/* @brief Convert a character to uppercase
 *
 * @param c: The character to convert
 */
char to_upper(unsigned char c);

/* @brief Check if a character is a digit
 *
 * @returns true if digiit
 */
bool is_digit(char c);

/* @brief Convert a character to digit */
int to_digit(char c);

/* @brief Calculate the length of a string */
size_t strlen(const char* str);

/* @brief Calculate the length of a string
 *
 * @return MAX(strlen(str), max)
 */
size_t strnlen(const char* str, size_t max);

/* @brief Copy a string from @src to @dest */
char* strcpy(char* dest, const char* src);

/* @brief Copy @n bytes from @src to @dest */
char* strncpy(char* dest, const char* src, size_t n);

/* @brief Copy non-whitespace characters from @src to @dest */
char* strcpy_strip(char* dest, const char* src);

/* @brief Compare the @n first bytes of @s1 and @s2
 *
 * @returns 0 if equal
 */
int strncmp(const char* s1, const char* s2, size_t n);

/* @brief Compare, case-insensitive, the @n first bytes of @s1 and @s2
 *
 * @returns 0 if equal
 */
int istrncmp(const char* s1, const char* s2, size_t n);

/* @brief Calculate the length of @str until @terminator, or @max if too long */
int strnlen_terminator(const char* str, int max, char terminator);

/* brief Print @str to the terminal */
void print(const char* str);

/* @brief Print @c to the terminal */
void putchar(char c);

#endif /* _STRING_H*/
