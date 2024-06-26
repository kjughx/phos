#ifndef _STRING_H_
#define _STRING_H_

#include "stdbool.h"
#include "stddef.h"

/* @brief Convert @c to lowercase */
char to_lower(unsigned char c);

/* @brief Convert @c to uppercase */
char to_upper(unsigned char c);

/* @brief Check if @c is a digit */
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

char* strtok(char* str, const char* delimit);

#endif /* _STRING_H_ */
