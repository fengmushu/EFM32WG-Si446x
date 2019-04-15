/***************************************************************************//**
 * @file
 * @brief SPIDRV API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#if 0
#include <string.h>

#include "em_device.h"
#include "em_gpio.h"
#include "em_core.h"
#include "em_usart.h"

#include "dmadrv.h"
#include "spidrv.h"

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
#define SPI_DMA_IRQ   DMA_IRQn

#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
#define SPI_DMA_IRQ   LDMA_IRQn

#else
#error "No valid SPIDRV DMA engine defined."
#endif

/**
 * @brief SPI Pins structure used when mapping from location to gpio port+pin.
 */
typedef struct {
  uint8_t mosiPort;
  uint8_t mosiPin;
  uint8_t misoPort;
  uint8_t misoPin;
  uint8_t clkPort;
  uint8_t clkPin;
  uint8_t csPort;
  uint8_t csPin;
} SPI_Pins_t;

static bool     spidrvIsInitialized = false;

static void     BlockingComplete(SPIDRV_Handle_t handle,
                                 Ecode_t transferStatus,
                                 int itemsTransferred);

static Ecode_t  ConfigGPIO(SPIDRV_Handle_t handle, bool enable);

static bool     RxDMAComplete(unsigned int channel,
                              unsigned int sequenceNo,
                              void *userParam);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
static void     SlaveTimeout(RTCDRV_TimerID_t id,
                             void *user);
#endif

static void     StartReceiveDMA(SPIDRV_Handle_t handle,
                                void *buffer,
                                int count,
                                SPIDRV_Callback_t callback);

static void     StartTransferDMA(SPIDRV_Handle_t handle,
                                 const void *txBuffer,
                                 void *rxBuffer,
                                 int count,
                                 SPIDRV_Callback_t callback);

static void     StartTransmitDMA(SPIDRV_Handle_t handle,
                                 const void *buffer,
                                 int count,
                                 SPIDRV_Callback_t callback);

static Ecode_t  TransferApiPrologue(SPIDRV_Handle_t handle,
                                    void *buffer,
                                    int count);

static Ecode_t  TransferApiBlockingPrologue(SPIDRV_Handle_t handle,
                                            void *buffer,
                                            int count);

static void     WaitForTransferCompletion(SPIDRV_Handle_t handle);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
static Ecode_t  WaitForIdleLine(SPIDRV_Handle_t handle);
#endif

/// @endcond

