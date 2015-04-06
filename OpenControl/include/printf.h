#ifndef _NON_STDIO_H_
#define _NON_STDIO_H_

#include <stdarg.h>

#define	ENOMEM		12	/* Out of Memory */
#define	EINVAL		22	/* Invalid argument */
#define ENOSPC		28	/* No space left on device */


int _vsprintf(char *buf, const char *fmt, va_list args);
int printk(const char *fmt, ...);

#endif				/* _PPC_BOOT_STDIO_H_ */
