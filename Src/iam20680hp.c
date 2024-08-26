/*

MIT License

Copyright (c) 2024 Rémy Hurx

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Be careful with the scd41 instructions to stop almost always periodic measurement!
Some delays are neccessary for the instructions to work, change to osdelay for rtos.
Some texts are copied from the datasheet of the sensor, to make it easier to understand the functions.

*/

#include "iam20680hp.h"

// Location of the I2C handler, or elsewhere
#include "main.h"

extern I2C_HandleTypeDef I2C_HANDLER;

HAL_StatusTypeDef status;
IAM20680HP_err_t iam20680hpStatus;

uint8_t data[20];


IAM20680HP_err_t iam20680hpCheckDeviceID()
{
    uint8_t deviceID;

    data[0] = IAM20680HP_WHO_AM_I;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), &deviceID, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    // Expect back the value 0xF8
    if (deviceID != IAM20680HP_DEVICE_ID)
    {
        return IAM20680HP_ERR_DEVICE_ID;
    }
    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpResetDevice()
{

    // For the IAM20680HP, the PWR_MGMT_1 register is used to reset the device, set to "or 0x80" to reset
    data[0] = IAM20680HP_PWR_MGMT_1;
    data[1] = 0x81;
    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);

    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    HAL_Delay(50);
    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadSelfTestRegisters(IAM20680HP_selfTest_t *selfTest)
{

    data[0] = IAM20680HP_SELF_TEST_X_GYRO;
    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 6);
    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 6, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    selfTest->selfTestXGyro = data[0];
    selfTest->selfTestYGyro = data[1];
    selfTest->selfTestZGyro = data[2];
    selfTest->selfTestXAccel = data[3];
    selfTest->selfTestYAccel = data[4];
    selfTest->selfTestZAccel = data[5];

    // TODO - Function to check the values against the given formula/values

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpGyroOffsetAdjustment(IAM20680HP_gyroOffset_t *gyroOffset, bool writeOffset)
{

    if (writeOffset)
    {
        data[0] = IAM20680HP_XG_OFFS_USRH;
        data[1] = (uint8_t)(gyroOffset->offsetXGyro >> 8);
        data[2] = (uint8_t)(gyroOffset->offsetXGyro);
        data[3] = (uint8_t)(gyroOffset->offsetYGyro >> 8);
        data[4] = (uint8_t)(gyroOffset->offsetYGyro);
        data[5] = (uint8_t)(gyroOffset->offsetZGyro >> 8);
        data[6] = (uint8_t)(gyroOffset->offsetZGyro);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 7, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_XG_OFFS_USRH;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        memset(data, 0, 6);
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 6, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        gyroOffset->offsetXGyro = (int16_t)(data[0] << 8 | data[1]);
        gyroOffset->offsetYGyro = (int16_t)(data[2] << 8 | data[3]);
        gyroOffset->offsetZGyro = (int16_t)(data[4] << 8 | data[5]);
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680SampleRateDivider(uint8_t *sampleRateDivider, bool writeDivider)
{
    if (writeDivider)
    {
        data[0] = IAM20680HP_SMPLRT_DIV;
        data[1] = *sampleRateDivider;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_SMPLRT_DIV;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *sampleRateDivider = data[0];
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpConfigFifo(bool *fifoEnabled, bool writeFifo)
{
    if (writeFifo)
    {
        data[0] = IAM20680HP_CONFIG;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        if (*fifoEnabled)
        {
            data[1] = data[0] | 0x40; // 0b01000000;
        }
        else
        {
            data[1] = data[0] & 0xBF; // 0b10111111;
        }

        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        if (data[0] & 0x40)
        {
            *fifoEnabled = true;
        }
        else
        {
            *fifoEnabled = false;
        }
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpConfigExtSync(uint8_t *extSyncSet, bool writeExtSyncSet)
{
    if (*extSyncSet > 7)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeExtSyncSet)
    {
        data[0] = IAM20680HP_CONFIG;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = data[0] & 0xC7; // 0b11000111;
        data[1] = data[0] | (*extSyncSet << 3);

        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *extSyncSet = (data[0] & 0x38) >> 3; // 0b00111000;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpConfigDlpfCfg(uint8_t *dlpf, bool writeDlpf)
{
    if (*dlpf > 7)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeDlpf)
    {
        data[0] = IAM20680HP_CONFIG;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = data[0] & 0xF8; // 0b11111000;
        data[1] = data[0] | *dlpf;
        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *dlpf = data[0] & 0x07; // 0b00000111;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpGyroConfig(IAM20680HP_gyroConfig_t *gyro, bool writeConfig)
{
    if (gyro->FS_Sel > 3)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (gyro->FChoice > 2)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeConfig)
    {
        data[0] = IAM20680HP_GYRO_CONFIG;
        data[1] = 0;
        data[1] = data[1] | (gyro->xGyroSelfTest << 7);
        data[1] = data[1] | (gyro->yGyroSelfTest << 6);
        data[1] = data[1] | (gyro->zGyroSelfTest << 5);
        data[1] = data[1] | (gyro->FS_Sel << 3);
        data[1] = data[1] | (gyro->FChoice << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_GYRO_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        gyro->xGyroSelfTest = (data[0] & 0x80) >> 7; // 0b10000000;
        gyro->yGyroSelfTest = (data[0] & 0x40) >> 6; // 0b01000000;
        gyro->zGyroSelfTest = (data[0] & 0x20) >> 5; // 0b00100000;
        gyro->FS_Sel = (data[0] & 0x18) >> 3;        // 0b00011000;
        gyro->FChoice = (data[0] & 0x03);            // 0b00000011;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpAccelConfig(IAM20680HP_accelConfig_t *accel, bool writeConfig)
{
    if (accel->AFS_Sel > 3)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }
    if (accel->fifoSize > 3)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }
    if (accel->dec2Cfg > 3)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }
    if (accel->dlpfCfg > 7)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeConfig)
    {
        data[0] = IAM20680HP_ACCEL_CONFIG;
        data[1] = 0;
        data[1] = data[1] | (accel->xAccelSelfTest << 7);
        data[1] = data[1] | (accel->yAccelSelfTest << 6);
        data[1] = data[1] | (accel->zAccelSelfTest << 5);
        data[1] = data[1] | (accel->AFS_Sel << 3);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = IAM20680HP_ACCEL_CONFIG2;
        data[1] = 0;
        data[1] = data[1] | (accel->fifoSize << 6);
        data[1] = data[1] | (accel->dec2Cfg << 4);
        data[1] = data[1] | (accel->FChoice << 3);
        data[1] = data[1] | (accel->dlpfCfg << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_ACCEL_CONFIG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        accel->xAccelSelfTest = (data[0] & 0x80) >> 7; // 0b10000000;
        accel->yAccelSelfTest = (data[0] & 0x40) >> 6; // 0b01000000;
        accel->zAccelSelfTest = (data[0] & 0x20) >> 5; // 0b00100000;
        accel->AFS_Sel = (data[0] & 0x18) >> 3;        // 0b00011000;

        accel->fifoSize = (data[1] & 0xC0) >> 6; // 0b11000000;
        accel->dec2Cfg = (data[1] & 0x30) >> 4;  // 0b00110000;
        accel->FChoice = (data[1] & 0x08) >> 3;  // 0b00001000;
        accel->dlpfCfg = (data[1] & 0x07);       // 0b00000111;
    }
    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpLowPowerMode(bool *enableLPM, uint8_t *avgFilterCfg, uint8_t *womMode, bool writeConfig)
{
    if (*avgFilterCfg > 3)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }
    if (*womMode > 11)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }
    if (*womMode < 4 && *womMode != 0)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeConfig)
    {
        data[0] = IAM20680HP_LP_MODE_CFG;
        data[1] = 0;
        data[1] = data[1] | (*enableLPM << 7);
        data[1] = data[1] | (*avgFilterCfg << 4);
        data[1] = data[1] | (*womMode << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_LP_MODE_CFG;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *enableLPM = (data[0] & 0x80) >> 7;    // 0b10000000;
        *avgFilterCfg = (data[0] & 0x70) >> 4; // 0b01110000;
        *womMode = (data[0] & 0x0F);           // 0b00001111;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpWakeOnMotionThreshold(uint8_t *womThreshold, bool writeWomThreshold)
{

    if (writeWomThreshold)
    {
        data[0] = IAM20680HP_ACCEL_WOM_THR;
        data[1] = *womThreshold;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_ACCEL_WOM_THR;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *womThreshold = data[0];
    }
    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpFiFoEnable(bool *tempFiFo, bool *gyroX, bool *gyroY, bool *gyroZ, bool *accel, bool writeFiFo)
{
    if (writeFiFo)
    {
        data[0] = IAM20680HP_FIFO_EN;
        data[1] = 0;
        data[1] = data[1] | (*tempFiFo << 7);
        data[1] = data[1] | (*gyroX << 6);
        data[1] = data[1] | (*gyroY << 5);
        data[1] = data[1] | (*gyroZ << 4);
        data[1] = data[1] | (*accel << 3);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_FIFO_EN;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *tempFiFo = (data[0] & 0x80) >> 7; // 0b10000000;
        *gyroX = (data[0] & 0x40) >> 6;    // 0b01000000;
        *gyroY = (data[0] & 0x20) >> 5;    // 0b00100000;
        *gyroZ = (data[0] & 0x10) >> 4;    // 0b00010000;
        *accel = (data[0] & 0x08) >> 3;    // 0b00001000;
    }
    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadFsyncInterruptStatus(bool *fsyncInt)
{
    data[0] = IAM20680HP_FSYNC_INT;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    if (data[0] & 0x80)
    {
        *fsyncInt = true;
    }
    else
    {
        *fsyncInt = false;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpIntConfig(IAM20680HP_intPinConfig_t *intPin, bool writeConfig)
{
    if (writeConfig)
    {
        data[0] = IAM20680HP_INT_PIN_CFG;
        data[1] = 0;
        data[1] = data[1] | (intPin->int_level << 7);
        data[1] = data[1] | (intPin->int_open << 6);
        data[1] = data[1] | (intPin->latch_int_en << 5);
        data[1] = data[1] | (intPin->int_rd_clear << 4);
        data[1] = data[1] | (intPin->fSync_int_level << 3);
        data[1] = data[1] | (intPin->fSync_int_mode_en << 2);
        data[2] = 0;
        if (intPin->wom_int_en == 1)
        {
            data[2] = data[2] | 0xE0; // 0b11100000;
        }
        data[2] = data[2] | (intPin->fifo_oflow_en << 4);
        data[2] = data[2] | (intPin->gdrive_int_en << 2);
        data[2] = data[2] | (intPin->data_rdy_en << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 3, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_INT_PIN_CFG;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        intPin->int_level = (data[0] & 0x80) >> 7;         // 0b10000000;
        intPin->int_open = (data[0] & 0x40) >> 6;          // 0b01000000;
        intPin->latch_int_en = (data[0] & 0x20) >> 5;      // 0b00100000;
        intPin->int_rd_clear = (data[0] & 0x10) >> 4;      // 0b00010000;
        intPin->fSync_int_level = (data[0] & 0x08) >> 3;   // 0b00001000;
        intPin->fSync_int_mode_en = (data[0] & 0x04) >> 2; // 0b00000100;

        intPin->wom_int_en = data[1] & 0xE0;    // 0b11100000;
        intPin->fifo_oflow_en = data[1] & 0x10; // 0b00010000;
        intPin->gdrive_int_en = data[1] & 0x04; // 0b00000100;
        intPin->data_rdy_en = data[1] & 0x01;   // 0b00000001;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpIntStatus(IAM20680HP_intStatus_t *intStatus)
{

    data[0] = IAM20680HP_INT_STATUS;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    intStatus->wom_int = (data[0] & 0xE0) >> 5;        // 0b11100000;
    intStatus->fifo_oflow_int = (data[0] & 0x10) >> 4; // 0b00010000;
    intStatus->gdrive_int = (data[0] & 0x04) >> 2;     // 0b00000100;
    intStatus->data_rdy_int = (data[0] & 0x01);        // 0b00000001;

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadAccelData(IAM20680HP_accelData_t *accelData)
{
    data[0] = IAM20680HP_ACCEL_XOUT_H;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 6);
    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 6, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    accelData->xAccel = (int16_t)(data[0] << 8 | data[1]);
    accelData->yAccel = (int16_t)(data[2] << 8 | data[3]);
    accelData->zAccel = (int16_t)(data[4] << 8 | data[5]);

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadTemperatureData(int16_t *temperature)
{
    data[0] = IAM20680HP_TEMP_OUT_H;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 2);
    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    *temperature = (int16_t)(data[0] << 8 | data[1]);
    *temperature = ((*temperature / 326.8) + 25) * 100;

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadGyroData(IAM20680HP_gyroData_t *gyroData)
{
    data[0] = IAM20680HP_GYRO_XOUT_H;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 6);
    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 6, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    gyroData->xGyro = (int16_t)(data[0] << 8 | data[1]);
    gyroData->yGyro = (int16_t)(data[2] << 8 | data[3]);
    gyroData->zGyro = (int16_t)(data[4] << 8 | data[5]);

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpSignalPathReset(bool *accel, bool *temp, bool writeReset)
{

    if (writeReset)
    {
        data[0] = IAM20680HP_SIGNAL_PATH_RESET;
        data[1] = 0;
        data[1] = data[1] | (*accel << 1);
        data[1] = data[1] | *temp;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_SIGNAL_PATH_RESET;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *accel = (data[0] & 0x02) >> 1; // 0b00000010;
        *temp = (data[0] & 0x01);       // 0b00000001;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpIntelControl(bool *enable, bool *mode, bool writeConfig)
{
    if (writeConfig)
    {
        data[0] = IAM20680HP_ACCEL_INTEL_CTRL;
        data[1] = 0;
        data[1] = data[1] | (*enable << 7);
        data[1] = data[1] | (*mode << 6);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_ACCEL_INTEL_CTRL;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        *enable = (data[0] & 0x80) >> 7; // 0b10000000;
        *mode = (data[0] & 0x40) >> 6;   // 0b01000000;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpUserControl(IAM20680HP_userControl_t *userControl, bool writeConfig)
{
    if (writeConfig)
    {
        data[0] = IAM20680HP_USER_CTRL;
        data[1] = 0;
        data[1] = data[1] | (userControl->fifo_en << 6);
        data[1] = data[1] | (userControl->i2c_if_dis << 4);
        data[1] = data[1] | (userControl->fifo_rst << 2);
        data[1] = data[1] | (userControl->sig_cond_rst << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_USER_CTRL;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        data[0] = 0;

        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        userControl->fifo_en = (data[0] & 0x40) >> 6;    // 0b01000000;
        userControl->i2c_if_dis = (data[0] & 0x10) >> 4; // 0b00010000;
        userControl->fifo_rst = (data[0] & 0x04) >> 2;   // 0b00000100;
        userControl->sig_cond_rst = (data[0] & 0x01);    // 0b00000001;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpPowerManagement(IAM20680HP_powerManagement_t *powerManagement, bool writeConfig)
{
    if (powerManagement->clockSel > 7)
    {
        return IAM20680HP_ERR_INVALID_PARAM;
    }

    if (writeConfig)
    {
        data[0] = IAM20680HP_PWR_MGMT_1;
        data[1] = 0;
        data[1] = data[1] | (powerManagement->reset << 7);
        data[1] = data[1] | (powerManagement->sleep << 6);
        data[1] = data[1] | (powerManagement->accelCycle << 5);
        data[1] = data[1] | (powerManagement->gyroStandby << 4);
        data[1] = data[1] | (powerManagement->tempDis << 3);
        data[1] = data[1] | (powerManagement->clockSel << 0);
        data[2] = 0;
        data[2] = data[2] | (powerManagement->fifo_lp_en << 7);
        data[2] = data[2] | (powerManagement->stby_xa << 5);
        data[2] = data[2] | (powerManagement->stby_ya << 4);
        data[2] = data[2] | (powerManagement->stby_za << 3);
        data[2] = data[2] | (powerManagement->stby_xg << 2);
        data[2] = data[2] | (powerManagement->stby_yg << 1);
        data[2] = data[2] | (powerManagement->stby_zg << 0);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 3, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {
        data[0] = IAM20680HP_PWR_MGMT_1;

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        memset(data, 0, 2);
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        powerManagement->reset = (data[0] & 0x80) >> 7;       // 0b10000000;
        powerManagement->sleep = (data[0] & 0x40) >> 6;       // 0b01000000;
        powerManagement->accelCycle = (data[0] & 0x20) >> 5;  // 0b00100000;
        powerManagement->gyroStandby = (data[0] & 0x10) >> 4; // 0b00010000;
        powerManagement->tempDis = (data[0] & 0x08) >> 3;     // 0b00001000;
        powerManagement->clockSel = (data[0] & 0x07);         // 0b00000111;

        powerManagement->fifo_lp_en = (data[1] & 0x80) >> 7; // 0b10000000;
        powerManagement->stby_xa = (data[1] & 0x20) >> 5;    // 0b00100000;
        powerManagement->stby_ya = (data[1] & 0x10) >> 4;    // 0b00010000;
        powerManagement->stby_za = (data[1] & 0x08) >> 3;    // 0b00001000;
        powerManagement->stby_xg = (data[1] & 0x04) >> 2;    // 0b00000100;
        powerManagement->stby_yg = (data[1] & 0x02) >> 1;    // 0b00000010;
        powerManagement->stby_zg = (data[1] & 0x01);         // 0b00000001;
    }

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadFifoCount(uint16_t *fifoCount)
{
    data[0] = IAM20680HP_FIFO_COUNTH;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 2);
    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 2, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    *fifoCount = (uint16_t)(data[0] << 8 | data[1]);

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpReadFifoData(IAM20680HP_fifoData_t *fifoData)
{
    data[0] = IAM20680HP_FIFO_R_W;

    status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    memset(data, 0, 14);

    status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 14, IAM20680HP_I2C_TIMEOUT);
    if (status != HAL_OK)
    {
        return IAM20680HP_ERR_I2C;
    }

    // Data integrity check
    for (size_t i = 0; i < 14; i++)
    {
        if (data[i] == 0x00)
        {
            return IAM20680HP_ERR_NOT_INITIALIZED;
        }
        if (data[i] == 0xFF)
        {
            return IAM20680HP_ERR_EOL;
        }
    }

    fifoData->accelData.xAccel = (int16_t)(data[0] << 8 | data[1]);
    fifoData->accelData.yAccel = (int16_t)(data[2] << 8 | data[3]);
    fifoData->accelData.zAccel = (int16_t)(data[4] << 8 | data[5]);
    fifoData->temperature = (int16_t)(data[6] << 8 | data[7]);
    fifoData->temperature = ((fifoData->temperature / 326.8) + 25) * 100;
    fifoData->gyroData.xGyro = (int16_t)(data[8] << 8 | data[9]);
    fifoData->gyroData.yGyro = (int16_t)(data[10] << 8 | data[11]);
    fifoData->gyroData.zGyro = (int16_t)(data[12] << 8 | data[13]);

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpAccelerometerOffset(IAM20680HP_accelOffset_t *offSet, bool writeConfig)
{
    if (writeConfig)
    {
        memset(data, 0, 7);
        data[0] = IAM20680HP_XA_OFFSET_H;

        offSet->offsetXAccel <<= 1;
        uint16_t tempData = (uint16_t)offSet->offsetXAccel;
        data[1] = (uint8_t)(tempData >> 8);
        data[2] = (uint8_t)(tempData & 0xFF);

        offSet->offsetYAccel <<= 1;
        tempData = (uint16_t)offSet->offsetYAccel;
        data[3] = (uint8_t)(tempData >> 8);
        data[4] = (uint8_t)(tempData & 0xFF);

        offSet->offsetZAccel <<= 1;
        tempData = (uint16_t)offSet->offsetZAccel;
        data[5] = (uint8_t)(tempData >> 8);
        data[6] = (uint8_t)(tempData & 0xFF);

        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 7, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }
    }
    else
    {

        data[0] = IAM20680HP_XA_OFFSET_H;
        status = HAL_I2C_Master_Transmit(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 1, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        memset(data, 0, 6);
        status = HAL_I2C_Master_Receive(&I2C_HANDLER, (uint16_t)(IAM20680HP_I2C_ADDRESS << 1), data, 6, IAM20680HP_I2C_TIMEOUT);
        if (status != HAL_OK)
        {
            return IAM20680HP_ERR_I2C;
        }

        offSet->offsetXAccel = (int16_t)(data[0] << 8 | data[1]);
        offSet->offsetXAccel >>= 1;

        offSet->offsetYAccel = (int16_t)(data[2] << 8 | data[3]);
        offSet->offsetYAccel >>= 1;

        offSet->offsetZAccel = (int16_t)(data[4] << 8 | data[5]);
        offSet->offsetZAccel >>= 1;
    }

    return IAM20680HP_OK;
}





IAM20680HP_err_t iam20680hpInit()
{
    IAM20680HP_err_t result;

    // Sequence is first to reset device, see page 23 of datasheet
    result = iam20680hpResetDevice();
    if (result != IAM20680HP_OK)
        return result;
    
    // Then to check if the device is present
    result = iam20680hpCheckDeviceID();
    if (result != IAM20680HP_OK)
        return result;  

    // Output data selection
    uint8_t sampleRateDivider = SAMPLE_RATE_DIV;
    result = iam20680SampleRateDivider(&sampleRateDivider, true); 
    if (result != IAM20680HP_OK)
        return result;

    // Config dlpf low pass filter setting
    uint8_t dlpf = LOW_PASS_FILTER_DLPF_CFG;
    result = iam20680hpConfigDlpfCfg(&dlpf, true);
    if (result != IAM20680HP_OK)
        return result;

    // Standard settings for gyro and accelerometer
    IAM20680HP_gyroConfig_t gyroConfig;
    memset(&gyroConfig, 0, sizeof(gyroConfig));
    iam20680hpGyroConfig(&gyroConfig, false);
    gyroConfig.FS_Sel = GYRO_FS_SEL;
    gyroConfig.FChoice = GYRO_FCHOICE;
    iam20680hpGyroConfig(&gyroConfig, true);

    IAM20680HP_accelConfig_t accelConfig;
    memset(&accelConfig, 0, sizeof(accelConfig));
    iam20680hpAccelConfig(&accelConfig, false);
    accelConfig.AFS_Sel = ACCEL_FS_SEL;
    accelConfig.FChoice = ACCEL_FCHOICE;
    accelConfig.dlpfCfg = ACCEL_DLPF_CFG;
    accelConfig.fifoSize = ACCEL_FIFO_SIZE;
    accelConfig.dec2Cfg = ACCEL_DEC2_CFG;
    iam20680hpAccelConfig(&accelConfig, true);

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpEnableWomModeFunction()
{
    IAM20680HP_err_t result;

    // On the basis of AN-000409, WoM wake on motion is enabled

    // Device reset at init device, not on re-enable

    // Gyro and accel are put off
    IAM20680HP_powerManagement_t powerManagement;
    memset(&powerManagement, 0, sizeof(powerManagement));
    result = iam20680hpPowerManagement(&powerManagement, false);
    if (result != IAM20680HP_OK)
        return result;

    powerManagement.stby_xg = 1;
    powerManagement.stby_yg = 1;
    powerManagement.stby_zg = 1;
    powerManagement.stby_xa = 1;
    powerManagement.stby_ya = 1;
    powerManagement.stby_za = 1;
    result = iam20680hpPowerManagement(&powerManagement, true);
    if (result != IAM20680HP_OK)
        return result;

    // Accelerometer configuration
    // set ACCEL_FCHOICE_B = 0 and A_DLPF_CFG[2:0] = 7 (b111)
    IAM20680HP_accelConfig_t accelConfig;
    memset(&accelConfig, 0, sizeof(accelConfig));
    result = iam20680hpAccelConfig(&accelConfig, false);
    if (result != IAM20680HP_OK)
        return result;

    accelConfig.FChoice = 0;
    accelConfig.dlpfCfg = 0x05; // See table 19 of general datasheet or AN-000409
    result = iam20680hpAccelConfig(&accelConfig, true);
    if (result != IAM20680HP_OK)
        return result;

    // Interruption settings
    IAM20680HP_intPinConfig_t intPinConfig;
    memset(&intPinConfig, 0, sizeof(intPinConfig));

    result = iam20680hpIntConfig(&intPinConfig, false);
    if (result != IAM20680HP_OK)
        return result;

    intPinConfig.int_level = DEF_INT_LEVEL;
    intPinConfig.int_open = DEF_INT_OPEN;
    intPinConfig.wom_int_en = 1; // Has to be enabled anyways
    intPinConfig.latch_int_en = DEF_LATCH_INT_EN;
    intPinConfig.int_rd_clear = DEF_INT_RD_CLEAR;
    intPinConfig.gdrive_int_en = DEF_GDRIVE_INT_EN;
    intPinConfig.fifo_oflow_en = DEF_FIFO_OFLOW_EN;
    intPinConfig.data_rdy_en = DEF_DATA_RDY_EN;
    result = iam20680hpIntConfig(&intPinConfig, true);
    if (result != IAM20680HP_OK)
        return result;

    // Set threshold. Higher needs more shock-like movement
    uint8_t threshold = WOL_THRESHOLD;
    result = iam20680hpWakeOnMotionThreshold(&threshold, true);
    if (result != IAM20680HP_OK)
        return result;

    // Enable accell intel. modeIntel seems to have to be true.
    bool enableIntel = true;
    bool modeIntel = ACCEL_INTEL_MODE;
    result = iam20680hpIntelControl(&enableIntel, &modeIntel, true);
    if (result != IAM20680HP_OK)
        return result;

    // Gyro off, put accel in low energy mode
    bool gyroEnable = false;
    uint8_t avgCfg = ACCEL_AVG_CFG;
    uint8_t accel_wom = ACCEL_FREQ_WAKEUP;
    result = iam20680hpLowPowerMode(&gyroEnable, &avgCfg, &accel_wom, true);
    if (result != IAM20680HP_OK)
        return result;

    // Put accel cycle on in powermanagement, NOT into sleep mode
    powerManagement.accelCycle = 1;
    powerManagement.stby_xa = 0;
    powerManagement.stby_ya = 0;
    powerManagement.stby_za = 0;
    result = iam20680hpPowerManagement(&powerManagement, true);
    if (result != IAM20680HP_OK)
        return result;

    return IAM20680HP_OK;
}

IAM20680HP_err_t iam20680hpDisableWomModeFunction()
{
    IAM20680HP_err_t result;

    // On the basis of page 24 of the datasheet, WoM wake on motion is enabled
    IAM20680HP_powerManagement_t powerManagement;
    memset(&powerManagement, 0, sizeof(powerManagement));
    iam20680hpPowerManagement(&powerManagement, false);

    // Gyro and accel are put off
    powerManagement.stby_xg = 1;
    powerManagement.stby_yg = 1;
    powerManagement.stby_zg = 1;
    powerManagement.stby_xa = 1;
    powerManagement.stby_ya = 1;
    powerManagement.stby_za = 1;
    iam20680hpPowerManagement(&powerManagement, true);

    // Enable accell intel.
    bool enableIntel = false;
    bool modeIntel = ACCEL_INTEL_MODE;
    iam20680hpIntelControl(&enableIntel, &modeIntel, true);

    // Gyro off, put accel in low energy mode
    bool gyroEnable = true;
    uint8_t avgCfg = ACCEL_AVG_CFG;
    uint8_t accel_wom = ACCEL_FREQ_WAKEUP;
    iam20680hpLowPowerMode(&gyroEnable, &avgCfg, &accel_wom, true);

    // Interruption settings, retreive standards
    IAM20680HP_intPinConfig_t intPinConfig;
    memset(&intPinConfig, 0, sizeof(intPinConfig));
    result = iam20680hpIntConfig(&intPinConfig, false);
    if (result != IAM20680HP_OK)
        return result;

    intPinConfig.int_level = DEF_INT_LEVEL;
    intPinConfig.int_open = DEF_INT_OPEN;
    intPinConfig.wom_int_en = 0;
    intPinConfig.latch_int_en = DEF_LATCH_INT_EN;
    intPinConfig.int_rd_clear = DEF_LATCH_INT_EN;
    intPinConfig.gdrive_int_en = DEF_GDRIVE_INT_EN;
    intPinConfig.fifo_oflow_en = DEF_FIFO_OFLOW_EN;
    intPinConfig.data_rdy_en = DEF_DATA_RDY_EN;
    result = iam20680hpIntConfig(&intPinConfig, true);
    if (result != IAM20680HP_OK)
        return result;

    // Clear interrupts
    IAM20680HP_intStatus_t intStatus;
    iam20680hpIntStatus(&intStatus);

    // Put accel cycle on in powermanagement, NOT into sleep mode
    powerManagement.accelCycle = 0;
    powerManagement.stby_xa = 0;
    powerManagement.stby_ya = 0;
    powerManagement.stby_za = 0;
    powerManagement.stby_xg = 0;
    powerManagement.stby_yg = 0;
    powerManagement.stby_zg = 0;
    result = iam20680hpPowerManagement(&powerManagement, true);
    if (result != IAM20680HP_OK)
        return result;
  
    HAL_Delay(50);

    return IAM20680HP_OK;
}
