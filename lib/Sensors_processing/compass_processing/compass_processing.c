#include "compass_processing.h"
#include <math.h>

#define COMPASS_PI               3.14159265358979323846f
#define COMPASS_MIN_INIT_VALUE   32767
#define COMPASS_MAX_INIT_VALUE   (-32768)
#define COMPASS_MIN_SAMPLES      50U

/* ================================================================
 * CALIBRATION
 * ================================================================ */

void Compass_CalibrationSession_Start(Compass_CalibrationSession_t *session) {
    if (session == NULL) {
        return;
    }
    session->x_min = (int16_t)COMPASS_MIN_INIT_VALUE;
    session->y_min = (int16_t)COMPASS_MIN_INIT_VALUE;
    session->z_min = (int16_t)COMPASS_MIN_INIT_VALUE;
    session->x_max = (int16_t)COMPASS_MAX_INIT_VALUE;
    session->y_max = (int16_t)COMPASS_MAX_INIT_VALUE;
    session->z_max = (int16_t)COMPASS_MAX_INIT_VALUE;
    session->sampleCount = 0U;
}

void Compass_CalibrationSession_Update(Compass_CalibrationSession_t *session,
                                        int16_t x, int16_t y, int16_t z) {
    if (session == NULL) {
        return;
    }

    if (x < session->x_min) { session->x_min = x; }
    if (x > session->x_max) { session->x_max = x; }
    if (y < session->y_min) { session->y_min = y; }
    if (y > session->y_max) { session->y_max = y; }
    if (z < session->z_min) { session->z_min = z; }
    if (z > session->z_max) { session->z_max = z; }

    session->sampleCount++;
}

Compass_Status_t Compass_CalibrationSession_Finish(const Compass_CalibrationSession_t *session,
                                                    Compass_Calibration_t *outCal) {
    float deltaX, deltaY, deltaZ, avgDelta;

    if ((session == NULL) || (outCal == NULL)) {
        return COMPASS_ERROR;
    }
    if (session->sampleCount < COMPASS_MIN_SAMPLES) {
        return COMPASS_ERROR_NOT_CALIBRATED;
    }

    outCal->offset_x = ((float)session->x_min + (float)session->x_max) / 2.0f;
    outCal->offset_y = ((float)session->y_min + (float)session->y_max) / 2.0f;
    outCal->offset_z = ((float)session->z_min + (float)session->z_max) / 2.0f;

    deltaX = ((float)session->x_max - (float)session->x_min) / 2.0f;
    deltaY = ((float)session->y_max - (float)session->y_min) / 2.0f;
    deltaZ = ((float)session->z_max - (float)session->z_min) / 2.0f;

    if ((deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f)) {
        return COMPASS_ERROR_NOT_CALIBRATED;
    }

    avgDelta = (deltaX + deltaY + deltaZ) / 3.0f;

    outCal->scale_x = avgDelta / deltaX;
    outCal->scale_y = avgDelta / deltaY;
    outCal->scale_z = avgDelta / deltaZ;

    outCal->isValid = true;
    return COMPASS_OK;
}

void Compass_ApplyCalibration(const Compass_Calibration_t *cal,
                               int16_t x_raw, int16_t y_raw, int16_t z_raw,
                               float *x_out, float *y_out, float *z_out) {
    if ((cal == NULL) || (x_out == NULL) || (y_out == NULL) || (z_out == NULL)) {
        return;
    }

    if (!cal->isValid) {
        *x_out = (float)x_raw;
        *y_out = (float)y_raw;
        *z_out = (float)z_raw;
        return;
    }

    *x_out = ((float)x_raw - cal->offset_x) * cal->scale_x;
    *y_out = ((float)y_raw - cal->offset_y) * cal->scale_y;
    *z_out = ((float)z_raw - cal->offset_z) * cal->scale_z;
}

/* ================================================================
 * FILTERING
 * ================================================================ */

void Compass_Filter_Init(Compass_Filter_t *filter) {
    if (filter == NULL) {
        return;
    }
    filter->x = 0.0f;
    filter->y = 0.0f;
    filter->z = 0.0f;
    filter->isInitialized = false;
}

void Compass_Filter_Update(Compass_Filter_t *filter, float alpha,
                            float x_in, float y_in, float z_in,
                            float *x_out, float *y_out, float *z_out) {
    if ((filter == NULL) || (x_out == NULL) || (y_out == NULL) || (z_out == NULL)) {
        return;
    }

    if (!filter->isInitialized) {
        filter->x = x_in;
        filter->y = y_in;
        filter->z = z_in;
        filter->isInitialized = true;
    } else {
        filter->x = (alpha * x_in) + ((1.0f - alpha) * filter->x);
        filter->y = (alpha * y_in) + ((1.0f - alpha) * filter->y);
        filter->z = (alpha * z_in) + ((1.0f - alpha) * filter->z);
    }

    *x_out = filter->x;
    *y_out = filter->y;
    *z_out = filter->z;
}

/* ================================================================
 * HEADING
 * ================================================================ */

float Compass_ComputeHeadingRaw(float x, float y) {
    return atan2f(y, x);
}

float Compass_ComputeHeadingTiltCompensated(float x, float y, float z,
                                             float rollRad, float pitchRad) {
    float cosRoll  = cosf(rollRad);
    float sinRoll  = sinf(rollRad);
    float cosPitch = cosf(pitchRad);
    float sinPitch = sinf(pitchRad);

    float xh = (x * cosPitch) + (z * sinPitch);
    float yh = (x * sinRoll * sinPitch) + (y * cosRoll) - (z * sinRoll * cosPitch);

    return atan2f(-yh, xh);
}

float Compass_ApplyDeclination(float headingRad, float declinationRad) {
    float result = headingRad + declinationRad;

    if (result > COMPASS_PI) {
        result -= (2.0f * COMPASS_PI);
    } else if (result < -COMPASS_PI) {
        result += (2.0f * COMPASS_PI);
    }

    return result;
}

/* ================================================================
 * HELPER
 * ================================================================ */

float Compass_RadToDeg(float rad) {
    return rad * (180.0f / COMPASS_PI);
}

float Compass_NormalizeDeg(float deg) {
    while (deg < 0.0f) {
        deg += 360.0f;
    }
    while (deg >= 360.0f) {
        deg -= 360.0f;
    }
    return deg;
}