/***************************************************************************//**
 * @brief
 *    Initialize an SPI driver instance.
 *
 * @param[out] handle  Pointer to an SPI driver handle; refer to @ref
 *                     SPIDRV_Handle_t.
 *
 * @param[in] initData Pointer to an initialization data structure;
 *                     refer to @ref SPIDRV_Init_t.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_Init(SPIDRV_Handle_t handle, SPIDRV_Init_t *initData)
{
  Ecode_t retVal;
  CORE_DECLARE_IRQ_STATE;
  USART_InitSync_TypeDef usartInit = USART_INITSYNC_DEFAULT;
#if defined(_SILICON_LABS_32B_SERIES_2)
  int8_t spiPortNum = -1;
#endif

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ( initData == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  memset(handle, 0, sizeof(SPIDRV_HandleData_t));

  if ( 0 ) {
#if defined(USART0)
  } else if ( initData->port == USART0 ) {
    handle->usartClock  = cmuClock_USART0;
    handle->txDMASignal = dmadrvPeripheralSignal_USART0_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
    spiPortNum = 0;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART0_RXDATAV;
  #endif
#endif
#if defined(USART1)
  } else if ( initData->port == USART1 ) {
    handle->usartClock  = cmuClock_USART1;
    handle->txDMASignal = dmadrvPeripheralSignal_USART1_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
    spiPortNum = 1;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART1_RXDATAV;
  #endif
#endif
#if defined(USART2)
  } else if ( initData->port == USART2 ) {
    handle->usartClock  = cmuClock_USART2;
    handle->txDMASignal = dmadrvPeripheralSignal_USART2_TXBL;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
    spiPortNum = 2;
  #else
    handle->rxDMASignal = dmadrvPeripheralSignal_USART2_RXDATAV;
  #endif
#endif
#if defined(USART3)
  } else if ( initData->port == USART3 ) {
    handle->usartClock  = cmuClock_USART3;
    handle->txDMASignal = dmadrvPeripheralSignal_USART3_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART3_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 3;
  #endif
#endif
#if defined(USART4)
  } else if ( initData->port == USART4 ) {
    handle->usartClock  = cmuClock_USART4;
    handle->txDMASignal = dmadrvPeripheralSignal_USART4_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART4_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 4;
  #endif
#endif
#if defined(USART5)
  } else if ( initData->port == USART5 ) {
    handle->usartClock  = cmuClock_USART5;
    handle->txDMASignal = dmadrvPeripheralSignal_USART5_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USART5_RXDATAV;
  #if defined(_SILICON_LABS_32B_SERIES_2)
    spiPortNum = 5;
  #endif
#endif
#if defined(USARTRF0)
  } else if ( initData->port == USARTRF0 ) {
    handle->usartClock  = cmuClock_USARTRF0;
    handle->txDMASignal = dmadrvPeripheralSignal_USARTRF0_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF0_RXDATAV;
#endif
#if defined(USARTRF1)
  } else if ( initData->port == USARTRF1 ) {
    handle->usartClock  = cmuClock_USARTRF1;
    handle->txDMASignal = dmadrvPeripheralSignal_USARTRF1_TXBL;
    handle->rxDMASignal = dmadrvPeripheralSignal_USARTRF1_RXDATAV;
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

#if defined(_SILICON_LABS_32B_SERIES_2)
  if ( spiPortNum == -1) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
#endif

  handle->initData = *initData;

  if ( initData->bitOrder == spidrvBitOrderMsbFirst ) {
    usartInit.msbf = true;
  }

  if ( initData->clockMode == spidrvClockMode0 ) {
    usartInit.clockMode = usartClockMode0;
  } else if ( initData->clockMode == spidrvClockMode1 ) {
    usartInit.clockMode = usartClockMode1;
  } else if ( initData->clockMode == spidrvClockMode2 ) {
    usartInit.clockMode = usartClockMode2;
  } else if ( initData->clockMode == spidrvClockMode3 ) {
    usartInit.clockMode = usartClockMode3;
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if ( initData->type == spidrvSlave ) {
    usartInit.master = false;
    usartInit.baudrate = 1000;      // Dummy value needed by USART_InitSync()
  } else {
    usartInit.baudrate = initData->bitRate;
  }

#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(handle->usartClock, true);

  if ((initData->frameLength < 4U) || (initData->frameLength > 16U) ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  uint32_t databits = initData->frameLength - 4U + _USART_FRAME_DATABITS_FOUR;
  usartInit.databits = (USART_Databits_TypeDef)databits;

  USART_InitSync(initData->port, &usartInit);

  if ( (initData->type == spidrvMaster)
       && (initData->csControl == spidrvCsControlAuto) ) {
    initData->port->CTRL |= USART_CTRL_AUTOCS;
  }

  if ( initData->csControl == spidrvCsControlAuto ) {
    // SPI 4 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
    initData->port->ROUTELOC0 = (initData->port->ROUTELOC0
                                 & ~(_USART_ROUTELOC0_TXLOC_MASK
                                     | _USART_ROUTELOC0_RXLOC_MASK
                                     | _USART_ROUTELOC0_CLKLOC_MASK
                                     | _USART_ROUTELOC0_CSLOC_MASK))
                                | (initData->portLocationTx  << _USART_ROUTELOC0_TXLOC_SHIFT)
                                | (initData->portLocationRx  << _USART_ROUTELOC0_RXLOC_SHIFT)
                                | (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT)
                                | (initData->portLocationCs  << _USART_ROUTELOC0_CSLOC_SHIFT);

    initData->port->ROUTEPEN = USART_ROUTEPEN_TXPEN
                               | USART_ROUTEPEN_RXPEN
                               | USART_ROUTEPEN_CLKPEN
                               | USART_ROUTEPEN_CSPEN;
#elif defined (_GPIO_USART_ROUTEEN_MASK)
    GPIO->USARTROUTE[spiPortNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                           | GPIO_USART_ROUTEEN_RXPEN
                                           | GPIO_USART_ROUTEEN_CLKPEN
                                           | GPIO_USART_ROUTEEN_CSPEN;

    GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx
                                            << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinTx
                                              << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx
                                            << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinRx
                                              << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->portClk
                                             << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinClk
                                               << _GPIO_USART_CLKROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CSROUTE = ((uint32_t)initData->portCs
                                            << _GPIO_USART_CSROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinCs
                                              << _GPIO_USART_CSROUTE_PIN_SHIFT);
#else
    initData->port->ROUTE = USART_ROUTE_TXPEN
                            | USART_ROUTE_RXPEN
                            | USART_ROUTE_CLKPEN
                            | USART_ROUTE_CSPEN
                            | (initData->portLocation
                               << _USART_ROUTE_LOCATION_SHIFT);
#endif
  } else {
    // SPI 3 wire mode
#if defined(USART_ROUTEPEN_TXPEN)
    initData->port->ROUTELOC0 = (initData->port->ROUTELOC0
                                 & ~(_USART_ROUTELOC0_TXLOC_MASK
                                     | _USART_ROUTELOC0_RXLOC_MASK
                                     | _USART_ROUTELOC0_CLKLOC_MASK))
                                | (initData->portLocationTx  << _USART_ROUTELOC0_TXLOC_SHIFT)
                                | (initData->portLocationRx  << _USART_ROUTELOC0_RXLOC_SHIFT)
                                | (initData->portLocationClk << _USART_ROUTELOC0_CLKLOC_SHIFT);

    initData->port->ROUTEPEN = USART_ROUTEPEN_TXPEN
                               | USART_ROUTEPEN_RXPEN
                               | USART_ROUTEPEN_CLKPEN;
#elif defined (GPIO_USART_ROUTEEN_TXPEN)
    GPIO->USARTROUTE[spiPortNum].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN
                                           | GPIO_USART_ROUTEEN_RXPEN
                                           | GPIO_USART_ROUTEEN_CLKPEN;

    GPIO->USARTROUTE[spiPortNum].TXROUTE = ((uint32_t)initData->portTx
                                            << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinTx
                                              << _GPIO_USART_TXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].RXROUTE = ((uint32_t)initData->portRx
                                            << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                           | ((uint32_t)initData->pinRx
                                              << _GPIO_USART_RXROUTE_PIN_SHIFT);

    GPIO->USARTROUTE[spiPortNum].CLKROUTE = ((uint32_t)initData->pinClk
                                             << _GPIO_USART_CLKROUTE_PORT_SHIFT)
                                            | ((uint32_t)initData->pinClk
                                               << _GPIO_USART_CLKROUTE_PIN_SHIFT);
#else
    initData->port->ROUTE = USART_ROUTE_TXPEN
                            | USART_ROUTE_RXPEN
                            | USART_ROUTE_CLKPEN
                            | (initData->portLocation
                               << _USART_ROUTE_LOCATION_SHIFT);
#endif
  }

  if ( (retVal = ConfigGPIO(handle, true)) != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  CORE_ENTER_ATOMIC();
  if ( !spidrvIsInitialized ) {
    spidrvIsInitialized = true;
    CORE_EXIT_ATOMIC();

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
    RTCDRV_Init();
#endif
  } else {
    CORE_EXIT_ATOMIC();
  }

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if ( initData->type == spidrvSlave ) {
    if ( RTCDRV_AllocateTimer(&handle->timer) != ECODE_EMDRV_RTCDRV_OK ) {
      return ECODE_EMDRV_SPIDRV_TIMER_ALLOC_ERROR;
    }
  }
#endif

  // Initialize DMA.
  DMADRV_Init();

  if ( DMADRV_AllocateChannel(&handle->txDMACh, NULL) != ECODE_EMDRV_DMADRV_OK ) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  if ( DMADRV_AllocateChannel(&handle->rxDMACh, NULL) != ECODE_EMDRV_DMADRV_OK ) {
    return ECODE_EMDRV_SPIDRV_DMA_ALLOC_ERROR;
  }

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Deinitialize an SPI driver instance.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_DeInit(SPIDRV_Handle_t handle)
{
  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  // Stop DMAs.
  DMADRV_StopTransfer(handle->rxDMACh);
  DMADRV_StopTransfer(handle->txDMACh);

  ConfigGPIO(handle, false);

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if ( handle->initData.type == spidrvSlave ) {
    RTCDRV_StopTimer(handle->timer);
    RTCDRV_FreeTimer(handle->timer);
  }
#endif

  USART_Reset(handle->initData.port);

  CMU_ClockEnable(handle->usartClock, false);

  DMADRV_FreeChannel(handle->txDMACh);
  DMADRV_FreeChannel(handle->rxDMACh);
  DMADRV_DeInit();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Abort an ongoing SPI transfer.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_IDLE if
 *    SPI is idle. On failure, an appropriate SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_AbortTransfer(SPIDRV_Handle_t handle)
{
  CORE_DECLARE_IRQ_STATE;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state == spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_IDLE;
  }

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if ( handle->initData.type == spidrvSlave ) {
    RTCDRV_StopTimer(handle->timer);
  }
#endif

  // Stop DMA's.
  DMADRV_StopTransfer(handle->rxDMACh);
  DMADRV_StopTransfer(handle->txDMACh);
  DMADRV_TransferRemainingCount(handle->rxDMACh, &handle->remaining);
  handle->transferStatus    = ECODE_EMDRV_SPIDRV_ABORTED;
  handle->state             = spidrvStateIdle;
  handle->transferStatus    = ECODE_EMDRV_SPIDRV_ABORTED;
  handle->blockingCompleted = true;

  if ( handle->userCallback != NULL ) {
    handle->userCallback(handle,
                         ECODE_EMDRV_SPIDRV_ABORTED,
                         handle->transferCount - handle->remaining);
  }
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current SPI bus bitrate.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] bitRate Current SPI bus bitrate.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetBitrate(SPIDRV_Handle_t handle, uint32_t *bitRate)
{
  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ( bitRate == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  *bitRate = USART_BaudrateGet(handle->initData.port);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get current SPI framelength.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] frameLength Current SPI bus framelength.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetFramelength(SPIDRV_Handle_t handle, uint32_t *frameLength)
{
  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ( frameLength == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  *frameLength = handle->initData.frameLength;

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Get the status of an SPI transfer.
 *
 * @details
 *    Returns status of an ongoing transfer. If no transfer is in progress,
 *    the status of the last transfer is reported.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[out] itemsTransferred Number of items (frames) transferred.
 *
 * @param[out] itemsRemaining Number of items (frames) remaining.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_GetTransferStatus(SPIDRV_Handle_t handle,
                                 int *itemsTransferred,
                                 int *itemsRemaining)
{
  int remaining;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ( (itemsTransferred == NULL) || (itemsRemaining == NULL) ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ATOMIC_SECTION(
    if ( handle->state == spidrvStateIdle ) {
    remaining = handle->remaining;
  } else {
    DMADRV_TransferRemainingCount(handle->rxDMACh, &remaining);
  }
    )

  * itemsTransferred = handle->transferCount - remaining;
  *itemsRemaining   = remaining;

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master receive transfer.
 *
 * @note
 *    The MOSI wire will transmit @ref SPIDRV_Init_t.dummyTxValue.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MReceive(SPIDRV_Handle_t handle,
                        void *buffer,
                        int count,
                        SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  StartReceiveDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking receive transfer.
 *
 * @note
 *    The MOSI wire will transmit @ref SPIDRV_Init_t.dummyTxValue.
 *    @n This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MReceiveB(SPIDRV_Handle_t handle,
                         void *buffer,
                         int count)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  StartReceiveDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master transfer.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransfer(SPIDRV_Handle_t handle,
                         const void *txBuffer,
                         void *rxBuffer,
                         int count,
                         SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, (void*)txBuffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( rxBuffer == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking transfer.
 *
 * @note
 *    This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransferB(SPIDRV_Handle_t handle,
                          const void *txBuffer,
                          void *rxBuffer,
                          int count)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, (void*)txBuffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( rxBuffer == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking single item (frame) transfer.
 *
 * @note
 *    This function is blocking and returns when the transfer is complete
 *    or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] txValue Value to transmit.
 *
 * @param[out] rxValue Value received.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransferSingleItemB(SPIDRV_Handle_t handle,
                                    uint32_t txValue,
                                    void *rxValue)
{
  void *pRx;
  CORE_DECLARE_IRQ_STATE;
  uint32_t rxBuffer;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state != spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  if ( (pRx = rxValue) == NULL ) {
    pRx = &rxBuffer;
  }

  StartTransferDMA(handle, &txValue, pRx, 1, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master transmit transfer.
 *
 * @note
 *    The data received on the MISO wire is discarded.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] buffer Transmit data buffer.
 *
 * @param[in] count Number of bytes in transfer.
 *
 * @param[in] callback Transfer completion callback.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransmit(SPIDRV_Handle_t handle,
                         const void *buffer,
                         int count,
                         SPIDRV_Callback_t callback)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, (void*)buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  StartTransmitDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI master blocking transmit transfer.
 *
 * @note
 *    The data received on the MISO wire is discarded.
 *    @n This function is blocking and returns when the transfer is complete.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] buffer Transmit data buffer.
 *
 * @param[in] count Number of bytes in transfer.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success or @ref ECODE_EMDRV_SPIDRV_ABORTED
 *    if @ref SPIDRV_AbortTransfer() has been called. On failure, an appropriate
 *    SPIDRV @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_MTransmitB(SPIDRV_Handle_t handle,
                          const void *buffer,
                          int count)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvSlave ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, (void*)buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  StartTransmitDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Set SPI bus bitrate.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] bitRate New SPI bus bitrate.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SetBitrate(SPIDRV_Handle_t handle, uint32_t bitRate)
{
  CORE_DECLARE_IRQ_STATE;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state != spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }

  handle->initData.bitRate = bitRate;
  USART_BaudrateSyncSet(handle->initData.port, 0, bitRate);
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Set SPI framelength.
 *
 * @param[in] handle Pointer to an SPI driver handle.
 *
 * @param[in] frameLength New SPI bus framelength.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SetFramelength(SPIDRV_Handle_t handle, uint32_t frameLength)
{
  CORE_DECLARE_IRQ_STATE;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  frameLength -= 3;
  if ( (frameLength < _USART_FRAME_DATABITS_FOUR)
       || (frameLength > _USART_FRAME_DATABITS_SIXTEEN) ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state != spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }

  handle->initData.frameLength = frameLength + 3;
  handle->initData.port->FRAME = (handle->initData.port->FRAME
                                  & ~_USART_FRAME_DATABITS_MASK)
                                 | (frameLength
                                    << _USART_FRAME_DATABITS_SHIFT);
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief
 *    Start an SPI slave receive transfer.
 *
 * @note
 *    The MISO wire will transmit @ref SPIDRV_Init_t.dummyTxValue.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_SReceive(SPIDRV_Handle_t handle,
                        void *buffer,
                        int count,
                        SPIDRV_Callback_t callback,
                        int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartReceiveDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking receive transfer.
 *
 * @note
 *    The MISO wire will transmit @ref SPIDRV_Init_t.dummyTxValue.
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[out] buffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_SReceiveB(SPIDRV_Handle_t handle,
                         void *buffer,
                         int count,
                         int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartReceiveDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave transfer.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransfer(SPIDRV_Handle_t handle,
                         const void *txBuffer,
                         void *rxBuffer,
                         int count,
                         SPIDRV_Callback_t callback,
                         int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, (void*)txBuffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( rxBuffer == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking transfer.
 *
 * @note
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  txBuffer Transmit data buffer.
 *
 * @param[out] rxBuffer Receive data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransferB(SPIDRV_Handle_t handle,
                          const void *txBuffer,
                          void *rxBuffer,
                          int count,
                          int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, (void*)txBuffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( rxBuffer == NULL ) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartTransferDMA(handle, txBuffer, rxBuffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave transmit transfer.
 *
 * @note
 *    The data received on the MOSI wire is discarded.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  buffer Transmit data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  callback Transfer completion callback.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success. On failure, an appropriate SPIDRV
 *    @ref Ecode_t is returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransmit(SPIDRV_Handle_t handle,
                         const void *buffer,
                         int count,
                         SPIDRV_Callback_t callback,
                         int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiPrologue(handle, (void*)buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartTransmitDMA(handle, buffer, count, callback);

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief
 *    Start an SPI slave blocking transmit transfer.
 *
 * @note
 *    The data received on the MOSI wire is discarded.
 *    @n This function is blocking and returns when the transfer is complete,
 *    on timeout, or when @ref SPIDRV_AbortTransfer() is called.
 *
 * @param[in]  handle Pointer to an SPI driver handle.
 *
 * @param[in]  buffer Transmit data buffer.
 *
 * @param[in]  count Number of bytes in transfer.
 *
 * @param[in]  timeoutMs Transfer timeout in milliseconds.
 *
 * @return
 *    @ref ECODE_EMDRV_SPIDRV_OK on success, @ref ECODE_EMDRV_SPIDRV_TIMEOUT on
 *    timeout or @ref ECODE_EMDRV_SPIDRV_ABORTED if @ref SPIDRV_AbortTransfer()
 *    has been called. On failure, an appropriate SPIDRV @ref Ecode_t is
 *    returned.
 ******************************************************************************/
