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

*/

#ifndef IAM20680HP_H_
#define IAM20680HP_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"


/* User: change these */
// 1) Change to correct handler
#define I2C_HANDLER hi2c1

// 2) Change to 1 if logic level is high, 0 if logic level is low (chip has 2 addresses)
#define IAM20680_LOGIC 1

#if IAM20680_LOGIC == 1
#define IAM20680HP_I2C_ADDRESS 0x69
#else
#define IAM20680HP_I2C_ADDRESS 0x68
#endif

// 3) Timeout of I2C communication
#define IAM20680HP_I2C_TIMEOUT 100


//INITIAL CONFIGURATION
#define SAMPLE_RATE_DIV 0x09 //1khz/(1+9) = 100hz
#define LOW_PASS_FILTER_DLPF_CFG 0x01 //Table 18 datasheet
#define GYRO_FS_SEL 0x00
#define GYRO_FCHOICE 0x00
#define ACCEL_FS_SEL 0x00
#define ACCEL_FIFO_SIZE 0x00
#define ACCEL_DEC2_CFG 0x00
#define ACCEL_FCHOICE 0x00
#define ACCEL_DLPF_CFG 0x05 

//WOL SETUP
#define DEF_INT_LEVEL 1
#define DEF_INT_OPEN 0
#define DEF_LATCH_INT_EN 0
#define DEF_INT_RD_CLEAR 0
#define DEF_GDRIVE_INT_EN 0
#define DEF_FIFO_OFLOW_EN 0
#define DEF_DATA_RDY_EN 0

#define WOL_THRESHOLD 0x2
#define ACCEL_INTEL_MODE 0
#define ACCEL_AVG_CFG 0
#define ACCEL_FREQ_WAKEUP 7 //31.3hz table 19 datasheet 



/* User: not recommend to change */

// Result of WHO_AM_I register, if correct I2C device
#define IAM20680HP_DEVICE_ID 0xF8

#define IAM20680HP_SELF_TEST_X_GYRO 0x00
#define IAM20680HP_SELF_TEST_Y_GYRO 0x01
#define IAM20680HP_SELF_TEST_Z_GYRO 0x02
#define IAM20680HP_SELF_TEST_X_ACCEL 0x0D
#define IAM20680HP_SELF_TEST_Y_ACCEL 0x0E
#define IAM20680HP_SELF_TEST_Z_ACCEL 0x0F
#define IAM20680HP_XG_OFFS_USRH 0x13
#define IAM20680HP_XG_OFFS_USRL 0x14
#define IAM20680HP_YG_OFFS_USRH 0x15
#define IAM20680HP_YG_OFFS_USRL 0x16
#define IAM20680HP_ZG_OFFS_USRH 0x17
#define IAM20680HP_ZG_OFFS_USRL 0x18
#define IAM20680HP_SMPLRT_DIV 0x19
#define IAM20680HP_CONFIG 0x1A
#define IAM20680HP_GYRO_CONFIG 0x1B
#define IAM20680HP_ACCEL_CONFIG 0x1C
#define IAM20680HP_ACCEL_CONFIG2 0x1D
#define IAM20680HP_LP_MODE_CFG 0x1E
#define IAM20680HP_ACCEL_WOM_THR 0x1F
#define IAM20680HP_FIFO_EN 0x23
#define IAM20680HP_FSYNC_INT 0x36
#define IAM20680HP_INT_PIN_CFG 0x37
#define IAM20680HP_INT_ENABLE 0x38
#define IAM20680HP_INT_STATUS 0x3A
#define IAM20680HP_ACCEL_XOUT_H 0x3B
#define IAM20680HP_ACCEL_XOUT_L 0x3C
#define IAM20680HP_ACCEL_YOUT_H 0x3D
#define IAM20680HP_ACCEL_YOUT_L 0x3E
#define IAM20680HP_ACCEL_ZOUT_H 0x3F
#define IAM20680HP_ACCEL_ZOUT_L 0x40
#define IAM20680HP_TEMP_OUT_H 0x41
#define IAM20680HP_TEMP_OUT_L 0x42
#define IAM20680HP_GYRO_XOUT_H 0x43
#define IAM20680HP_GYRO_XOUT_L 0x44
#define IAM20680HP_GYRO_YOUT_H 0x45
#define IAM20680HP_GYRO_YOUT_L 0x46
#define IAM20680HP_GYRO_ZOUT_H 0x47
#define IAM20680HP_GYRO_ZOUT_L 0x48
#define IAM20680HP_SIGNAL_PATH_RESET 0x68
#define IAM20680HP_ACCEL_INTEL_CTRL 0x69
#define IAM20680HP_USER_CTRL 0x6A
#define IAM20680HP_PWR_MGMT_1 0x6B
#define IAM20680HP_PWR_MGMT_2 0x6C
#define IAM20680HP_FIFO_COUNTH 0x72
#define IAM20680HP_FIFO_COUNTL 0x73
#define IAM20680HP_FIFO_R_W 0x74
#define IAM20680HP_WHO_AM_I 0x75
#define IAM20680HP_XA_OFFSET_H 0x77
#define IAM20680HP_XA_OFFSET_L 0x78
#define IAM20680HP_YA_OFFSET_H 0x7A
#define IAM20680HP_YA_OFFSET_L 0x7B
#define IAM20680HP_ZA_OFFSET_H 0x7D
#define IAM20680HP_ZA_OFFSET_L 0x7E

