/*
 * Copyright (C) Paul Mackerras 1997.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <stdarg.h>
#include <stddef.h>
#include "string.h"


/************************err type**************************/
#define PRINT_ERR_PARA_NULL_POINTER		(-1)
#define PRINT_ERR_PARA_TIMEOUT		(-2)

/***********************state mask************************/
#define STAT_PERCENT		0x01

#define MAX_SIZE		1024

extern int uart_send(int ch);


int _vsprintf(char *buf, unsigned int size, const char *fmt, va_list args)
{
	char *str = buf;
	char c = '\0';
	const char *s = NULL;
	unsigned int len = 0;
	unsigned int hex = 0;
	const char *digits = "0123456789ABCDEF";
	char result[10];
	int sign_num = 0;

	if (buf == NULL || fmt == NULL) {
		return -1;
	}

	while (*fmt != '\0') {
		if (*fmt == '%') {
			c = *(fmt + 1);
			switch (c) {
			case 'c':
				*str++ = (unsigned char) va_arg(args, int);
				fmt += 2;
				break;
			case 's':
				s = va_arg(args, char *);
				if (NULL != s) {
					len = 0;
					while (('\0' != *s) && (len < MAX_SIZE)) {
						*str++ = *s++;
					}
				}
				fmt += 2;
				break;;
			case 'X':
			case 'x':
				hex = va_arg(args, unsigned int);
				*str++ = '0';
				*str++ = 'x';
				for (len = 0; len < 8; len++) {
					*str++ = digits[(hex >> ((7 - len) << 2)) & 0x0F];
				}
				fmt += 2;
				break;
			case 'd':
				sign_num = va_arg(args, int);
				if (sign_num < 0) {
					sign_num = -sign_num;
					*str++ = '-';
				}
				len = 0;
				do {
					result[len++] = digits[sign_num % 10];
					sign_num /= 10;
				} while (sign_num != 0);
				while (len > 0) {
					*str++ = result[--len];
				}
				fmt += 2;
				break;
			case 'o':
			case 'O':
			    fmt += 2;
				break;
			default:
				*str++ = *fmt++;
			}
		} else {
			if (*fmt == '\n') {
				*str++ = '\r';
			}
			*str++ = *fmt++;
		}
	}

	*str = '\0';

	return str-buf;
}

int _sprintf(char * buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = _vsprintf(buf, 100, fmt, args);
	va_end(args);

	return i;
}

#define FMT_BUF_LEN		128
static char fmt_buf[FMT_BUF_LEN];

int printk(const char *fmt, ...)
{
	va_list args;
	int n = 0;
	unsigned int i = 0;

	va_start(args, fmt);
	n = _vsprintf(fmt_buf, FMT_BUF_LEN, fmt, args);
	va_end(args);

	for (i = 0; i < n; i++) {
		uart_send(fmt_buf[i]);
	}

	return n;
}