Ecode_t SPIDRV_STransmitB(SPIDRV_Handle_t handle,
                          const void *buffer,
                          int count,
                          int timeoutMs)
{
  Ecode_t retVal;

  if ( handle->initData.type == spidrvMaster ) {
    return ECODE_EMDRV_SPIDRV_MODE_ERROR;
  }

  if ( (retVal = TransferApiBlockingPrologue(handle, (void*)buffer, count))
       != ECODE_EMDRV_SPIDRV_OK ) {
    return retVal;
  }

  if ( timeoutMs ) {
    RTCDRV_StartTimer(handle->timer,
                      rtcdrvTimerTypeOneshot,
                      timeoutMs,
                      SlaveTimeout,
                      handle);
  }

  if ( handle->initData.slaveStartMode == spidrvSlaveStartDelayed ) {
    if ( (retVal = WaitForIdleLine(handle)) != ECODE_EMDRV_SPIDRV_OK ) {
      return retVal;
    }
  }

  StartTransmitDMA(handle, buffer, count, BlockingComplete);

  WaitForTransferCompletion(handle);

  return handle->transferStatus;
}
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN

/***************************************************************************//**
 * @brief
 *    Transfer complete callback function used by blocking transfer API
 *    functions. Called by DMA interrupt handler, timer timeout handler
 *    or @ref SPIDRV_AbortTransfer() function.
 ******************************************************************************/