typedef enum
{
    /// @brief Return code OK
    IAM20680HP_OK = 0,
    /// @brief Error during I2C communication
    IAM20680HP_ERR_I2C,
    /// @brief Device is not found during I2C communication
    IAM20680HP_ERR_DEVICE_NOT_FOUND,
    /// @brief Device is not ready
    IAM20680HP_ERR_NOT_READY,
    /// @brief Device is busy
    IAM20680HP_ERR_BUSY,
    /// @brief Invalid parameter
    IAM20680HP_ERR_INVALID_PARAM,
    /// @brief Device is not initialized
    IAM20680HP_ERR_NOT_INITIALIZED,
    /// @brief Device is not supported
    IAM20680HP_ERR_NOT_SUPPORTED,
    /// @brief Device is not calibrated
    IAM20680HP_ERR_NOT_CALIBRATED,
    /// @brief Device is not enabled
    IAM20680HP_ERR_NOT_ENABLED,
    /// @brief Device is not IAM20680HP
    IAM20680HP_ERR_DEVICE_ID,
    /// @brief EOL reached
    IAM20680HP_ERR_EOL,

} IAM20680HP_err_t;

typedef struct
{
    uint8_t selfTestXGyro;
    uint8_t selfTestYGyro;
    uint8_t selfTestZGyro;
    uint8_t selfTestXAccel;
    uint8_t selfTestYAccel;
    uint8_t selfTestZAccel;
} IAM20680HP_selfTest_t;

typedef struct
{
    int16_t offsetXGyro;
    int16_t offsetYGyro;
    int16_t offsetZGyro;
} IAM20680HP_gyroOffset_t;

typedef struct
{
    int16_t offsetXAccel;
    int16_t offsetYAccel;
    int16_t offsetZAccel;
} IAM20680HP_accelOffset_t;

typedef struct
{
    bool xGyroSelfTest;
    bool yGyroSelfTest;
    bool zGyroSelfTest;
    uint8_t FS_Sel;
    uint8_t FChoice;
} IAM20680HP_gyroConfig_t;

typedef struct
{
    bool xAccelSelfTest;
    bool yAccelSelfTest;
    bool zAccelSelfTest;
    uint8_t AFS_Sel;
    uint8_t fifoSize;
    uint8_t dec2Cfg;
    bool FChoice;
    uint8_t dlpfCfg;
} IAM20680HP_accelConfig_t;

typedef struct
{
    bool int_level;
    bool int_open;
    bool latch_int_en;
    bool int_rd_clear;
    bool fSync_int_level;
    bool fSync_int_mode_en;
    bool wom_int_en;
    bool fifo_oflow_en;
    bool gdrive_int_en;
    bool data_rdy_en;
} IAM20680HP_intPinConfig_t;

typedef struct
{
    bool wom_int;
    bool fifo_oflow_int;
    bool gdrive_int;
    bool data_rdy_int;
} IAM20680HP_intStatus_t;

typedef struct
{
    int16_t xAccel;
    int16_t yAccel;
    int16_t zAccel;
} IAM20680HP_accelData_t;

