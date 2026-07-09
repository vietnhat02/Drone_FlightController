#include "stm32f4xx_hal.h"
#include "i2c_driver.h"

/* ================================================================
 * Hàm nội bộ (static)
 * ================================================================ */

static void I2C_EnableClock(I2C_TypeDef *i2c) {
    if (i2c == I2C1) {
        __HAL_RCC_I2C1_CLK_ENABLE();
    } else if (i2c == I2C2) {
        __HAL_RCC_I2C2_CLK_ENABLE();
    } else if (i2c == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();
    }
}

static uint32_t I2C_GetPclk1MHz(void) {
    uint32_t pclk1 = SystemCoreClock;
    uint32_t ppre1 = (RCC->CFGR >> 10U) & 0x7U;

    if (ppre1 == 0x4U) {
        pclk1 /= 2U;
    } else if (ppre1 == 0x5U) {
        pclk1 /= 4U;
    } else if (ppre1 == 0x6U) {
        pclk1 /= 8U;
    } else if (ppre1 == 0x7U) {
        pclk1 /= 16U;
    }

    return pclk1 / 1000000U;
}

/* Chờ 1 flag trong SR1 lên 1, có timeout.
 * Trả về I2C_OK nếu thành công, I2C_ERROR_TIMEOUT nếu quá thời gian chờ. */
static I2C_Status_t I2C_WaitSR1Flag(I2C_TypeDef *i2c, uint32_t flagMask) {
    uint32_t timeout = I2C_TIMEOUT_LOOPS;

    while ((i2c->SR1 & flagMask) == 0U) {
        if (--timeout == 0U) {
            return I2C_ERROR_TIMEOUT;
        }
        /* Nếu bus lỗi (AF - Acknowledge Failure) thì thoát sớm, không chờ hết timeout */
        if ((i2c->SR1 & I2C_SR1_AF) != 0U) {
            i2c->SR1 &= ~I2C_SR1_AF;   /* clear cờ AF bằng cách ghi 0 */
            return I2C_ERROR_NACK;
        }
    }
    return I2C_OK;
}

/* Đọc SR1 rồi SR2 để clear cờ ADDR - PHẢI được gọi đúng thời điểm
 * theo yêu cầu của caller (xem chú thích trong I2C_ReadBytes) */
static void I2C_ClearAddrFlag(I2C_TypeDef *i2c) {
    (void)i2c->SR1;
    (void)i2c->SR2;
}

/* Gửi điều kiện START + địa chỉ slave, chờ tới khi ADDR được set.
 * KHÔNG clear ADDR ở đây - caller phải tự clear đúng lúc,
 * vì với đọc 1 byte cần set NACK/STOP TRƯỚC khi clear ADDR (theo RM0090). */
static I2C_Status_t I2C_SendStartAndAddress(I2C_TypeDef *i2c,
                                             uint8_t slaveAddress,
                                             uint8_t direction) {
    I2C_Status_t status;

    i2c->CR1 |= I2C_CR1_START;
    status = I2C_WaitSR1Flag(i2c, I2C_SR1_SB);
    if (status != I2C_OK) {
        return status;
    }
    (void)i2c->SR1;   /* đọc SR1 là bước bắt buộc của chuỗi clear SB */

    i2c->DR = (uint8_t)((slaveAddress << 1U) | direction);

    status = I2C_WaitSR1Flag(i2c, I2C_SR1_ADDR);
    return status;   /* ADDR CHƯA được clear, caller lo tiếp */
}

/* ================================================================
 * API public
 * ================================================================ */

void I2C_Init(I2C_TypeDef *i2c, uint32_t clockSpeedHz, uint32_t ownAddress) {
    uint32_t pclk1MHz = 0U;

    I2C_EnableClock(i2c);
    pclk1MHz = I2C_GetPclk1MHz();

    i2c->CR1 = 0U;
    i2c->CR2 = (pclk1MHz & 0x3FU);

    i2c->OAR1 = (1U << I2C_OAR1_ALWAYS1_POS) | (ownAddress << 1U);

    if (clockSpeedHz <= I2C_STANDARD_MODE_MAX_HZ) {
        /* ---- Standard Mode (thường 100kHz) ----
         * Thigh = Tlow = CCR x Tpclk1
         * CCR = Tpclk1_period / (2 x T_scl_period) tính theo đơn vị MHz/Hz */
        uint32_t ccr = (pclk1MHz * 1000000U) / (clockSpeedHz * 2U);
        if (ccr < 4U) {
            ccr = 4U;   /* giá trị tối thiểu theo RM0090 */
        }
        i2c->CCR = ccr & I2C_CCR_VALUE_MASK;
        i2c->TRISE = pclk1MHz + 1U;
    } else {
        /* ---- Fast Mode (thường 400kHz), dùng tỉ lệ Thigh:Tlow = 1:2 (DUTY=0) ----
         * CCR = Tpclk1_period / (3 x T_scl_period) */
        uint32_t ccr = (pclk1MHz * 1000000U) / (clockSpeedHz * 3U);
        if (ccr < 1U) {
            ccr = 1U;
        }
        i2c->CCR = (ccr & I2C_CCR_VALUE_MASK) | (1U << I2C_CCR_FS_POS);
        i2c->TRISE = ((pclk1MHz * I2C_FAST_MODE_RISE_TIME_NS) / 1000U) + 1U;
    }

    i2c->CR1 = I2C_CR1_PE;   /* bật peripheral, phải làm SAU CÙNG */
}

