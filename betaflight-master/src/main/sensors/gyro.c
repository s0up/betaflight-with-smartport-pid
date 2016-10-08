/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "platform.h"
#include "debug.h"

#include "common/axis.h"
#include "common/maths.h"
#include "common/filter.h"

#include "drivers/sensor.h"
#include "drivers/system.h"
#include "drivers/accgyro.h"

#include "io/beeper.h"
#include "io/statusindicator.h"

#include "sensors/sensors.h"
#include "sensors/boardalignment.h"
#include "sensors/gyro.h"

gyro_t gyro;                      // gyro access functions
sensor_align_e gyroAlign = 0;

int32_t gyroADC[XYZ_AXIS_COUNT];
float gyroADCf[XYZ_AXIS_COUNT];

static int32_t gyroZero[XYZ_AXIS_COUNT] = { 0, 0, 0 };
static const gyroConfig_t *gyroConfig;
static biquadFilter_t gyroFilterLPF[XYZ_AXIS_COUNT];
static biquadFilter_t gyroFilterNotch_1[XYZ_AXIS_COUNT], gyroFilterNotch_2[XYZ_AXIS_COUNT];
static pt1Filter_t gyroFilterPt1[XYZ_AXIS_COUNT];
static denoisingState_t gyroDenoiseState[XYZ_AXIS_COUNT];
static uint8_t gyroSoftLpfType;
static uint16_t gyroSoftNotchHz_1, gyroSoftNotchHz_2;
static float gyroSoftNotchQ_1, gyroSoftNotchQ_2;
static uint8_t gyroSoftLpfHz;
static uint16_t calibratingG = 0;
static float gyroDt;

void gyroUseConfig(const gyroConfig_t *gyroConfigToUse,
                   uint8_t gyro_soft_lpf_hz,
                   uint16_t gyro_soft_notch_hz_1,
                   uint16_t gyro_soft_notch_cutoff_1,
                   uint16_t gyro_soft_notch_hz_2,
                   uint16_t gyro_soft_notch_cutoff_2,
                   uint8_t gyro_soft_lpf_type)
{
    gyroConfig = gyroConfigToUse;
    gyroSoftLpfHz = gyro_soft_lpf_hz;
    gyroSoftNotchHz_1 = gyro_soft_notch_hz_1;
    gyroSoftNotchHz_2 = gyro_soft_notch_hz_2;
    gyroSoftLpfType = gyro_soft_lpf_type;
    gyroSoftNotchQ_1 = filterGetNotchQ(gyro_soft_notch_hz_1, gyro_soft_notch_cutoff_1);
    gyroSoftNotchQ_2 = filterGetNotchQ(gyro_soft_notch_hz_2, gyro_soft_notch_cutoff_2);
}

void gyroInit(void)
{
    if (gyroSoftLpfHz && gyro.targetLooptime) {  // Initialisation needs to happen once samplingrate is known
        for (int axis = 0; axis < 3; axis++) {
            if (gyroSoftLpfType == FILTER_BIQUAD)
                biquadFilterInitLPF(&gyroFilterLPF[axis], gyroSoftLpfHz, gyro.targetLooptime);
            else
                gyroDt = (float) gyro.targetLooptime * 0.000001f;
        }
    }

    if ((gyroSoftNotchHz_1 || gyroSoftNotchHz_2) && gyro.targetLooptime) {
        for (int axis = 0; axis < 3; axis++) {
            biquadFilterInit(&gyroFilterNotch_1[axis], gyroSoftNotchHz_1, gyro.targetLooptime, gyroSoftNotchQ_1, FILTER_NOTCH);
            biquadFilterInit(&gyroFilterNotch_2[axis], gyroSoftNotchHz_2, gyro.targetLooptime, gyroSoftNotchQ_2, FILTER_NOTCH);
        }
    }
}

bool isGyroCalibrationComplete(void)
{
    return calibratingG == 0;
}

static bool isOnFinalGyroCalibrationCycle(void)
{
    return calibratingG == 1;
}