typedef struct
{
    int16_t xGyro;
    int16_t yGyro;
    int16_t zGyro;
} IAM20680HP_gyroData_t;

typedef struct
{
    bool fifo_en;
    bool i2c_if_dis;
    bool fifo_rst;
    bool sig_cond_rst;
} IAM20680HP_userControl_t;

typedef struct
{
    bool reset;
    bool sleep;
    bool accelCycle;
    bool gyroStandby;
    bool tempDis;
    uint8_t clockSel;
    bool fifo_lp_en;
    bool stby_xa;
    bool stby_ya;
    bool stby_za;
    bool stby_xg;
    bool stby_yg;
    bool stby_zg;
} IAM20680HP_powerManagement_t;

typedef struct
{
    IAM20680HP_accelData_t accelData;
    int16_t temperature;
    IAM20680HP_gyroData_t gyroData;
} IAM20680HP_fifoData_t;




IAM20680HP_err_t iam20680hpCheckDeviceID(void);
IAM20680HP_err_t iam20680hpResetDevice(void);
IAM20680HP_err_t iam20680hpReadSelfTestRegisters(IAM20680HP_selfTest_t *selfTest);
IAM20680HP_err_t iam20680hpGyroOffsetAdjustment(IAM20680HP_gyroOffset_t *gyroOffset, bool writeOffset);
IAM20680HP_err_t iam20680SampleRateDivider(uint8_t sampleRateDivider, bool writeDivider);
IAM20680HP_err_t iam20680hpConfigFifo(bool *fifoEnabled, bool writeFifo);
IAM20680HP_err_t iam20680hpConfigExtSync(uint8_t *extSyncSet, bool writeExtSyncSet);
IAM20680HP_err_t iam20680hpConfigDlpfCfg(uint8_t *dlpf, bool writeDlpf);
IAM20680HP_err_t iam20680hpGyroConfig(IAM20680HP_gyroConfig_t *gyro, bool writeConfig);
IAM20680HP_err_t iam20680hpAccelConfig(IAM20680HP_accelConfig_t *accel, bool writeConfig);
IAM20680HP_err_t iam20680hpLowPowerMode(bool *enableLPM, uint8_t *avgFilterCfg, uint8_t *womMode, bool writeConfig);
IAM20680HP_err_t iam20680hpWakeOnMotionThreshold(uint8_t *womThreshold, bool writeWomThreshold);
IAM20680HP_err_t iam20680hpFiFoEnable(bool *tempFiFo, bool *gyroX, bool *gyroY, bool *gyroZ, bool *accel, bool writeFiFo);
IAM20680HP_err_t iam20680hpReadFsyncInterruptStatus(bool *fsyncInt);
IAM20680HP_err_t iam20680hpIntConfig(IAM20680HP_intPinConfig_t *intPin, bool writeConfig);
IAM20680HP_err_t iam20680hpIntStatus(IAM20680HP_intStatus_t *intStatus);
IAM20680HP_err_t iam20680hpReadAccelData(IAM20680HP_accelData_t *accelData);
IAM20680HP_err_t iam20680hpReadTemperatureData(int16_t *temperature);
IAM20680HP_err_t iam20680hpReadGyroData(IAM20680HP_gyroData_t *gyroData);
IAM20680HP_err_t iam20680hpSignalPathReset(bool *accel, bool *temp, bool writeReset);
IAM20680HP_err_t iam20680hpIntelControl(bool *enable, bool *mode, bool writeConfig);
IAM20680HP_err_t iam20680hpUserControl(IAM20680HP_userControl_t *userControl, bool writeConfig);
IAM20680HP_err_t iam20680hpPowerManagement(IAM20680HP_powerManagement_t *powerManagement, bool writeConfig);
IAM20680HP_err_t iam20680hpReadFifoCount(uint16_t *fifoCount);
IAM20680HP_err_t iam20680hpReadFifoData(IAM20680HP_fifoData_t *fifoData);
//TODO -  FiFo write functions not implemented yet
IAM20680HP_err_t iam20680hpAccelerometerOffset(IAM20680HP_accelOffset_t *offSet, bool writeConfig);

IAM20680HP_err_t iam20680hpInit(void);
IAM20680HP_err_t iam20680hpEnableWomModeFunction(void);
IAM20680HP_err_t iam20680hpDisableWomModeFunction(void);

#endif // IAM20680HP_H
