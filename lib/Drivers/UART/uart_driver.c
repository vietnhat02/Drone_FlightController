#include "stm32f4xx_hal.h"
#include "uart_driver.h"

/* ================================================================
 * Hàm nội bộ (static)
 * ================================================================ */

static void UART_EnableClock(USART_TypeDef *uart) {
    if (uart == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();
    } else if (uart == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();
    } else if (uart == USART3) {
        __HAL_RCC_USART3_CLK_ENABLE();
    } else if (uart == UART4) {
        __HAL_RCC_UART4_CLK_ENABLE();
    } else if (uart == UART5) {
        __HAL_RCC_UART5_CLK_ENABLE();
    } else if (uart == USART6) {
        __HAL_RCC_USART6_CLK_ENABLE();
    }
}

/* USART1 và USART6 nằm trên APB2, các UART còn lại nằm trên APB1.
 * Hai bus này có thể có prescaler khác nhau, nên KHÔNG được dùng chung
 * SystemCoreClock (SYSCLK) để tính BRR - phải tính đúng PCLK tương ứng. */
static uint32_t UART_GetPeripheralClockHz(USART_TypeDef *uart) {
    uint32_t sysClk = SystemCoreClock;
    uint32_t cfgr = RCC->CFGR;
    uint32_t prescaler;
    uint32_t ppreBits;

    if ((uart == USART1) || (uart == USART6)) {
        ppreBits = (cfgr >> 13U) & 0x7U;   /* PPRE2[2:0] */
    } else {
        ppreBits = (cfgr >> 10U) & 0x7U;   /* PPRE1[2:0] */
    }

    if (ppreBits == 0x4U) {
        prescaler = 2U;
    } else if (ppreBits == 0x5U) {
        prescaler = 4U;
    } else if (ppreBits == 0x6U) {
        prescaler = 8U;
    } else if (ppreBits == 0x7U) {
        prescaler = 16U;
    } else {
        prescaler = 1U;
    }

    return sysClk / prescaler;
}

/* Chờ 1 flag trong SR lên 1, có timeout. */
static UART_Status_t UART_WaitFlag(USART_TypeDef *uart, uint32_t flagMask) {
    uint32_t timeout = UART_TIMEOUT_LOOPS;

    while ((uart->SR & flagMask) == 0U) {
        if (--timeout == 0U) {
            return UART_ERROR_TIMEOUT;
        }
    }
    return UART_OK;
}

/* Kiểm tra và dịch các cờ lỗi khung truyền thành mã lỗi tương ứng.
 * Đọc SR rồi đọc DR là chuỗi clear chuẩn cho ORE/FE/NE/PE theo RM0090. */
static UART_Status_t UART_CheckErrorFlags(USART_TypeDef *uart) {
    uint32_t sr = uart->SR;

    if ((sr & UART_SR_ERROR_MASK) == 0U) {
        return UART_OK;
    }

    (void)uart->DR;   /* đọc DR để hoàn tất chuỗi clear lỗi */

    if ((sr & USART_SR_ORE) != 0U) {
        return UART_ERROR_OVERRUN;
    }
    if ((sr & USART_SR_FE) != 0U) {
        return UART_ERROR_FRAMING;
    }
    if ((sr & USART_SR_NE) != 0U) {
        return UART_ERROR_NOISE;
    }
    return UART_ERROR_PARITY;
}

/* ================================================================
 * API public
 * ================================================================ */

void UART_Init(USART_TypeDef *uart, uint32_t baudRate) {
    uint32_t pclkHz;
    uint32_t usartDiv;

    UART_EnableClock(uart);

    uart->CR1 &= ~USART_CR1_UE;   /* tắt UART trước khi cấu hình */

    pclkHz = UART_GetPeripheralClockHz(uart);

    /* Công thức chuẩn (không dùng oversampling 8x, OVER8=0 mặc định):
     * USARTDIV = f_PCLK / (16 x baud), giữ nguyên cả phần thập phân
     * bằng cách nhân 100 trước khi chia để giảm sai số làm tròn. */
    usartDiv = (pclkHz * 100U) / (16U * baudRate);

    uint32_t mantissa = usartDiv / 100U;
    uint32_t fraction  = ((usartDiv - (mantissa * 100U)) * 16U + 50U) / 100U;

    /* Fraction tràn 4 bit (tối đa 15) do làm tròn -> cộng dồn lên mantissa */
    if (fraction >= 16U) {
        fraction = 0U;
        mantissa += 1U;
    }

    uart->BRR = (mantissa << 4U) | (fraction & 0xFU);

    uart->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

UART_Status_t UART_WriteByte(USART_TypeDef *uart, uint8_t data) {
    UART_Status_t status = UART_WaitFlag(uart, USART_SR_TXE);
    if (status != UART_OK) {
        return status;
    }
    uart->DR = data;
    return UART_OK;
}

UART_Status_t UART_Write(USART_TypeDef *uart, const uint8_t *data, uint16_t length) {
    UART_Status_t status;
    uint16_t index;

    for (index = 0U; index < length; ++index) {
        status = UART_WriteByte(uart, data[index]);
        if (status != UART_OK) {
            return status;
        }
    }

    /* Chờ byte cuối thực sự đẩy ra khỏi shift register hoàn toàn */
    return UART_WaitFlag(uart, USART_SR_TC);
}

UART_Status_t UART_ReadByte(USART_TypeDef *uart, uint8_t *data) {
    UART_Status_t status;

    status = UART_WaitFlag(uart, USART_SR_RXNE);
    if (status != UART_OK) {
        return status;
    }

    /* Kiểm tra lỗi khung truyền TRƯỚC khi lấy dữ liệu, vì đọc DR
     * ở đây cũng đồng thời clear RXNE - phải xử lý lỗi đúng thứ tự */
    status = UART_CheckErrorFlags(uart);
    if (status != UART_OK) {
        return status;
    }

    *data = (uint8_t)uart->DR;
    return UART_OK;
}

UART_Status_t UART_Read(USART_TypeDef *uart, uint8_t *data, uint16_t length) {
    UART_Status_t status;
    uint16_t index;

    for (index = 0U; index < length; ++index) {
        status = UART_ReadByte(uart, &data[index]);
        if (status != UART_OK) {
            return status;
        }
    }
    return UART_OK;
}