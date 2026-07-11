#include <string.h>
#include "unity.h"
#include "gpio_driver.h"
#include "pwm_driver.h"
#include "spi_driver.h"
#include "timer_driver.h"
#include "uart_driver.h"
#include "i2c_driver.h"

uint32_t SystemCoreClock = 16000000U;
RCC_TypeDef *RCC = NULL;

static GPIO_TypeDef gpio;
static TIM_TypeDef tim;
static SPI_TypeDef spi;
static I2C_TypeDef i2c;
static USART_TypeDef uart;
static RCC_TypeDef rcc;

void setUp(void) {
    memset(&gpio, 0, sizeof(gpio));
    memset(&tim, 0, sizeof(tim));
    memset(&spi, 0, sizeof(spi));
    memset(&i2c, 0, sizeof(i2c));
    memset(&uart, 0, sizeof(uart));
    memset(&rcc, 0, sizeof(rcc));
    RCC = &rcc;
}

void tearDown(void) {
}

void test_gpio_config_and_toggle(void) {
    GPIO_Config(&gpio, 5U, GPIODRV_MODE_OUTPUT, GPIODRV_OTYPE_PP,
                GPIODRV_SPEED_HIGH, GPIODRV_PUPD_NONE, 0U);

    TEST_ASSERT_EQUAL_UINT32(0x1U << 10U, gpio.MODER & (0x3U << 10U));

    GPIO_Set(&gpio, 5U);
    TEST_ASSERT_TRUE((gpio.BSRR & (1U << 5U)) != 0U);

    GPIO_Toggle(&gpio, 5U);
    TEST_ASSERT_TRUE((gpio.ODR & (1U << 5U)) != 0U);

    GPIO_Write(&gpio, 5U, 0U);
    TEST_ASSERT_TRUE((gpio.BSRR & (1U << 21U)) != 0U);
}

void test_pwm_init_and_duty(void) {
    PWM_Init(&tim, 1000U, 10000U, 1U);
    TEST_ASSERT_EQUAL_UINT32(999U, tim.PSC);
    TEST_ASSERT_EQUAL_UINT32(9999U, tim.ARR);
    TEST_ASSERT_TRUE((tim.CCER & TIM_CCER_CC1E) != 0U);

    PWM_SetDuty(&tim, 1U, 2500U);
    TEST_ASSERT_EQUAL_UINT32(2500U, tim.CCR1);

    PWM_Start(&tim, 1U);
    TEST_ASSERT_TRUE((tim.CR1 & TIM_CR1_CEN) != 0U);

    PWM_Stop(&tim, 1U);
    TEST_ASSERT_TRUE((tim.CCER & TIM_CCER_CC1E) == 0U);
}

void test_spi_init_and_transfer(void) {
    spi.SR = SPI_SR_TXE | SPI_SR_RXNE;

    SPI_Init(&spi, SPI_BAUD_DIV_8, SPI_MODE_1, SPI_MSB_FIRST);
    TEST_ASSERT_TRUE((spi.CR1 & SPI_CR1_SPE) != 0U);

    uint8_t rx = 0U;
    TEST_ASSERT_EQUAL(SPI_OK, SPI_TransmitByte(&spi, 0xA5U, &rx));
    TEST_ASSERT_EQUAL_HEX8(0xA5U, rx);

    uint8_t txData[2] = {0x11U, 0x22U};
    uint8_t rxData[2] = {0U, 0U};
    TEST_ASSERT_EQUAL(SPI_OK, SPI_TransmitReceive(&spi, txData, rxData, 2U));
    TEST_ASSERT_EQUAL_HEX8(0x11U, rxData[0]);
    TEST_ASSERT_EQUAL_HEX8(0x22U, rxData[1]);
}

