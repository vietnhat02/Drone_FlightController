#include "stm32f4xx_hal.h"
#include "dma_driver.h"

/** @brief Enables the clock for the DMA controller associated with the given stream
 * @param stream Pointer to the DMA stream
 */
static void DMA_EnableClock(DMA_Stream_TypeDef *stream) {
    if (((uint32_t)stream & 0xFFFF0000U) == DMA1_BASE) {
        __HAL_RCC_DMA1_CLK_ENABLE();
    } else {
        __HAL_RCC_DMA2_CLK_ENABLE();
    }
}

/** @brief Waits for the DMA stream to be disabled
 * @param stream Pointer to the DMA stream
 */
static void DMA_WaitDisabled(DMA_Stream_TypeDef *stream) {
    stream->CR &= ~DMA_SxCR_EN;
    while ((stream->CR & DMA_SxCR_EN) != 0U) {
    }
}

/** @brief Initializes a DMA channel
 * @param stream Pointer to the DMA stream
 * @param channel DMA channel number
 * @param direction Transfer direction
 * @param priority Transfer priority
 */
void DMA_InitChannel(DMA_Stream_TypeDef *stream,
                     uint32_t channel,
                     uint32_t direction,
                     uint32_t priority) {
    DMA_EnableClock(stream);
    DMA_WaitDisabled(stream);

    stream->CR   = 0U;
    stream->NDTR = 0U;
    stream->PAR  = 0U;
    stream->M0AR = 0U;
    stream->M1AR = 0U;
    stream->FCR  = 0U;

    stream->CR |= (channel  & DMA_CR_CHANNEL_MASK)  << DMA_CR_CHANNEL_POS;       // choose channel 0-7
    stream->CR |= (priority & DMA_CR_PRIORITY_MASK) << DMA_CR_PRIORITY_POS;      // set priority level
    stream->CR |= (direction & DMA_CR_DIRECTION_MASK) << DMA_CR_DIRECTION_POS;   // set transfer direction
    stream->CR |= DMA_SxCR_MINC;                                                 // enable memory increment mode
}

/** @brief Starts a DMA transfer
 * @param stream Pointer to the DMA stream
 * @param srcAddress Source address
 * @param dstAddress Destination address
 * @param length Transfer length
 * @param direction Transfer direction
 */
void DMA_StartTransfer(DMA_Stream_TypeDef *stream,
                       uint32_t srcAddress,
                       uint32_t dstAddress,
                       uint16_t length,
                       uint32_t direction) {
    DMA_WaitDisabled(stream);

    /* PAR and M0AR are set based on the transfer direction */
    if (direction == DMA_DIR_PERIPH_TO_MEM) {
        stream->PAR  = srcAddress;   
        stream->M0AR = dstAddress;  
    } else if (direction == DMA_DIR_MEM_TO_PERIPH) {
        stream->PAR  = dstAddress;   
        stream->M0AR = srcAddress;   
    } else {
        /* MEM_TO_MEM: PAR is source, M0AR is destination */
        stream->PAR  = srcAddress;
        stream->M0AR = dstAddress;
    }

    stream->NDTR = length;
    stream->CR  |= DMA_SxCR_EN;
}


/** @brief Stops a DMA transfer
 * @param stream Pointer to the DMA stream
 */
void DMA_StopTransfer(DMA_Stream_TypeDef *stream) {
    DMA_WaitDisabled(stream);
}


/** @brief Clears DMA interrupt flags for a specific stream
 * @param dma Pointer to the DMA controller
 * @param streamIndex Index of the DMA stream (0-7)
 */
void DMA_ClearFlags(DMA_TypeDef *dma, uint8_t streamIndex) {
    static const uint8_t offsetTable[DMA_STREAMS_PER_GROUP] = {
        DMA_FLAG_OFFSET_0,
        DMA_FLAG_OFFSET_1,
        DMA_FLAG_OFFSET_2,
        DMA_FLAG_OFFSET_3
    };

    uint8_t groupIndex = streamIndex % DMA_STREAMS_PER_GROUP;
    uint32_t mask = DMA_FLAG_CLEAR_MASK << offsetTable[groupIndex];

    if (streamIndex < DMA_STREAMS_PER_GROUP) {
        dma->LIFCR = mask;   /* stream 0-3 */
    } else {
        dma->HIFCR = mask;   /* stream 4-7 */
    }
}