/**
 ******************************************************************************
 * @addtogroup PIOS PIOS Core hardware abstraction layer
 * @{
 * @addtogroup   PIOS_USART USART Functions
 * @brief PIOS interface for USART port
 * @{
 *
 * @file       pios_usart.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      USART commands. Inits USARTs, controls USARTs & Interupt handlers. (STM32 dependent)
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_conf.h"
#include "stm32f10x.h"
#include "misc.h"
#include "usart.h"

#define GPIO_NO_REMAP	0x00000000
#define BIT(n)		(1 << (n))


typedef enum
{
	ID_UART1 = 0,
	ID_UART2,
	ID_UART3,
	UART_MAX
} uart_id_t;

/*stm32 com info type*/
typedef struct com_info
{
	USART_TypeDef* usart;
	uint32_t usart_rcc;
	IRQn_Type usart_irqn;
	/*io info*/
	GPIO_TypeDef *tx_port;
	uint16_t tx_pin;
	GPIO_TypeDef *rx_port;
	uint16_t rx_pin;
	uint32_t remap;
} com_info_t;

static com_info_t com_dev[COM_MAX] = 
{
	{USART1, RCC_APB2Periph_USART1, USART1_IRQn, GPIOA, GPIO_Pin_9 , GPIOA, GPIO_Pin_10, GPIO_NO_REMAP},
	{USART1, RCC_APB2Periph_USART1, USART1_IRQn, GPIOB, GPIO_Pin_6 , GPIOB, GPIO_Pin_7 , GPIO_Remap_USART1},

	{USART2, RCC_APB1Periph_USART2, USART2_IRQn, GPIOA, GPIO_Pin_2 , GPIOA, GPIO_Pin_3 , GPIO_NO_REMAP},
	{USART2, RCC_APB1Periph_USART2, USART2_IRQn, GPIOD, GPIO_Pin_5 , GPIOD, GPIO_Pin_6 , GPIO_Remap_USART2},

	{USART3, RCC_APB1Periph_USART3, USART3_IRQn, GPIOB, GPIO_Pin_10, GPIOB, GPIO_Pin_11, GPIO_NO_REMAP},
	{USART3, RCC_APB1Periph_USART3, USART3_IRQn, GPIOC, GPIO_Pin_10, GPIOC, GPIO_Pin_11, GPIO_PartialRemap_USART3},
	{USART3, RCC_APB1Periph_USART3, USART3_IRQn, GPIOD, GPIO_Pin_8 , GPIOD, GPIO_Pin_9 , GPIO_FullRemap_USART3},
};

static uint32_t usart_available = 0x00;

/*
 *return :
 *         0  available
 *         -1 para error
 *         other value : unavaiable
 *
*/
int32_t usart_alloc(const USART_TypeDef* usart)
{
	//add mutex
	uint32_t usart_id = 0;

	switch ((uint32_t)usart) {
	case (uint32_t)USART1:
		usart_id = 1;
		break;
	case (uint32_t)USART2:
		usart_id = 2;
		break;
	case (uint32_t)USART3:
		usart_id = 3;
		break;
	default:
		return ERROR;//-1
	}
	
	if (usart_available & BIT(usart_id)) {
		return usart_id;
	}
	usart_available |= BIT(usart_id);

	return 0;
}

/**
 * Initialise a single USART device
 */
int32_t com_init(com_id_t com, uart_baud_t baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_InitTypeDef USART_InitStruct;

	if (com >= COM_MAX) {
		return -1;
	}

	//if (0 != usart_alloc(com_dev[com].usart)) {
	//	return -1;
	//}

	/* Enable the USART Pins Software Remapping */
	if (com_dev[com].remap) {
		GPIO_PinRemapConfig(com_dev[com].remap, ENABLE);
	}

	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = com_dev[com].tx_pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(com_dev[com].tx_port, &GPIO_InitStruct);

	/* Configure USART Rx as input floating */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = com_dev[com].rx_pin;
	GPIO_Init(com_dev[com].rx_port, &GPIO_InitStruct);

	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(com_dev[com].usart_rcc, ENABLE);


	/* USARTx configured as follow:
	   - BaudRate = 115200 baud
	   - Word Length = 8 Bits
	   - One Stop Bit
	   - No parity
	   - Hardware flow control disabled (RTS and CTS signals)
	   - Receive and transmit enabled
	*/
	USART_InitStruct.USART_BaudRate = (uint32_t)baud;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* USART configuration */
	USART_Init(com_dev[com].usart, &USART_InitStruct);

	/* Configure USART Interrupts */
	NVIC_InitStruct.NVIC_IRQChannel = com_dev[com].usart_irqn;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;//fixme
	NVIC_Init(&NVIC_InitStruct);

	USART_ITConfig(com_dev[com].usart, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(com_dev[com].usart, USART_IT_TXE, ENABLE);//fixme

	/* Enable USART */
	USART_Cmd(com_dev[com].usart, ENABLE);

	return 0;
}

/*config usart baudrate*/
void usart_set_baudrate(com_id_t com, uart_baud_t baud)
{
#if 0
	USART_InitTypeDef USART_InitStruct;

	/* Adjust the baud rate */
	USART_InitStruct.USART_BaudRate = baud;

	/* Write back the new configuration */
	USART_Init(usart_dev->cfg->regs, &USART_InitStruct);
#endif
}

int uart_send_char(USART_TypeDef* usart, int ch)
{
	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(usart, USART_FLAG_TC) == RESET);

	USART_SendData(usart, (u8) ch);

	return ch;
}

int uart_send_string(USART_TypeDef* usart, u8 *head)
{
	u32 count = 0;

	while ('\0' != *head) {
		uart_send_char(usart, *head);
		count++;
	};

	return count;
}

int com_send_char(com_id_t com, int ch)
{
	return uart_send_char(com_dev[com].usart, ch);
}

int com_send_string(com_id_t com, u8 *head, u32 len)
{
	u32 count = len;
	while ((0 != count) && ('\0' != *head)) {
		com_send_char(com, *head);
		count--;
		head++;
	};

	return (len - count);
}

void usart_rx_it_enable(com_id_t com)
{
	USART_ITConfig(com_dev[com].usart, USART_IT_RXNE, ENABLE);
}
void usart_tx_it_enable(com_id_t com)
{
	USART_ITConfig(com_dev[com].usart, USART_IT_TXE, ENABLE);
}

void usart_irq_handler(USART_TypeDef* usart)
{
	/* receive a byte */
	if (USART_GetFlagStatus(usart, USART_FLAG_RXNE)) {
		u8 byte = USART_ReceiveData(usart);
		uart_send_char(usart, byte);
	}
}

void USART1_IRQHandler(void)
{
	usart_irq_handler(USART1);
}

void USART2_IRQHandler(void)
{
	usart_irq_handler(USART2);
}

void USART3_IRQHandler(void)
{
	usart_irq_handler(USART3);
}

/**
 * @}
 * @}
 */