static void BlockingComplete(SPIDRV_Handle_t handle,
                             Ecode_t transferStatus,
                             int itemsTransferred)
{
  (void)itemsTransferred;

  handle->transferStatus    = transferStatus;
  handle->blockingCompleted = true;
}

#if defined(_SILICON_LABS_32B_SERIES_0)
/***************************************************************************//**
 * @brief Get SPI pins for Series 0 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  uint32_t location;

  location = handle->initData.portLocation;

  if ( 0 ) {
#if defined(USART0)
  } else if ( handle->initData.port == USART0 ) {
    pins->mosiPort = AF_USART0_TX_PORT(location);
    pins->misoPort = AF_USART0_RX_PORT(location);
    pins->clkPort  = AF_USART0_CLK_PORT(location);
    pins->csPort   = AF_USART0_CS_PORT(location);
    pins->mosiPin  = AF_USART0_TX_PIN(location);
    pins->misoPin  = AF_USART0_RX_PIN(location);
    pins->clkPin   = AF_USART0_CLK_PIN(location);
    pins->csPin    = AF_USART0_CS_PIN(location);
#endif
#if defined(USART1)
  } else if ( handle->initData.port == USART1 ) {
    pins->mosiPort = AF_USART1_TX_PORT(location);
    pins->misoPort = AF_USART1_RX_PORT(location);
    pins->clkPort  = AF_USART1_CLK_PORT(location);
    pins->csPort   = AF_USART1_CS_PORT(location);
    pins->mosiPin  = AF_USART1_TX_PIN(location);
    pins->misoPin  = AF_USART1_RX_PIN(location);
    pins->clkPin   = AF_USART1_CLK_PIN(location);
    pins->csPin    = AF_USART1_CS_PIN(location);
#endif
#if defined(USART2)
  } else if ( handle->initData.port == USART2 ) {
    pins->mosiPort = AF_USART2_TX_PORT(location);
    pins->misoPort = AF_USART2_RX_PORT(location);
    pins->clkPort  = AF_USART2_CLK_PORT(location);
    pins->csPort   = AF_USART2_CS_PORT(location);
    pins->mosiPin  = AF_USART2_TX_PIN(location);
    pins->misoPin  = AF_USART2_RX_PIN(location);
    pins->clkPin   = AF_USART2_CLK_PIN(location);
    pins->csPin    = AF_USART2_CS_PIN(location);
#endif
#if defined(USARTRF0)
  } else if ( handle->initData.port == USARTRF0 ) {
    pins->mosiPort = AF_USARTRF0_TX_PORT(location);
    pins->misoPort = AF_USARTRF0_RX_PORT(location);
    pins->clkPort  = AF_USARTRF0_CLK_PORT(location);
    pins->csPort   = AF_USARTRF0_CS_PORT(location);
    pins->mosiPin  = AF_USARTRF0_TX_PIN(location);
    pins->misoPin  = AF_USARTRF0_RX_PIN(location);
    pins->clkPin   = AF_USARTRF0_CLK_PIN(location);
    pins->csPin    = AF_USARTRF0_CS_PIN(location);
#endif
#if defined(USARTRF1)
  } else if ( handle->initData.port == USARTRF1 ) {
    pins->mosiPort = AF_USARTRF1_TX_PORT(location);
    pins->misoPort = AF_USARTRF1_RX_PORT(location);
    pins->clkPort  = AF_USARTRF1_CLK_PORT(location);
    pins->csPort   = AF_USARTRF1_CS_PORT(location);
    pins->mosiPin  = AF_USARTRF1_TX_PIN(location);
    pins->misoPin  = AF_USARTRF1_RX_PIN(location);
    pins->clkPin   = AF_USARTRF1_CLK_PIN(location);
    pins->csPin    = AF_USARTRF1_CS_PIN(location);
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
/***************************************************************************//**
 * @brief Get SPI pins for Series 1 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  SPIDRV_Init_t *initData;
  initData = &handle->initData;

  if ( 0 ) {
#if defined(USART0)
  } else if ( handle->initData.port == USART0 ) {
    pins->mosiPort = AF_USART0_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART0_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART0_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART0_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART0_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART0_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART0_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART0_CS_PIN(initData->portLocationCs);
#endif
#if defined(USART1)
  } else if ( handle->initData.port == USART1 ) {
    pins->mosiPort = AF_USART1_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART1_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART1_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART1_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART1_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART1_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART1_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART1_CS_PIN(initData->portLocationCs);
#endif
#if defined(USART2)
  } else if ( handle->initData.port == USART2 ) {
    pins->mosiPort = AF_USART2_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART2_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART2_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART2_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART2_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART2_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART2_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART2_CS_PIN(initData->portLocationCs);
#endif
#if defined(USART3)
  } else if ( handle->initData.port == USART3 ) {
    pins->mosiPort = AF_USART3_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART3_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART3_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART3_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART3_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART3_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART3_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART3_CS_PIN(initData->portLocationCs);
#endif
#if defined(USART4)
  } else if ( handle->initData.port == USART4 ) {
    pins->mosiPort = AF_USART4_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART4_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART4_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART4_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART4_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART4_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART4_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART4_CS_PIN(initData->portLocationCs);
#endif
#if defined(USART5)
  } else if ( handle->initData.port == USART5 ) {
    pins->mosiPort = AF_USART5_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USART5_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USART5_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USART5_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USART5_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USART5_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USART5_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USART5_CS_PIN(initData->portLocationCs);
#endif
#if defined(USARTRF0)
  } else if ( handle->initData.port == USARTRF0 ) {
    pins->mosiPort = AF_USARTRF0_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USARTRF0_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USARTRF0_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USARTRF0_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USARTRF0_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USARTRF0_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USARTRF0_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USARTRF0_CS_PIN(initData->portLocationCs);
#endif
#if defined(USARTRF1)
  } else if ( handle->initData.port == USARTRF1 ) {
    pins->mosiPort = AF_USARTRF1_TX_PORT(initData->portLocationTx);
    pins->misoPort = AF_USARTRF1_RX_PORT(initData->portLocationRx);
    pins->clkPort  = AF_USARTRF1_CLK_PORT(initData->portLocationClk);
    pins->csPort   = AF_USARTRF1_CS_PORT(initData->portLocationCs);
    pins->mosiPin  = AF_USARTRF1_TX_PIN(initData->portLocationTx);
    pins->misoPin  = AF_USARTRF1_RX_PIN(initData->portLocationRx);
    pins->clkPin   = AF_USARTRF1_CLK_PIN(initData->portLocationClk);
    pins->csPin    = AF_USARTRF1_CS_PIN(initData->portLocationCs);
#endif
  } else {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }
  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief Get SPI pins for Series 2 devices.
 ******************************************************************************/
