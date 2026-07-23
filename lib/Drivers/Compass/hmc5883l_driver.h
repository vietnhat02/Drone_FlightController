#ifndef HMC5883L_DRIVER_H
#define HMC5883L_DRIVER_H

#include "i2c_driver.h"
#include <stdint.h>

typedef enum {
    HMC_OK = 0U,
    HMC_ERROR = 1U,          /* tham số đầu vào không hợp lệ (con trỏ NULL) */
    HMC_ERROR_WRITE = 2U,    /* lỗi khi ghi thanh ghi cấu hình lúc init */
    HMC_ERROR_READ = 3U,     /* lỗi I2C khi đọc dữ liệu X/Y/Z */
    HMC_ERROR_ID = 4U,       /* Identification Register không khớp 'H','4','3' */
} HMC_Status_t;

#define HMC5883L_I2C_ADDR_DEFAULT   0x1EU

/* ---- Configuration Register A: Samples Averaged ---- */
#define HMC5883L_CRA_AVG_1          0x00U
#define HMC5883L_CRA_AVG_2          0x20U
#define HMC5883L_CRA_AVG_4          0x40U
#define HMC5883L_CRA_AVG_8          0x60U

/* ---- Configuration Register A: Data Output Rate ---- */
#define HMC5883L_CRA_ODR_0_75HZ     0x00U
#define HMC5883L_CRA_ODR_1_5HZ      0x04U
#define HMC5883L_CRA_ODR_3HZ        0x08U
#define HMC5883L_CRA_ODR_7_5HZ      0x0CU
#define HMC5883L_CRA_ODR_15HZ       0x10U   /* mặc định khuyến nghị */
#define HMC5883L_CRA_ODR_30HZ       0x14U
#define HMC5883L_CRA_ODR_75HZ       0x18U

/* ---- Configuration Register A: Measurement Mode ---- */
#define HMC5883L_CRA_MEAS_NORMAL    0x00U

/* ---- Configuration Register B: Gain (độ nhạy) ---- */
#define HMC5883L_CRB_GAIN_0_88GA    0x00U
#define HMC5883L_CRB_GAIN_1_3GA     0x20U   /* mặc định khuyến nghị, 1090 LSB/Gauss */
#define HMC5883L_CRB_GAIN_1_9GA     0x40U
#define HMC5883L_CRB_GAIN_2_5GA     0x60U
#define HMC5883L_CRB_GAIN_4_0GA     0x80U
#define HMC5883L_CRB_GAIN_4_7GA     0xA0U
#define HMC5883L_CRB_GAIN_5_6GA     0xC0U
#define HMC5883L_CRB_GAIN_8_1GA     0xE0U

/* ---- Mode Register ---- */
#define HMC5883L_MODE_CONTINUOUS    0x00U
#define HMC5883L_MODE_SINGLE        0x01U
#define HMC5883L_MODE_IDLE          0x02U

/* ---- Status Register bit ---- */
#define HMC5883L_STATUS_RDY_MASK    0x01U
#define HMC5883L_STATUS_LOCK_MASK   0x02U



/* ---- Register map (theo datasheet Honeywell HMC5883L) ---- */
#define HMC5883L_REG_CONFIG_A       0x00U
#define HMC5883L_REG_CONFIG_B       0x01U
#define HMC5883L_REG_MODE           0x02U
#define HMC5883L_REG_DATA_X_MSB     0x03U   /* điểm bắt đầu đọc burst 6 byte */
#define HMC5883L_REG_DATA_X_LSB     0x04U
#define HMC5883L_REG_DATA_Z_MSB     0x05U
#define HMC5883L_REG_DATA_Z_LSB     0x06U
#define HMC5883L_REG_DATA_Y_MSB     0x07U
#define HMC5883L_REG_DATA_Y_LSB     0x08U
#define HMC5883L_REG_STATUS         0x09U   /* read-only */
#define HMC5883L_REG_ID_A           0x0AU   /* phải đọc ra 'H' = 0x48 */
#define HMC5883L_REG_ID_B           0x0BU   /* phải đọc ra '4' = 0x34 */
#define HMC5883L_REG_ID_C           0x0CU   /* phải đọc ra '3' = 0x33 */

#define HMC5883L_ID_A_EXPECTED      0x48U
#define HMC5883L_ID_B_EXPECTED      0x34U
#define HMC5883L_ID_C_EXPECTED      0x33U


HMC_Status_t HMC5883L_Init(I2C_TypeDef *i2c, uint8_t slaveAddress);
HMC_Status_t HMC5883L_CheckId(I2C_TypeDef *i2c);
HMC_Status_t HMC5883L_ReadRaw(I2C_TypeDef *i2c, int16_t *x, int16_t *y, int16_t *z);
HMC_Status_t HMC5883L_ReadStatus(I2C_TypeDef *i2c, uint8_t *status);

#endif // HMC5883L_DRIVER_H