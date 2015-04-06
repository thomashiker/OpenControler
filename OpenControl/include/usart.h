#ifndef __USART_H
#define __USART_H

typedef enum
{
	COM1 = 0,
	COM2 = 1,
	COM3 = 2,
	COM4 = 3,
	COM5 = 4,
	COM6 = 5,
	COM7 = 6,
	COM_MAX
} com_id_t;

typedef enum
{
	BAUD4800 = 4800,
	BAUD115200 = 115200,
} uart_baud_t;

int32_t com_init(com_id_t com, uart_baud_t baud);
void usart_set_baudrate(com_id_t com, uart_baud_t baud);
int com_send_char(com_id_t com, int ch);
#endif /* __USART_H */