static Ecode_t GetSpiPins(SPIDRV_Handle_t handle, SPI_Pins_t * pins)
{
  pins->mosiPort = handle->initData.portTx;
  pins->misoPort = handle->initData.portRx;
  pins->clkPort  = handle->initData.portClk;
  pins->csPort   = handle->initData.portCs;
  pins->mosiPin  = handle->initData.pinTx;
  pins->misoPin  = handle->initData.pinRx;
  pins->clkPin   = handle->initData.pinClk;
  pins->csPin    = handle->initData.pinCs;

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

/***************************************************************************//**
 * @brief Configure/deconfigure SPI GPIO pins.
 ******************************************************************************/
static Ecode_t ConfigGPIO(SPIDRV_Handle_t handle, bool enable)
{
  SPI_Pins_t pins;
  Ecode_t ret;

  ret = GetSpiPins(handle, &pins);
  if (ret != ECODE_EMDRV_SPIDRV_OK) {
    return ret;
  }
  handle->csPort = pins.csPort;
  handle->csPin  = pins.csPin;

  if ( enable ) {
    if ( handle->initData.type == spidrvMaster ) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin,
                      gpioModePushPull, 0);
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin,
                      gpioModeInput, 0);

      if (    (handle->initData.clockMode == spidrvClockMode0)
              || (handle->initData.clockMode == spidrvClockMode1) ) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModePushPull, 0);
      } else {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModePushPull, 1);
      }

      if ( handle->initData.csControl == spidrvCsControlAuto ) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)handle->csPort, handle->csPin,
                        gpioModePushPull, 1);
      }
    } else {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin,
                      gpioModeInput, 0);
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin,
                      gpioModePushPull, 0);

      if (    (handle->initData.clockMode == spidrvClockMode0)
              || (handle->initData.clockMode == spidrvClockMode1) ) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModeInputPull, 0);
      } else {
        GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin,
                        gpioModeInputPull, 1);
      }

      if ( handle->initData.csControl == spidrvCsControlAuto ) {
        GPIO_PinModeSet((GPIO_Port_TypeDef)handle->csPort, handle->csPin,
                        gpioModeInputPull, 1);
      }
    }
  } else {
    GPIO_PinModeSet((GPIO_Port_TypeDef)pins.mosiPort, pins.mosiPin, gpioModeInputPull, 0);
    GPIO_PinModeSet((GPIO_Port_TypeDef)pins.misoPort, pins.misoPin, gpioModeInputPull, 0);

    if (    (handle->initData.clockMode == spidrvClockMode0)
            || (handle->initData.clockMode == spidrvClockMode1) ) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 0);
    } else {
      GPIO_PinModeSet((GPIO_Port_TypeDef)pins.clkPort, pins.clkPin, gpioModeInputPull, 1);
    }

    if ( handle->initData.csControl == spidrvCsControlAuto ) {
      GPIO_PinModeSet((GPIO_Port_TypeDef)handle->csPort, handle->csPin,
                      gpioModeDisabled, 0);
    }
  }

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief DMA transfer completion callback. Called by the DMA interrupt handler.
 ******************************************************************************/