void test_timer_init_and_control(void) {
    TIMER_Init(&tim, 1600U, 1000U);
    TEST_ASSERT_EQUAL_UINT32(1599U, tim.PSC);
    TEST_ASSERT_EQUAL_UINT32(999U, tim.ARR);

    TIMER_Start(&tim);
    TEST_ASSERT_TRUE((tim.CR1 & TIM_CR1_CEN) != 0U);

    TIMER_Stop(&tim);
    TEST_ASSERT_TRUE((tim.CR1 & TIM_CR1_CEN) == 0U);
}

void test_uart_init_and_transfer(void) {
    uart.SR = USART_SR_TXE;
    UART_Init(&uart, 9600U);
    TEST_ASSERT_TRUE((uart.CR1 & USART_CR1_UE) != 0U);

    TEST_ASSERT_EQUAL(UART_OK, UART_WriteByte(&uart, 0x7EU));
    TEST_ASSERT_EQUAL_HEX8(0x7EU, uart.DR);

    uart.SR = USART_SR_RXNE;
    uart.DR = 0x33U;

    uint8_t rxByte = 0U;
    TEST_ASSERT_EQUAL(UART_OK, UART_ReadByte(&uart, &rxByte));
    TEST_ASSERT_EQUAL_HEX8(0x33U, rxByte);
}

void test_i2c_init_and_register_access(void) {
    i2c.SR1 = I2C_SR1_SB | I2C_SR1_ADDR | I2C_SR1_TXE | I2C_SR1_BTF | I2C_SR1_RXNE;
    I2C_Init(&i2c, 400000U, 0x50U);
    TEST_ASSERT_TRUE((i2c.CR1 & I2C_CR1_PE) != 0U);

    /* WriteRegister: byte CUỐI CÙNG ghi vào DR chính là value 0x55,
     * không bị ghi đè bởi bước nào sau đó -> assert được chính xác giá trị */
    TEST_ASSERT_EQUAL(I2C_OK, I2C_WriteRegister(&i2c, 0x50U, 0x10U, 0x55U));
    TEST_ASSERT_EQUAL_HEX8(0x55U, i2c.DR);

    /* ReadRegister: chỉ assert trạng thái trả về I2C_OK (chuỗi thanh ghi
     * chạy đúng trình tự, không timeout/NACK).
     *
     * KHÔNG assert giá trị `value` nhận về, vì mock I2C_TypeDef.DR là
     * 1 biến phẳng đơn thuần lưu giá trị ghi cuối cùng - nó không mô
     * phỏng được hành vi phần cứng thật: trên chip thật, khi 1 byte
     * dữ liệu đến từ bus, CHÍNH PHẦN CỨNG tự động nạp giá trị đó vào DR
     * đúng lúc RXNE được set. Mock đơn giản (chỉ preset SR1 sẵn từ đầu)
     * không có khả năng "bơm" giá trị đúng thời điểm driver đọc DR,
     * nên giá trị cuối cùng trong DR chỉ là byte địa chỉ/lệnh gần nhất
     * được driver tự ghi trong chuỗi Start/Repeated-Start, không phải
     * dữ liệu thật của cảm biến.
     *
     * Muốn assert được giá trị nhận về chính xác, cần test tích hợp
     * trên phần cứng thật, hoặc nâng cấp mock thành dependency injection
     * (function pointer thay vì struct phẳng) - không cần thiết cho
     * mục tiêu unit test logic điều khiển thanh ghi ở tầng này. */
    i2c.SR1 = I2C_SR1_SB | I2C_SR1_ADDR | I2C_SR1_TXE | I2C_SR1_BTF | I2C_SR1_RXNE;

    uint8_t value = 0U;
    TEST_ASSERT_EQUAL(I2C_OK, I2C_ReadRegister(&i2c, 0x50U, 0x10U, &value));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_gpio_config_and_toggle);
    RUN_TEST(test_pwm_init_and_duty);
    RUN_TEST(test_spi_init_and_transfer);
    RUN_TEST(test_timer_init_and_control);
    RUN_TEST(test_uart_init_and_transfer);
    RUN_TEST(test_i2c_init_and_register_access);
    return UNITY_END();
}