I2C_Status_t I2C_WriteBytes(I2C_TypeDef *i2c, uint8_t slaveAddress,
                             const uint8_t *data, uint16_t length) {
    I2C_Status_t status;
    uint16_t index;

    status = I2C_SendStartAndAddress(i2c, slaveAddress, I2C_DIR_WRITE);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }
    I2C_ClearAddrFlag(i2c);   /* write mode: clear ADDR bình thường, không cần thứ tự đặc biệt */

    for (index = 0U; index < length; ++index) {
        status = I2C_WaitSR1Flag(i2c, I2C_SR1_TXE);
        if (status != I2C_OK) {
            i2c->CR1 |= I2C_CR1_STOP;
            return status;
        }
        i2c->DR = data[index];
    }

    status = I2C_WaitSR1Flag(i2c, I2C_SR1_BTF);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }

    i2c->CR1 |= I2C_CR1_STOP;
    return I2C_OK;
}

I2C_Status_t I2C_ReadBytes(I2C_TypeDef *i2c, uint8_t slaveAddress,
                            uint8_t *data, uint16_t length) {
    I2C_Status_t status;
    uint16_t index;

    if (length == 0U) {
        return I2C_OK;
    }

    status = I2C_SendStartAndAddress(i2c, slaveAddress, I2C_DIR_READ);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }

    if (length == 1U) {
        /* ---- Trường hợp đặc biệt: đọc đúng 1 byte ----
         * Theo RM0090, thứ tự BẮT BUỘC là:
         *   1. Clear ACK (set NACK) TRƯỚC khi clear ADDR
         *   2. Clear ADDR
         *   3. Set STOP NGAY SAU đó (trước khi byte dữ liệu về DR)
         * Sai thứ tự này gây lỗi timing kinh điển của I2C STM32. */
        i2c->CR1 &= ~I2C_CR1_ACK;
        I2C_ClearAddrFlag(i2c);
        i2c->CR1 |= I2C_CR1_STOP;

        status = I2C_WaitSR1Flag(i2c, I2C_SR1_RXNE);
        if (status != I2C_OK) {
            return status;
        }
        data[0] = (uint8_t)i2c->DR;
        return I2C_OK;
    }

    /* ---- Đọc nhiều byte ---- */
    i2c->CR1 |= I2C_CR1_ACK;
    I2C_ClearAddrFlag(i2c);

    for (index = 0U; index < length; ++index) {
        if (index == (uint16_t)(length - 1U)) {
            /* Byte áp chót: set NACK cho byte CUỐI trước khi nó về DR,
             * theo đúng quy tắc "N-1" của I2C STM32 multi-byte read */
            i2c->CR1 &= ~I2C_CR1_ACK;
            i2c->CR1 |= I2C_CR1_STOP;
        }

        status = I2C_WaitSR1Flag(i2c, I2C_SR1_RXNE);
        if (status != I2C_OK) {
            return status;
        }
        data[index] = (uint8_t)i2c->DR;
    }

    return I2C_OK;
}

I2C_Status_t I2C_WriteRegister(I2C_TypeDef *i2c, uint8_t slaveAddress,
                                uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    return I2C_WriteBytes(i2c, slaveAddress, buffer, 2U);
}

I2C_Status_t I2C_ReadRegister(I2C_TypeDef *i2c, uint8_t slaveAddress,
                               uint8_t reg, uint8_t *value) {
    return I2C_ReadRegisters(i2c, slaveAddress, reg, value, 1U);
}

I2C_Status_t I2C_ReadRegisters(I2C_TypeDef *i2c, uint8_t slaveAddress,
                                uint8_t reg, uint8_t *data, uint16_t length) {
    I2C_Status_t status;

    if (length == 0U) {
        return I2C_OK;
    }

    /* ---- Bước 1: Start + gửi địa chỉ register cần đọc (write phase) ---- */
    status = I2C_SendStartAndAddress(i2c, slaveAddress, I2C_DIR_WRITE);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }
    I2C_ClearAddrFlag(i2c);

    status = I2C_WaitSR1Flag(i2c, I2C_SR1_TXE);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }
    i2c->DR = reg;

    status = I2C_WaitSR1Flag(i2c, I2C_SR1_BTF);
    if (status != I2C_OK) {
        i2c->CR1 |= I2C_CR1_STOP;
        return status;
    }

    /* ---- Bước 2: Repeated START (KHÔNG có STOP ở giữa) rồi đọc dữ liệu ----
     * Đây là điểm khác biệt quan trọng so với việc gọi WriteBytes() rồi
     * ReadBytes() riêng lẻ: không nhả bus giữa 2 giao dịch, đảm bảo
     * con trỏ thanh ghi bên trong cảm biến không bị thiết bị khác can thiệp. */
    return I2C_ReadBytes(i2c, slaveAddress, data, length);
}