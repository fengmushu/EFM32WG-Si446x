#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "em_device.h"
#include "hal-config-types.h"

/* COPY From SDK example */
/***************************************************************************//**
 * @file
 * @brief Provide BSP (board support package) configuration parameters.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef __BSPCONFIG_H
#define __BSPCONFIG_H

#define BSP_STK
#define BSP_STK_BRD2400

#define BSP_BCC_USART       UART0
#define BSP_BCC_CLK         cmuClock_UART0
#define BSP_BCC_LOCATION    UART_ROUTE_LOCATION_LOC1
#define BSP_BCC_TXPORT      gpioPortE
#define BSP_BCC_TXPIN       0
#define BSP_BCC_RXPORT      gpioPortE
#define BSP_BCC_RXPIN       1
#define BSP_BCC_ENABLE_PORT gpioPortF
#define BSP_BCC_ENABLE_PIN  7

#define BSP_GPIO_LEDS
//#define BSP_NO_OF_LEDS          2
#define BSP_GPIO_LED0_PORT      gpioPortE
#define BSP_GPIO_LED0_PIN       2
#define BSP_GPIO_LED1_PORT      gpioPortE
#define BSP_GPIO_LED1_PIN       3
//#define BSP_GPIO_LEDARRAY_INIT { { BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN }, { BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN } }

#define BSP_GPIO_BUTTONS
//#define BSP_NO_OF_BUTTONS       2
#define BSP_GPIO_PB0_PORT       gpioPortB
#define BSP_GPIO_PB0_PIN        9
#define BSP_GPIO_PB1_PORT       gpioPortB
#define BSP_GPIO_PB1_PIN        10

//#define BSP_GPIO_BUTTONARRAY_INIT { { BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN }, { BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN } }

#define BSP_STK_USE_EBI

#define BSP_INIT_DEFAULT  0

#define BSP_BCP_VERSION 2
#include "bsp_bcp.h"

#endif

/* LED re-directed */
#define BSP_LED0_PORT   BSP_GPIO_LED0_PORT
#define BSP_LED0_PIN    BSP_GPIO_LED0_PIN
#define BSP_LED1_PORT   BSP_GPIO_LED1_PORT
#define BSP_LED1_PIN    BSP_GPIO_LED1_PIN


// This file is auto-generated by Hardware Configurator in Simplicity Studio.
// Any content between $[ and ]$ will be replaced whenever the file is regenerated.
// Content outside these regions will be preserved.

// $[ACMP0]
// [ACMP0]$

// $[ACMP1]
// [ACMP1]$

// $[ADC0]
// [ADC0]$

// $[BTL_BUTTON]
#define HAL_BTL_BUTTON_ENABLE             (1)

// [BTL_BUTTON]$

// $[BUTTON]
#define BSP_BUTTON_PRESENT                (1)

#define BSP_BUTTON_COUNT                  (2U)
#define BSP_BUTTON_INIT                   { { BSP_BUTTON0_PORT, BSP_BUTTON0_PIN }, { BSP_BUTTON1_PORT, BSP_BUTTON1_PIN } }
#define BSP_BUTTON_GPIO_DOUT              (HAL_GPIO_DOUT_LOW)
#define BSP_BUTTON_GPIO_MODE              (HAL_GPIO_MODE_INPUT)
// [BUTTON]$

// $[CMU]
#define HAL_CLK_HFCLK_SOURCE              (HAL_CLK_HFCLK_SOURCE_HFRCO)
#define HAL_CLK_LFBCLK_SOURCE             (HAL_CLK_LFCLK_SOURCE_DISABLED)
#define BSP_CLK_LFXO_PRESENT              (0)
#define BSP_CLK_HFXO_PRESENT              (0)
#define BSP_CLK_LFXO_INIT                  CMU_LFXOINIT_DEFAULT
#define BSP_CLK_LFXO_BOOST                (0)
#define BSP_CLK_HFXO_BOOST                (_CMU_CTRL_HFXOBOOST_100PCENT)
#define BSP_CLK_LFXO_FREQ                 (32768U)
#define HAL_CLK_LFACLK_SOURCE             (HAL_CLK_LFCLK_SOURCE_DISABLED)
#define BSP_CLK_HFXO_FREQ                 (48000000UL)
#define BSP_CLK_HFXO_INIT                  CMU_HFXOINIT_DEFAULT
// [CMU]$

// $[DAC0]
// [DAC0]$

// $[EBI]
// [EBI]$

// $[EMU]
#define HAL_EMU_ENABLE                    (1)

#define HAL_EMU_EM23_VREG                 (0)
// [EMU]$

// $[EXTFLASH]
#define BSP_EXTFLASH_INTERNAL             (0)
// [EXTFLASH]$

// $[GPIO]
#define PORTIO_GPIO_SWCLK_PIN             (0U)
#define PORTIO_GPIO_SWCLK_PORT            (gpioPortF)
#define PORTIO_GPIO_DBGROUTE_LOC          (0U)