static uint16_t gyroCalculateCalibratingCycles(void)
{
    return (CALIBRATING_GYRO_CYCLES / gyro.targetLooptime) * CALIBRATING_GYRO_CYCLES;
}

static bool isOnFirstGyroCalibrationCycle(void)
{
    return calibratingG == gyroCalculateCalibratingCycles();
}

void gyroSetCalibrationCycles(void)
{
    calibratingG = gyroCalculateCalibratingCycles();
}

static void performAcclerationCalibration(uint8_t gyroMovementCalibrationThreshold)
{
    static int32_t g[3];
    static stdev_t var[3];

    for (int axis = 0; axis < 3; axis++) {

        // Reset g[axis] at start of calibration
        if (isOnFirstGyroCalibrationCycle()) {
            g[axis] = 0;
            devClear(&var[axis]);
        }

        // Sum up CALIBRATING_GYRO_CYCLES readings
        g[axis] += gyroADC[axis];
        devPush(&var[axis], gyroADC[axis]);

        // Reset global variables to prevent other code from using un-calibrated data
        gyroADC[axis] = 0;
        gyroZero[axis] = 0;

        if (isOnFinalGyroCalibrationCycle()) {
            float dev = devStandardDeviation(&var[axis]);
            // check deviation and startover in case the model was moved
            if (gyroMovementCalibrationThreshold && dev > gyroMovementCalibrationThreshold) {
                gyroSetCalibrationCycles();
                return;
            }
            gyroZero[axis] = (g[axis] + (gyroCalculateCalibratingCycles() / 2)) / gyroCalculateCalibratingCycles();
        }
    }

    if (isOnFinalGyroCalibrationCycle()) {
        beeper(BEEPER_GYRO_CALIBRATED);
    }
    calibratingG--;

}

static void applyGyroZero(void)
{
    for (int axis = 0; axis < 3; axis++) {
        gyroADC[axis] -= gyroZero[axis];
    }
}

void gyroUpdate(void)
{
    int16_t gyroADCRaw[XYZ_AXIS_COUNT];

    // range: +/- 8192; +/- 2000 deg/sec
    if (!gyro.read(gyroADCRaw)) {
        return;
    }

    for (int axis = 0; axis < XYZ_AXIS_COUNT; axis++) {
        gyroADC[axis] = gyroADCRaw[axis];
    }

    alignSensors(gyroADC, gyroADC, gyroAlign);

    if (!isGyroCalibrationComplete()) {
        performAcclerationCalibration(gyroConfig->gyroMovementCalibrationThreshold);
    }

    applyGyroZero();

    if (gyroSoftLpfHz) {
        for (int axis = 0; axis < XYZ_AXIS_COUNT; axis++) {

            if (debugMode == DEBUG_GYRO)
                debug[axis] = gyroADC[axis];

            if (gyroSoftLpfType == FILTER_BIQUAD)
                gyroADCf[axis] = biquadFilterApply(&gyroFilterLPF[axis], (float) gyroADC[axis]);
            else if (gyroSoftLpfType == FILTER_PT1)
                gyroADCf[axis] = pt1FilterApply4(&gyroFilterPt1[axis], (float) gyroADC[axis], gyroSoftLpfHz, gyroDt);
            else
                gyroADCf[axis] = denoisingFilterUpdate(&gyroDenoiseState[axis], (float) gyroADC[axis]);

            if (debugMode == DEBUG_NOTCH)
                debug[axis] = lrintf(gyroADCf[axis]);

            if (gyroSoftNotchHz_1)
                gyroADCf[axis] = biquadFilterApply(&gyroFilterNotch_1[axis], gyroADCf[axis]);

            if (gyroSoftNotchHz_2)
                gyroADCf[axis] = biquadFilterApply(&gyroFilterNotch_2[axis], gyroADCf[axis]);

            gyroADC[axis] = lrintf(gyroADCf[axis]);
        }
    } else {
        for (int axis = 0; axis < XYZ_AXIS_COUNT; axis++)
            gyroADCf[axis] = gyroADC[axis];
    }
}
