#ifndef DMA_DRIVER_H
#define DMA_DRIVER_H

#include "stm32f4xx.h"
#include <stdint.h>

#define DMA_CR_CHANNEL_POS      25U   /* CHSEL[2:0]  - choose channel 0-7 */
#define DMA_CR_PRIORITY_POS     16U   /* PL[1:0]     - priority level */
#define DMA_CR_DIRECTION_POS     6U   /* DIR[1:0]    - direction */

/* ============ Field width masks (before shift) ============ */
#define DMA_CR_CHANNEL_MASK      0x7U   /* 3 bit: 0-7 */
#define DMA_CR_PRIORITY_MASK     0x3U   /* 2 bit: 0-3 */
#define DMA_CR_DIRECTION_MASK    0x3U   /* 2 bit: 0-3 */

/* ============ Direction values (DIR[1:0]) - theo RM0090 ============ */
#define DMA_DIR_PERIPH_TO_MEM    0x0U
#define DMA_DIR_MEM_TO_PERIPH    0x1U
#define DMA_DIR_MEM_TO_MEM       0x2U

/* ============ Priority values (PL[1:0]) ============ */
#define DMA_PRIORITY_LOW         0x0U
#define DMA_PRIORITY_MEDIUM      0x1U
#define DMA_PRIORITY_HIGH        0x2U
#define DMA_PRIORITY_VERYHIGH    0x3U

/* ============ Interrupt flag clear mask (5 bit: FEIF,DMEIF,TEIF,HTIF,TCIF) ============ */
#define DMA_FLAG_CLEAR_MASK      0x3DU

/* ============ Stream group offset trong LIFCR/HIFCR ============ */
/* Mỗi group (LIFCR: stream 0-3, HIFCR: stream 4-7) có pattern offset lặp lại */
#define DMA_FLAG_OFFSET_0        0U
#define DMA_FLAG_OFFSET_1        6U
#define DMA_FLAG_OFFSET_2        16U
#define DMA_FLAG_OFFSET_3        22U

#define DMA_STREAMS_PER_GROUP    4U

void DMA_InitChannel(DMA_Stream_TypeDef *stream,
                     uint32_t channel,
                     uint32_t direction,
                     uint32_t priority);

void DMA_StartTransfer(DMA_Stream_TypeDef *stream,
                       uint32_t srcAddress,
                       uint32_t dstAddress,
                       uint16_t length,
                       uint32_t direction);

void DMA_StopTransfer(DMA_Stream_TypeDef *stream);

void DMA_ClearFlags(DMA_TypeDef *dma, uint8_t streamIndex);

#endif