static bool RxDMAComplete(unsigned int channel,
                          unsigned int sequenceNo,
                          void *userParam)
{
  CORE_DECLARE_IRQ_STATE;
  SPIDRV_Handle_t handle;
  (void)channel;
  (void)sequenceNo;

  CORE_ENTER_ATOMIC();

  handle = (SPIDRV_Handle_t)userParam;

  handle->transferStatus = ECODE_EMDRV_SPIDRV_OK;
  handle->state          = spidrvStateIdle;
  handle->remaining      = 0;

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
  if ( handle->initData.type == spidrvSlave ) {
    RTCDRV_StopTimer(handle->timer);
  }
#endif

  if ( handle->userCallback != NULL ) {
    handle->userCallback(handle, ECODE_EMDRV_SPIDRV_OK, handle->transferCount);
  }

  CORE_EXIT_ATOMIC();
  return true;
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief Slave transfer timeout callback function.
 ******************************************************************************/
static void SlaveTimeout(RTCDRV_TimerID_t id, void *user)
{
  bool active, pending;
  SPIDRV_Handle_t handle;
  (void)id;

  handle = (SPIDRV_Handle_t)user;

  if ( handle->state == spidrvStateTransferring ) {
    DMADRV_TransferActive(handle->rxDMACh, &active);
    if ( active ) {
      // Stop running DMAs
      DMADRV_StopTransfer(handle->rxDMACh);
      DMADRV_StopTransfer(handle->txDMACh);
      DMADRV_TransferRemainingCount(handle->rxDMACh, &handle->remaining);
    } else {
      // DMA is either completed or not yet started
      DMADRV_TransferCompletePending(handle->txDMACh, &pending);
      if ( pending ) {
        // A DMA interrupt is pending; let the DMA handler do the rest
        return;
      }
      handle->remaining = handle->transferCount;
    }
    handle->transferStatus = ECODE_EMDRV_SPIDRV_TIMEOUT;
    handle->state          = spidrvStateIdle;

    if ( handle->userCallback != NULL ) {
      handle->userCallback(handle,
                           ECODE_EMDRV_SPIDRV_TIMEOUT,
                           handle->transferCount - handle->remaining);
    }
  }
}
#endif

/***************************************************************************//**
 * @brief Start an SPI receive DMA.
 ******************************************************************************/
static void StartReceiveDMA(SPIDRV_Handle_t handle,
                            void *buffer,
                            int count,
                            SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->initData.port->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  handle->userCallback       = callback;

  if ( handle->initData.frameLength > 8 ) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  if ( handle->initData.frameLength > 9 ) {
    rxPort = (void *)&(handle->initData.port->RXDOUBLE);
    txPort = (void *)&(handle->initData.port->TXDOUBLE);
  } else if (handle->initData.frameLength == 9) {
    rxPort = (void *)&(handle->initData.port->RXDATAX);
    txPort = (void *)&(handle->initData.port->TXDATAX);
  } else {
    rxPort = (void *)&(handle->initData.port->RXDATA);
    txPort = (void *)&(handle->initData.port->TXDATA);
  }

  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          (void*)buffer,
                          rxPort,
                          true,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void *)&(handle->initData.dummyTxValue),
                          false,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Start an SPI transmit/receive DMA.
 ******************************************************************************/
static void StartTransferDMA(SPIDRV_Handle_t handle,
                             const void *txBuffer,
                             void *rxBuffer,
                             int count,
                             SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->initData.port->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  handle->userCallback       = callback;

  if ( handle->initData.frameLength > 8 ) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  if ( handle->initData.frameLength > 9 ) {
    rxPort = (void *)&(handle->initData.port->RXDOUBLE);
    txPort = (void *)&(handle->initData.port->TXDOUBLE);
  } else if (handle->initData.frameLength == 9) {
    rxPort = (void *)&(handle->initData.port->RXDATAX);
    txPort = (void *)&(handle->initData.port->TXDATAX);
  } else {
    rxPort = (void *)&(handle->initData.port->RXDATA);
    txPort = (void *)&(handle->initData.port->TXDATA);
  }

  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          rxBuffer,
                          rxPort,
                          true,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void*)txBuffer,
                          true,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Start an SPI transmit DMA.
 ******************************************************************************/
static void StartTransmitDMA(SPIDRV_Handle_t handle,
                             const void *buffer,
                             int count,
                             SPIDRV_Callback_t callback)
{
  void *rxPort, *txPort;
  DMADRV_DataSize_t size;

  handle->blockingCompleted  = false;
  handle->transferCount      = count;
  handle->initData.port->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
  handle->userCallback       = callback;

  if ( handle->initData.frameLength > 8 ) {
    size = dmadrvDataSize2;
  } else {
    size = dmadrvDataSize1;
  }

  if ( handle->initData.frameLength > 9 ) {
    rxPort = (void *)&(handle->initData.port->RXDOUBLE);
    txPort = (void *)&(handle->initData.port->TXDOUBLE);
  } else if (handle->initData.frameLength == 9) {
    rxPort = (void *)&(handle->initData.port->RXDATAX);
    txPort = (void *)&(handle->initData.port->TXDATAX);
  } else {
    rxPort = (void *)&(handle->initData.port->RXDATA);
    txPort = (void *)&(handle->initData.port->TXDATA);
  }

  // Receive DMA runs only to get precise numbers for SPIDRV_GetTransferStatus()
  // Start receive DMA.
  DMADRV_PeripheralMemory(handle->rxDMACh,
                          handle->rxDMASignal,
                          &(handle->dummyRx),
                          rxPort,
                          false,
                          count,
                          size,
                          RxDMAComplete,
                          handle);

  // Start transmit DMA.
  DMADRV_MemoryPeripheral(handle->txDMACh,
                          handle->txDMASignal,
                          txPort,
                          (void*)buffer,
                          true,
                          count,
                          size,
                          NULL,
                          NULL);
}

/***************************************************************************//**
 * @brief Parameter checking function for blocking transfer API functions.
 ******************************************************************************/
static Ecode_t TransferApiBlockingPrologue(SPIDRV_Handle_t handle,
                                           void *buffer,
                                           int count)
{
  CORE_DECLARE_IRQ_STATE;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state != spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief Parameter checking function for non-blocking transfer API functions.
 ******************************************************************************/
static Ecode_t TransferApiPrologue(SPIDRV_Handle_t handle,
                                   void *buffer,
                                   int count)
{
  CORE_DECLARE_IRQ_STATE;

  if ( handle == NULL ) {
    return ECODE_EMDRV_SPIDRV_ILLEGAL_HANDLE;
  }

  if ((buffer == NULL) || (count == 0) || (count > DMADRV_MAX_XFER_COUNT)) {
    return ECODE_EMDRV_SPIDRV_PARAM_ERROR;
  }

  CORE_ENTER_ATOMIC();
  if ( handle->state != spidrvStateIdle ) {
    CORE_EXIT_ATOMIC();
    return ECODE_EMDRV_SPIDRV_BUSY;
  }
  handle->state = spidrvStateTransferring;
  CORE_EXIT_ATOMIC();

  return ECODE_EMDRV_SPIDRV_OK;
}

/***************************************************************************//**
 * @brief Wait for transfer completion.
 ******************************************************************************/
static void WaitForTransferCompletion(SPIDRV_Handle_t handle)
{
  if (CORE_IrqIsBlocked(SPI_DMA_IRQ)) {
    // Poll for completion by calling IRQ handler.
    while ( handle->blockingCompleted == false ) {
#if defined(DMA_PRESENT) && (DMA_COUNT == 1)
      DMA_IRQHandler();
#elif defined(LDMA_PRESENT) && (LDMA_COUNT == 1)
      LDMA_IRQHandler();
#else
#error "No valid SPIDRV DMA engine defined."
#endif
    }
  } else {
    while ( handle->blockingCompleted == false ) ;
  }
}

#if defined(EMDRV_SPIDRV_INCLUDE_SLAVE)
/***************************************************************************//**
 * @brief Wait for CS deassertion. Used by slave transfer API functions.
 ******************************************************************************/
static Ecode_t WaitForIdleLine(SPIDRV_Handle_t handle)
{
  while ( !GPIO_PinInGet((GPIO_Port_TypeDef)handle->csPort, handle->csPin)
          && (handle->state != spidrvStateIdle) ) ;

  if ( handle->state == spidrvStateIdle ) {
    return handle->transferStatus;
  }

  return ECODE_EMDRV_SPIDRV_OK;
}
#endif

/// @endcond

/* *INDENT-OFF* */
/******** THE REST OF THE FILE IS DOCUMENTATION ONLY !**********************//**
 * @addtogroup emdrv
 * @{
 * @addtogroup SPIDRV
 * @brief SPIDRV Serial Peripheral Interface Driver
 * @{

   @details
   The spidrv.c and spidrv.h source files for the SPI driver library are in the
   emdrv/spidrv folder.

   @li @ref spidrv_intro
   @li @ref spidrv_conf
   @li @ref spidrv_api
   @li @ref spidrv_example

   @n @section spidrv_intro Introduction
   The SPI driver supports the SPI capabilities of EFM32/EZR32/EFR32 USARTs.
   The driver is fully reentrant, supports several driver instances, and
   does not buffer or queue data. Both synchronous and asynchronous transfer
   functions are included for both master and slave SPI mode. Synchronous
   transfer functions are blocking and do
   not return before the transfer is complete. Asynchronous transfer
   functions report transfer completion with callback functions. Transfers are
   handled using DMA.

   @note Transfer completion callback functions are called from within the DMA
   interrupt handler with interrupts disabled.

   @n @section spidrv_conf Configuration Options

   Some properties of the SPIDRV driver are compile-time configurable. These
   properties are stored in a file named @ref spidrv_config.h. A template for
   this file, containing default values, is in the emdrv/config folder.
   Currently the configuration options are as follows:
   @li Inclusion of slave API transfer functions.

   To configure SPIDRV, provide a custom configuration file. This is a
   sample @ref spidrv_config.h file:
   @verbatim
#ifndef __SILICON_LABS_SPIDRV_CONFIG_H__
#define __SILICON_LABS_SPIDRV_CONFIG_H__

// SPIDRV configuration option. Use this define to include the
// slave part of the SPIDRV API.
#define EMDRV_SPIDRV_INCLUDE_SLAVE

#endif
   @endverbatim

   The properties of each SPI driver instance are set at run-time using the
   @ref SPIDRV_Init_t data structure input parameter to the @ref SPIDRV_Init()
   function.

   @n @section spidrv_api The API

   This section contains brief descriptions of the API functions. For
   detailed information on input and output parameters and return values,
   click on the hyperlinked function names. Most functions return an error
   code, @ref ECODE_EMDRV_SPIDRV_OK is returned on success,
   see @ref ecode.h and @ref spidrv.h for other error codes.

   The application code must include @em spidrv.h.

   @ref SPIDRV_Init(), @ref SPIDRV_DeInit() @n
    These functions initialize or deinitializes the SPIDRV driver. Typically,
    @htmlonly SPIDRV_Init() @endhtmlonly is called once in the startup code.

   @ref SPIDRV_GetTransferStatus() @n
    Query the status of a transfer. Reports number of items (frames) transmitted
    and remaining.

   @ref SPIDRV_AbortTransfer() @n
    Stop an ongoing transfer.

   @ref SPIDRV_SetBitrate(), @ref SPIDRV_GetBitrate() @n
    Set or query the SPI bus bitrate.

   @ref SPIDRV_SetFramelength(), @ref SPIDRV_GetFramelength() @n
    Set or query SPI the bus frame length.

   SPIDRV_MReceive(), SPIDRV_MReceiveB() @n
   SPIDRV_MTransfer(), SPIDRV_MTransferB(), SPIDRV_MTransferSingleItemB() @n
   SPIDRV_MTransmit(), SPIDRV_MTransmitB() @n
   SPIDRV_SReceive(), SPIDRV_SReceiveB() @n
   SPIDRV_STransfer(), SPIDRV_STransferB() @n
   SPIDRV_STransmit(), SPIDRV_STransmitB() @n
    SPI transfer functions for SPI masters have an uppercase M in their name,
    the slave counterparts have an S.

    As previously mentioned, transfer functions are synchronous and asynchronous.
    The synchronous versions have an uppercase B (for Blocking) at the end of
    their function name.

    @em Transmit functions discard received data, @em receive functions transmit
    a fixed data pattern set when the driver is initialized
    (@ref SPIDRV_Init_t.dummyTxValue). @em Transfer functions both receive and
    transmit data.

    All slave transfer functions have a millisecond timeout parameter. Use 0
    for no (infinite) timeout.

   @n @section spidrv_example Example
   @verbatim
#include "spidrv.h"

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

void TransferComplete(SPIDRV_Handle_t handle,
                      Ecode_t transferStatus,
                      int itemsTransferred)
{
  if (transferStatus == ECODE_EMDRV_SPIDRV_OK) {
   // Success !
  }
}

int main(void)
{
  uint8_t buffer[10];
  SPIDRV_Init_t initData = SPIDRV_MASTER_USART2;

  // Initialize an SPI driver instance.
  SPIDRV_Init(handle, &initData);

  // Transmit data using a blocking transmit function.
  SPIDRV_MTransmitB(handle, buffer, 10);

  // Transmit data using a callback to catch transfer completion.
  SPIDRV_MTransmit(handle, buffer, 10, TransferComplete);
}
   @endverbatim

 * @} end group SPIDRV ********************************************************
 * @} end group emdrv ****************************************************/
#endif