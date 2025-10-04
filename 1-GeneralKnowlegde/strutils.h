#ifndef _STRUTILS_H_
#define _STRUTILS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define ERRNO_II -1 // Invalid Input

#define SUCCESS 0

uint32_t str_reverse(char *str, uint32_t len);
uint32_t str_trim(char *str, uint32_t len);
int32_t str_to_int(const char *str, uint32_t len);

#endif // _STRUTILS_H_