#define PORTIO_GPIO_SWDIO_PIN             (1U)
#define PORTIO_GPIO_SWDIO_PORT            (gpioPortF)

#define PORTIO_GPIO_SWO_PIN               (2U)
#define PORTIO_GPIO_SWO_PORT              (gpioPortF)

#define BSP_TRACE_SWO_PIN                 (2U)
#define BSP_TRACE_SWO_PORT                (gpioPortF)
#define BSP_TRACE_DBGROUTE_LOC            (0U)

// [GPIO]$

// $[I2C0]
// [I2C0]$

// $[I2C1]
// [I2C1]$

// $[I2CSENSOR]
// [I2CSENSOR]$

// $[IOEXP]
// [IOEXP]$

// $[LCD]
// [LCD]$

// $[LED]
#define BSP_LED_PRESENT                   (1)

#define BSP_LED_POLARITY                  (1)
#define BSP_LED_COUNT                     (2U)
#define BSP_LED_INIT                      { { BSP_LED0_PORT, BSP_LED0_PIN }, { BSP_LED1_PORT, BSP_LED1_PIN } }
// [LED]$

// $[LESENSE]
// [LESENSE]$

// $[LETIMER0]
// [LETIMER0]$

// $[LEUART0]
#define PORTIO_LEUART0_RX_PIN             (5U)
#define PORTIO_LEUART0_RX_PORT            (gpioPortD)
#define PORTIO_LEUART0_ROUTE_LOC          (0U)

#define PORTIO_LEUART0_TX_PIN             (4U)
#define PORTIO_LEUART0_TX_PORT            (gpioPortD)

#define HAL_LEUART0_ENABLE                (1)

#define BSP_LEUART0_RX_PIN                (5U)
#define BSP_LEUART0_RX_PORT               (gpioPortD)
#define BSP_LEUART0_ROUTE_LOC             (0U)

#define BSP_LEUART0_TX_PIN                (4U)
#define BSP_LEUART0_TX_PORT               (gpioPortD)

#define HAL_LEUART0_FLOW_CONTROL          (HAL_LEUART_FLOW_CONTROL_NONE)
#define HAL_LEUART0_BAUD_RATE             (9600UL)
// [LEUART0]$

// $[LEUART1]
// [LEUART1]$

// $[LFXO]
// [LFXO]$

// $[MSC]
// [MSC]$

// $[PCNT0]
// [PCNT0]$

// $[PCNT1]
// [PCNT1]$

// $[PCNT2]
// [PCNT2]$

// $[PORTIO]
// [PORTIO]$

// $[PRS]
// [PRS]$

// $[SERIAL]
// [SERIAL]$

// $[SPIDISPLAY]
// [SPIDISPLAY]$

// $[SPINCP]
// [SPINCP]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[UART0]
#define PORTIO_UART0_RX_PIN               (1U)
#define PORTIO_UART0_RX_PORT              (gpioPortE)
#define PORTIO_UART0_ROUTE_LOC            (1U)

#define PORTIO_UART0_TX_PIN               (0U)
#define PORTIO_UART0_TX_PORT              (gpioPortE)

#define HAL_UART0_ENABLE                  (1)
#define BSP_UART0_ROUTE_LOC               (1U)

#define BSP_UART0_RX_PIN                  (1U)
#define BSP_UART0_RX_PORT                 (gpioPortE)

#define BSP_UART0_TX_PIN                  (0U)
#define BSP_UART0_TX_PORT                 (gpioPortE)

#define HAL_UART0_BAUD_RATE               (115200UL)
#define HAL_UART0_FLOW_CONTROL            (HAL_UART_FLOW_CONTROL_NONE)
// [UART0]$

// $[UART1]
// [UART1]$

// $[UARTNCP]
// [UARTNCP]$

// $[USART0]
// [USART0]$

// $[USART1]
// [USART1]$

// $[USART2]
// [USART2]$

// $[USB]
// [USB]$

// $[VCOM]
// [VCOM]$

// $[WDOG]
#define HAL_WDOG_ENABLE                   (1)

// [WDOG]$

#if defined(_SILICON_LABS_MODULE)
#include "sl_module.h"
#endif


/* UART re-target define */
#define BSP_SERIAL_APP_PORT 		HAL_SERIAL_PORT_LEUART0
#define BSP_SERIAL_APP_TX_PORT  	BSP_LEUART0_TX_PORT
#define BSP_SERIAL_APP_TX_PIN 		BSP_LEUART0_TX_PIN
#define BSP_SERIAL_APP_RX_PORT  	BSP_LEUART0_RX_PORT
#define BSP_SERIAL_APP_RX_PIN 		BSP_LEUART0_RX_PIN
#define BSP_SERIAL_APP_ROUTE_LOC 	BSP_LEUART0_ROUTE_LOC

#endif /* HAL_CONFIG_H */

