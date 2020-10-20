#ifndef __USBSERIAL_H
#define __USBSERIAL_H

#include <stdint.h>

extern void USB_Serial_SendMessage(unsigned char* Message);
extern uint8_t USB_Serial_GetBytes(uint8_t ByteCount, uint8_t* Buffer, uint32_t Timeout);
extern uint8_t USBSerialMode;
extern void USB_SERIAL_FlushRX(void);
extern uint8_t FoundBM;
extern uint16_t USBTimeout;

#define DOS_FORMAT     0
#define UNIX_FORMAT    1
#define PRE_OSX_FORMAT 2
#define LAST_FORMAT    3

#define USBBINARYMODE  0
#define USBTEXTMODE    1

#define COMMAND_BUFFER_SIZE 256

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define USB_UART_BASE           UART0_BASE
#define USB_UART_PERIPH         SYSCTL_PERIPH_UART0
#define USB_UART_INT            INT_UART0

//*****************************************************************************
//
// Default line coding settings for the redirected UART.
//
//*****************************************************************************
#define DEFAULT_BIT_RATE        115200
#define DEFAULT_UART_CONFIG     (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | \
                                 UART_CONFIG_STOP_ONE)

//*****************************************************************************
//
// GPIO peripherals and pins muxed with the redirected UART.  These will depend
// upon the IC in use and the UART selected in USB_UART_BASE.  Be careful that
// these settings all agree with the hardware you are using.
//
//*****************************************************************************

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define TX_GPIO_BASE            GPIO_PORTA_BASE
#define TX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOA
#define TX_GPIO_PIN             GPIO_PIN_1

#define RX_GPIO_BASE            GPIO_PORTA_BASE
#define RX_GPIO_PERIPH          SYSCTL_PERIPH_GPIOA
#define RX_GPIO_PIN             GPIO_PIN_0


#endif
