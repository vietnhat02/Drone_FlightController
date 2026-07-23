#ifndef COMPASS_PROCESSING_H
#define COMPASS_PROCESSING_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    COMPASS_OK = 0U,
    COMPASS_ERROR = 1U,
    COMPASS_ERROR_NOT_CALIBRATED = 2U,
} Compass_Status_t;

/* ID dùng để lưu/đọc qua calibration_storage - PHẢI duy nhất giữa các sensor
 * dùng chung module lưu trữ (compass, IMU, baro...) */
#define COMPASS_CALSTORE_SENSOR_ID   1U

/* Dữ liệu hiệu chuẩn hard-iron / soft-iron - đây chính là struct sẽ được
 * truyền vào CalStorage_Save()/CalStorage_Load() (xem calibration_storage.h) */
typedef struct {
    float offset_x;
    float offset_y;
    float offset_z;
    float scale_x;
    float scale_y;
    float scale_z;
    bool  isValid;
} Compass_Calibration_t;

typedef struct {
    int16_t x_min, x_max;
    int16_t y_min, y_max;
    int16_t z_min, z_max;
    uint32_t sampleCount;
} Compass_CalibrationSession_t;

typedef struct {
    float x, y, z;
    bool  isInitialized;
} Compass_Filter_t;

/* ============ Calibration ============ */
void Compass_CalibrationSession_Start(Compass_CalibrationSession_t *session);
void Compass_CalibrationSession_Update(Compass_CalibrationSession_t *session,
                                        int16_t x, int16_t y, int16_t z);
Compass_Status_t Compass_CalibrationSession_Finish(const Compass_CalibrationSession_t *session,
                                                    Compass_Calibration_t *outCal);

void Compass_ApplyCalibration(const Compass_Calibration_t *cal,
                               int16_t x_raw, int16_t y_raw, int16_t z_raw,
                               float *x_out, float *y_out, float *z_out);

/* ============ Filtering ============ */
void Compass_Filter_Init(Compass_Filter_t *filter);
void Compass_Filter_Update(Compass_Filter_t *filter, float alpha,
                            float x_in, float y_in, float z_in,
                            float *x_out, float *y_out, float *z_out);

/* ============ Heading ============ */
float Compass_ComputeHeadingRaw(float x, float y);
float Compass_ComputeHeadingTiltCompensated(float x, float y, float z,
                                             float rollRad, float pitchRad);
float Compass_ApplyDeclination(float headingRad, float declinationRad);

/* ============ Helper ============ */
float Compass_RadToDeg(float rad);
float Compass_NormalizeDeg(float deg);

#endif