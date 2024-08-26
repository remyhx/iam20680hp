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

/*! 
* @brief Enum to hold the general return codes of the IAM-20680HP driver.
*/
typedef enum
{
    IAM20680HP_OK = 0,                      /**< Return code OK */
    IAM20680HP_ERR_I2C,                     /**< Error during I2C communication */
    IAM20680HP_ERR_DEVICE_NOT_FOUND,        /**< Device is not found during I2C communication */
    IAM20680HP_ERR_NOT_READY,               /**< Device is not ready */
    IAM20680HP_ERR_BUSY,                    /**< Device is busy */
    IAM20680HP_ERR_INVALID_PARAM,           /**< Invalid parameter */
    IAM20680HP_ERR_NOT_INITIALIZED,         /**< Device is not initialized */
    IAM20680HP_ERR_NOT_SUPPORTED,           /**< Device is not supported */         
    IAM20680HP_ERR_NOT_CALIBRATED,          /**< Device is not calibrated */
    IAM20680HP_ERR_NOT_ENABLED,             /**< Device is not enabled */
    IAM20680HP_ERR_DEVICE_ID,               /**< Device ID is not correct */
    IAM20680HP_ERR_EOL,                     /**< End of list */

} IAM20680HP_err_t;

/*! 
 * @brief Structure to hold the self-test results of the IAM-20680HP device.
 *
 * This structure contains the self-test results for the gyroscope and accelerometer 
 * axes of the IAM-20680HP device. Each field represents the self-test result for a 
 * specific axis, which can be used to evaluate the performance of the device.
*/
typedef struct
{
    uint8_t selfTestXGyro;      /**< Self-test result for the X-axis gyroscope. */
    uint8_t selfTestYGyro;      /**< Self-test result for the Y-axis gyroscope. */
    uint8_t selfTestZGyro;      /**< Self-test result for the Z-axis gyroscope. */
    uint8_t selfTestXAccel;     /**< Self-test result for the X-axis accelerometer. */
    uint8_t selfTestYAccel;     /**< Self-test result for the Y-axis accelerometer. */
    uint8_t selfTestZAccel;     /**< Self-test result for the Z-axis accelerometer. */
} IAM20680HP_selfTest_t;

/*! 
 * @brief Structure to hold the offset for the gyro.
 *
 * This register is used to remove DC bias from the sensor output. The value in this register is
 * added to the gyroscope sensor value before going into the sensor register. (Copy datasheet)
*/
typedef struct
{
    int16_t offsetXGyro;        /**< Offset value for the X-axis gyroscope. */
    int16_t offsetYGyro;        /**< Offset value for the Y-axis gyroscope. */
    int16_t offsetZGyro;        /**< Offset value for the Z-axis gyroscope. */
} IAM20680HP_gyroOffset_t;

/*! 
 * @brief Structure to hold the offset for the accel.
 *
 * This register is used to offset the accelerometer output. The value in this register is added to the accelerometer 
 * sensor value before going into the sensor register.
*/
typedef struct
{
    int16_t offsetXAccel;       /**< Offset value for the X-axis accelerometer. */
    int16_t offsetYAccel;       /**< Offset value for the Y-axis accelerometer. */
    int16_t offsetZAccel;       /**< Offset value for the Z-axis accelerometer. */
} IAM20680HP_accelOffset_t;

/*! 
 * @brief Structure to hold the configuration for the gyro.
 *
 * This structure contains the configuration settings for the gyroscope of the IAM-20680HP device. 
 * Each field represents a specific configuration setting that can be used to configure the 
 * gyroscope according to the application requirements.
*/
typedef struct
{
    bool xGyroSelfTest;         /**< Flag to enable/disable the self-test for the X-axis gyroscope. */
    bool yGyroSelfTest;         /**< Flag to enable/disable the self-test for the Y-axis gyroscope. */
    bool zGyroSelfTest;         /**< Flag to enable/disable the self-test for the Z-axis gyroscope. */
    uint8_t FS_Sel;             /**< Gyroscope full-scale range selection. */
    uint8_t FChoice;            /**< Gyroscope filter choice. */
} IAM20680HP_gyroConfig_t;

/*! 
 * @brief Structure to hold the configuration for the accelerometer.
 *
 * This structure contains the configuration settings for the accelerometer of the IAM-20680HP device. 
 * Each field represents a specific configuration setting that can be used to configure the 
 * accelerometer according to the application requirements.
*/
typedef struct
{
    bool xAccelSelfTest;        /**< Flag to enable/disable the self-test for the X-axis accelerometer. */
    bool yAccelSelfTest;        /**< Flag to enable/disable the self-test for the Y-axis accelerometer. */
    bool zAccelSelfTest;        /**< Flag to enable/disable the self-test for the Z-axis accelerometer. */
    uint8_t AFS_Sel;            /**< Accelerometer full-scale range selection. */
    uint8_t fifoSize;           /**< FIFO size. */
    uint8_t dec2Cfg;            /**< Dec2 configuration: Averaging filter settings for Low Power Accelerometer mode */
    bool FChoice;               /**< Filter choice: used to bypass DLPF */
    uint8_t dlpfCfg;            /**< DLPF: Digital low-pass filter configuration. */
} IAM20680HP_accelConfig_t;

/*! 
 * @brief Structure to hold the configuration for the interrupt pin.
 *
 * This structure contains the configuration settings for the interrupt pin of the IAM-20680HP device. 
 * Each field represents a specific configuration setting that can be used to configure the 
 * accelerometer according to the application requirements.
*/
typedef struct
{
    bool int_level;             /**< Interrupt active high (0) or active low (1). */
    bool int_open;              /**< Interrupt push-pull (0) or open-drain (1). */
    bool latch_int_en;          /**< 1 - Latch interrupt request until interrupt status is cleared. 0 - interrupt pulse’s width is 50 µs.*/
    bool int_rd_clear;          /**< 1 - Interrupt status is cleared by any read operation. 0 - cleared by reading int_status (iam20680hpIntStatus())*/
    bool fSync_int_level;       /**< 1 - FSYNC interrupt active low. 0 - FSYNC interrupt active high. */
    bool fSync_int_mode_en;     /**< 1 - FSYNC interrupt mode enabled. 0 - FSYNC interrupt mode disabled. */
    bool wom_int_en;            /**< 1 - Wake on motion interrupt enabled. 0 - Wake on motion interrupt disabled. */
    bool fifo_oflow_en;         /**< 1 - FIFO overflow interrupt enabled. 0 - FIFO overflow interrupt disabled. */
    bool gdrive_int_en;         /**< 1 - G-Drive interrupt enabled. 0 - G-Drive interrupt disabled. */
    bool data_rdy_en;           /**< 1 - Data ready interrupt enabled. 0 - Data ready interrupt disabled. */
} IAM20680HP_intPinConfig_t;

/*! 
    * @brief Structure to hold the status of the interrupt pin.
    *
    * This structure contains the status of the interrupt pin of the IAM-20680HP device. Clears to 0 after readout.
    * Each field represents a specific status of the interrupt pin that can be used to check the 
    * status of the interrupt pin.
*/
typedef struct
{
    bool wom_int;               /**< Wake on motion interrupt. */
    bool fifo_oflow_int;        /**< FIFO overflow interrupt. */
    bool gdrive_int;            /**< Gyroscope Drive System interrupt. */
    bool data_rdy_int;          /**< Data ready interrupt. */
} IAM20680HP_intStatus_t;

/*! 
    * @brief Structure to hold the raw accelerometer data.
    *
    * This structure contains the raw data from the accelerometer of the IAM-20680HP device. 
    * Each field represents the raw data for a specific axis of the accelerometer.
*/
typedef struct
{
    int16_t xAccel;             /**< Raw data for the X-axis accelerometer. */
    int16_t yAccel;             /**< Raw data for the Y-axis accelerometer. */
    int16_t zAccel;             /**< Raw data for the Z-axis accelerometer. */
} IAM20680HP_accelData_t;

/*! 
    * @brief Structure to hold the raw gyro data.
    *
    * This structure contains the raw data from the gyroscope of the IAM-20680HP device. 
    * Each field represents the raw data for a specific axis of the gyroscope.
*/
typedef struct
{
    int16_t xGyro;              /**< Raw data for the X-axis gyroscope. */
    int16_t yGyro;              /**< Raw data for the Y-axis gyroscope. */
    int16_t zGyro;              /**< Raw data for the Z-axis gyroscope. */
} IAM20680HP_gyroData_t;

/*! 
    * @brief Structure to hold the user control register.
    *
    * This structure contains the user control register of the IAM-20680HP device. 
    * Each field represents a specific configuration setting that can be used to configure the 
    * user control register according to the application requirements.
*/
typedef struct
{
    bool fifo_en;               /**< FIFO enable. */
    bool i2c_if_dis;            /**< I2C interface disable. */
    bool fifo_rst;              /**< FIFO reset. */
    bool sig_cond_rst;          /**< Signal path reset. (Gyro/Temp/Accel, also reset sensor registers)*/
} IAM20680HP_userControl_t;

/*! 
    * @brief Structure to hold the power management register.
    *
    * This structure contains the power management register of the IAM-20680HP device. 
    * Each field represents a specific configuration setting that can be used to configure the 
    * power management register according to the application requirements.
*/
typedef struct
{
    bool reset;                 /**< Device reset. */
    bool sleep;                 /**< Sleep mode. */
    bool accelCycle;            /**< Accelerometer cycle mode between sleep and taking samples in between. */
    bool gyroStandby;           /**< Gyro drive and pll circuitry are enabled, but the sense paths are disabled. This is a low power mode that allows quick enabling of the gyros. */
    bool tempDis;               /**< Temperature sensor disable. */
    uint8_t clockSel;           /**< Clock source select. */
    bool fifo_lp_en;            /**< FIFO low power enable. */
    bool stby_xa;               /**< X-axis accelerometer disabler. */
    bool stby_ya;               /**< Y-axis accelerometer disabler. */
    bool stby_za;               /**< Z-axis accelerometer disabler. */
    bool stby_xg;               /**< X-axis gyroscope disabler. */
    bool stby_yg;               /**< Y-axis gyroscope disabler. */
    bool stby_zg;               /**< Z-axis gyroscope disabler. */
} IAM20680HP_powerManagement_t;

/*! 
    * @brief Structure to hold the FIFO data.
    *
    * This structure contains the FIFO data of the IAM-20680HP device. 
    * Each field represents the FIFO data for the accelerometer, temperature, and gyroscope.
*/
typedef struct
{
    IAM20680HP_accelData_t accelData;       /**< FIFO data for the accelerometer. */
    int16_t temperature;                    /**< FIFO data for the temperature. */
    IAM20680HP_gyroData_t gyroData;         /**< FIFO data for the gyroscope. */
} IAM20680HP_fifoData_t;



/*! @brief Check if the device is connected and is the correct device
 *
 *  This function checks if the device is connected by reading the WHO_AM_I register and comparing it to the expected value
 *
 *  @retval IAM20680HP_OK if device is connected
 *  @retval IAM20680HP_ERR_DEVICE_ID if device is not recognized/detected (wrong return value)
 *  @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpCheckDeviceID(void);

/*! @brief Reset the device
 *
 *  This function resets the device by writing to the PWR_MGMT_1 register
 *
 *  @retval IAM20680HP_OK if device is reset
 *  @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpResetDevice(void);

/*! @brief Read the self test registers of the device (Gyro and Accelerometer)
 *
 *  This function reads the self test registers of the device. 
 *
 *  @param selfTest Pointer to the IAM20680HP_selfTest_t struct where the self test values will be stored
 *  @retval IAM20680HP_OK if self test registers are read
 *  @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadSelfTestRegisters(IAM20680HP_selfTest_t *selfTest);

/*! @brief Adjust the gyro offset
 *
 * This function adjusts the gyro offset by writing the offset values to the device or reading the offset values from the device
 *
 * @param gyroOffset Pointer to the struct IAM20680HP_gyroOffset_t where the offset values will be stored
 * @param writeOffset If true, the offset values will be written to the device, if false, the offset values will be read from the device
 * 
 * @retval IAM20680HP_OK if the offset values are adjusted/read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpGyroOffsetAdjustment(IAM20680HP_gyroOffset_t *gyroOffset, bool writeOffset);

/*! @brief Set the sample rate divider
 *
 * Divides the internal sample rate (see register CONFIG) to generate the sample rate that
 * controls sensor data output rate, FIFO sample rate.
 * This is the update rate of the sensor register:
 * SAMPLE_RATE = INTERNAL_SAMPLE_RATE / (1 + SMPLRT_DIV)
 * Where INTERNAL_SAMPLE_RATE = 1 kHz (Copy from datasheet)
 * 
 * @warning <b> Note: This register is only effective when FCHOICE_B register bits are 2’b00, and (0 < DLPF_CFG < 7). </b>
 *
 * @param sampleRateDivider Pointer to the sample rate divider value (uint8_t). Sample_rate = Internal_Sample_Rate / (1 + sampleRateDivider), internal sample rate is 1kHz
 * @param writeDivider If true, the sample rate divider value will be written to the device, if false, the sample rate divider value will be read from the device
 * @retval IAM20680HP_OK if the sample rate divider value is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680SampleRateDivider(uint8_t *sampleRateDivider, bool writeDivider);

/*! @brief Check or write the FiFo mode
 *
 * When FiFo is false, oldest data will be overwritten. When FiFo is true, the FiFo 
 * will stop when full.
 *
 * @param fifoEnabled Pointer to the boolean value that will be set to true if FiFo is not overwritten, false if FiFo is overwritten
 * @param writeFifo If true, the FiFo status will be written to the device, if false, the FiFo status will be read from the device
 * @retval IAM20680HP_OK if the FiFo status is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpConfigFifo(bool *fifoEnabled, bool writeFifo);

/*! @brief Reads or writes the external sync set, enable or disable the external sync
 *
 * @param extSyncSet setting of the external sync. 0 function disabled, 1 TEMP_OUT_L[0], 2 GYRO_XOUT_L[0], 3 GYRO_YOUT_L[0],
 * 4 GYRO_ZOUT_L[0], 5 ACCEL_XOUT_L[0], 6 ACCEL_YOUT_L[0], 7 ACCEL_ZOUT_L[0]
 * @param writeExtSyncSet write or read the external sync setting
 * @retval IAM20680HP_OK if the external sync setting is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpConfigExtSync(uint8_t *extSyncSet, bool writeExtSyncSet);

/*! @brief Reads or writes the DLPF configuration
 *
 * @param dlpf setting of the DLPF. 3dB bandwidth: 0: 250Hz, 1: 176Hz, 2: 92Hz, 3: 41Hz, 4: 20Hz, 5: 10Hz, 6: 6Hz, 7: 3281Hz
 * @param writeDlpf write or read the DLPF setting
 * @note For the DLPF to be used, FCHOICE_B[1:0] is 2’b00 (table 17 datasheet)
 * @retval IAM20680HP_OK if the DLPF setting is read/set
 * @retval IAM20680HP_ERR_INVALID_PARAM if the parameter is invalid
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpConfigDlpfCfg(uint8_t *dlpf, bool writeDlpf);

/*! @brief Reads or writes the gyro configuration, see page 35 of datasheet for more information
 *
 * Enables the self-test functionality for the gyro, sets the full-scale range of the gyro, and sets the filter choice of the gyro
 * Gyro full scale select:
 * 
 * - 00 = +/- 250dps
 * 
 * - 01 = +/- 500dps
 * 
 * - 10 = +/- 1000dps
 * 
 * - 11 = +/- 2000dps.
 * 
 * FChoice_B is to bypass the DLPF filter (Table 17 datasheet):
 * - FChoice = 1, DLPF Off, 3dB bandwidth = 8173hz
 * 
 * - Fchoice = 2, DLPF Off, 3dB bandwidth = 3281hz
 * 
 * - needs to be 0 for DLPF to be used
 *
 * @param gyro Pointer to the struct IAM20680HP_gyroConfig_t where the gyro configuration values will be stored
 * @param writeConfig If true, the gyro configuration values will be written to the device, if false, the gyro configuration values will be read from the device
 * @retval IAM20680HP_OK if the gyro configuration is read/set
 * @retval IAM20680HP_ERR_INVALID_PARAM if the parameter is invalid
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpGyroConfig(IAM20680HP_gyroConfig_t *gyro, bool writeConfig);

/*! @brief Reads or writes the accelerometer configuration, see page 36 of datasheet for more information
 *
 *  Enables the self-test functionality for the accelerometer, sets the full-scale range of the accelerometer, sets the FIFO size, 
 *  sets the Low Power averaging filter settings, sets the filter choice of the accelerometer, and sets the DLPF configuration 
 *  of the accelerometer
 * 
 *  Accelerometer full scale select:
 * 
 *  - 00 = +/- 2g
 * 
 *  - 01 = +/- 4g
 * 
 *  - 10 = +/- 8g
 * 
 *  - 11 = +/- 16g. 
 * 
 *  FIFO size:
 * 
 *  - 0 = 512byte
 * 
 *  - 1 = 1kByte
 * 
 *  - 2 = 2kByte
 * 
 *  - 3 = 4kByte. 
 * 
 *  Dec2Cfg: Averaging filter settings for Low Power Accelerometer mode:
 * 
 *  - 0: 4 samples
 * 
 *  - 1: 8 samples
 * 
 *  - 2: 16 samples
 * 
 *  - 3: 32 samples.
 * 
 *  FChoice_B is to bypass the DLPF filter (Table 17 datasheet): 1: bypass DLPF (3dB 1046 hz) , 0: use DLPF. 
 * 
 *  DLPF: Digital low-pass filter configuration:
 * - 0: 218.1Hz
 * 
 * - 1: 218.1Hz
 * 
 * - 2: 99.0Hz
 * 
 * - 3: 44.8Hz
 * 
 * - 4: 21.2Hz
 * 
 * - 5: 10.2Hz
 * 
 * - 6: 5.1Hz
 * 
 * - 7: 420.0Hz.
 * 
 * @param accel Pointer to the struct IAM20680HP_accelConfig_t where the accelerometer configuration values will be stored
 * @param writeConfig If true, the accelerometer configuration values will be written to the device, if false, the accelerometer configuration values will be read from the device
 * @retval IAM20680HP_OK if the accelerometer configuration is read/set
 * @retval IAM20680HP_ERR_INVALID_PARAM if the parameter is invalid
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 * 
 * @note The data output rate of the DLPF filter block can be further reduced by a factor of 1/(1+SMPLRT_DIV), where SMPLRT_DIV is an 8-bit
integer. (See iam20680SampleRateDivider()) Following is a small subset of ODRs that are configurable for the accelerometer in the low-noise mode in this manner (Hz):
3.91, 7.81, 15.63, 31.25, 62.50, 125, 250, 500, 1K.
 */
IAM20680HP_err_t iam20680hpAccelConfig(IAM20680HP_accelConfig_t *accel, bool writeConfig);

/*! @brief Reads or writes the low power mode configuration, see page 37 of datasheet for more information
 *
 * EnableLPM is to switch the gyro in low power mode (1 = enabled, 0 = disabled). AvgFilterCfg is the averaging filter configuration for the low
 * power gyro mode. (Default 0). 1: 2x, 2: 4x, 3: 8x, 4: 16x, 5: 32x, 6: 64x, 7: 128x. WOM Output Data Rate Mode: 0-3: reserved. 
 * 4: 3.9 Hz, 5: 7.8 Hz, 6: 15.6 Hz, 7: 31.3 Hz, 8: 62.5 Hz, 9: 125 Hz, 10: 250 Hz, 11: 500 Hz, 12: 1 kHz, 13-15: reserved.
 *
 * @param enableLPM Pointer to the boolean value that will be set to true if low power mode is enabled, false if low power mode is disabled
 * @param avgFilterCfg Pointer to the value that will be set to the average filter configuration
 * @param womMode Pointer to the value that will be set to the wake on motion mode
 * @param writeConfig If true, the low power mode configuration values will be written to the device, if false, the low power mode configuration values will be read from the device
 * @retval IAM20680HP_OK if the low power mode configuration is read/set
 * @retval IAM20680HP_ERR_INVALID_PARAM if the parameter is invalid
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 * 
 * @note To operate in accelerometer low-power mode, ACCEL_CYCLE should be set to ‘1’ in Powermanagement.
 */
IAM20680HP_err_t iam20680hpLowPowerMode(bool *enableLPM, uint8_t *avgFilterCfg, uint8_t *womMode, bool writeConfig);

/*! @brief Reads or writes the wake on motion configuration, see page 38 of datasheet for more information. Threshold resolution is 4mg/LSB regardless the selected full scale
 *
 * This register holds the threshold value for the Wake on Motion Interrupt for accelerometer.
 * Wake on motion threshold resolution is 4 mg/LSB regardless the selected full scale. (copy datasheet)
 *
 * @param womThreshold Pointer to the value that will be set to the wake on motion threshold
 * @param writeWomThreshold If true, the wake on motion threshold value will be written to the device, if false, the wake on motion threshold value will be read from the device
 * @retval IAM20680HP_OK if the wake on motion threshold is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpWakeOnMotionThreshold(uint8_t *womThreshold, bool writeWomThreshold);

/*! @brief Reads or writes the FiFo enable registry. See page 39 of datasheet for more information
 *
 * @param tempFiFo Pointer to the boolean value that will be set to true if temperature data is stored in FiFo (even if datapath is not enabled), false if temperature data is not stored in FiFo
 * @param gyroX Pointer to the boolean value that will be set to true if X-axis gyro data is stored in FiFo (even if datapath is not enabled), false if X-axis gyro data is not stored in FiFo
 * @param gyroY Pointer to the boolean value that will be set to true if Y-axis gyro data is stored in FiFo (even if datapath is not enabled), false if Y-axis gyro data is not stored in FiFo
 * @param gyroZ Pointer to the boolean value that will be set to true if Z-axis gyro data is stored in FiFo (even if datapath is not enabled), false if Z-axis gyro data is not stored in FiFo
 * @param accel Pointer to the boolean value that will be set to true if accelerometer data is stored in FiFo (even if datapath is not enabled), false if accelerometer data is not stored in FiFo
 * @param writeFiFo If true, the FiFo enable registry values will be written to the device, if false, the FiFo enable registry values will be read from the device
 * @retval IAM20680HP_OK if the FiFo enable registry values are read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpFiFoEnable(bool *tempFiFo, bool *gyroX, bool *gyroY, bool *gyroZ, bool *accel, bool writeFiFo);

/*! @brief Reads or writes the fsync interrupt status. Readout clears the bit. See page 39 of datasheet for more information
 *
 *   @param fsyncInt Pointer to the boolean value that will be set to true if fsync interrupt is active, false if fsync interrupt is not active
 *   @retval IAM20680HP_OK if the fsync interrupt status is read
 *   @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadFsyncInterruptStatus(bool *fsyncInt);

/*! @brief Reads or writes the interrupt config/enable. See page 39 and 40 of datasheet for more information
 *
 * - int_level;          Interrupt active high (0) or active low (1).
 * 
 * - int_open;           Interrupt push-pull (0) or open-drain (1).
 * 
 * - latch_int_en;       1 - Latch interrupt request until interrupt status is cleared. 0 - interrupt pulse’s width is 50 µs.
 * 
 * - int_rd_clear;       1 - Interrupt status is cleared by any read operation. 0 - cleared by reading int_status (iam20680hpIntStatus())
 * 
 * - fSync_int_level;    1 - FSYNC interrupt active low. 0 - FSYNC interrupt active high.
 * 
 * - fSync_int_mode_en;  1 - FSYNC interrupt mode enabled. 0 - FSYNC interrupt mode disabled.
 *  
 * - wom_int_en;         1 - Wake on motion interrupt enabled. 0 - Wake on motion interrupt disabled.
 * 
 * - fifo_oflow_en;      1 - FIFO overflow interrupt enabled. 0 - FIFO overflow interrupt disabled.
 * 
 * - gdrive_int_en;      1 - G-Drive interrupt enabled. 0 - G-Drive interrupt disabled.
 * 
 * - data_rdy_en;        1 - Data ready interrupt enabled. 0 - Data ready interrupt disabled.

 *  @param intPin Pointer to the struct IAM20680HP_intPinConfig_t where the interrupt pin configuration values will be stored
 *  @param writeConfig If true, the interrupt pin configuration values will be written to the device, if false, the interrupt pin configuration values will be read from the device
 *  @retval IAM20680HP_OK if the interrupt pin configuration is read/set
 *  @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpIntConfig(IAM20680HP_intPinConfig_t *intPin, bool writeConfig);

/*! @brief Reads the interrupt status. See page 40 of datasheet for more information. All the bits are cleared after readout.
 *
 * - wom_int;        Wake on motion interrupt.
 * 
 * - fifo_oflow_int; FIFO overflow interrupt.
 * 
 * - gdrive_int;     Gyroscope Drive System interrupt.
 * 
 * - data_rdy_int;   Data ready interrupt.
 *
 * @param intStatus Pointer to the struct IAM20680HP_intStatus_t where the interrupt status values will be stored
 * @retval IAM20680HP_OK if the interrupt status is read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpIntStatus(IAM20680HP_intStatus_t *intStatus);

/*! @brief Reads the raw measurements of the accelerometer. See page 40 of datasheet for more information
 *
 * @param accelData Pointer to the struct IAM20680HP_accelData_t where the accelerometer data will be stored
 * @retval IAM20680HP_OK if the accelerometer data is read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadAccelData(IAM20680HP_accelData_t *accelData);

/*! @brief Reads the temperature data. See page 41 of datasheet for more information
 *
 * @param temperature Pointer to the value where the temperature data will be stored, in celcius * 100
 * @retval IAM20680HP_OK if the temperature data is read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadTemperatureData(int16_t *temperature);

/*! @brief Reads the raw measurements of the gyroscope. See page 42 of datasheet for more information
 *
 * @param accelData Pointer to the struct IAM20680HP_gyroData_t where the gyroscope data will be stored
 * @return IAM20680HP_OK if the gyroscope data is read, IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadGyroData(IAM20680HP_gyroData_t *gyroData);

/*! @brief Reads or writes the signal path reset. See page 42 of datasheet for more information
 *
 * @param accel Pointer to the boolean value that will be set to true if the accelerometer signal path is reset, false if the accelerometer signal path is not reset
 * @param temp Pointer to the boolean value that will be set to true if the temperature signal path is reset, false if the temperature signal path is not reset
 * @param writeReset If true, the signal path reset values will be written to the device, if false, the signal path reset values will be read from the device
 * @retval IAM20680HP_OK if the signal path reset values are read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 * @note Sensor registers are not cleared. Use iam20680hpUserControl() to clear sensor registers.
 */
IAM20680HP_err_t iam20680hpSignalPathReset(bool *accel, bool *temp, bool writeReset);

/*! @brief Reads or writes the accelerometer intelligence control. See page 42 of datasheet for more information
 *
 * Intelligence mode:
 * 
 * - 0 – Compares the current sample to the first sample taken when entering in
low-power mode.
 *
 * - 1 – Compare the current sample with the previous sample.
 * 
 * @param enable Pointer to the boolean value that will be set to true if the accelerometer intelligence control is enabled, false if the accelerometer intelligence control is disabled
 * @param mode Pointer to the boolean value that will be set to true if the accelerometer intelligence control is in mode 1, false if the accelerometer intelligence control is in mode 0
 * @param writeConfig If true, the accelerometer intelligence control values will be written to the device, if false, the accelerometer intelligence control values will be read from the device
 * @retval IAM20680HP_OK if the accelerometer intelligence control is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 * @note The accelerometer intelligence control is only available in low-power mode.
 */
IAM20680HP_err_t iam20680hpIntelControl(bool *enable, bool *mode, bool writeConfig);

/*! @brief Reads or writes the user control. See page 43 of datasheet for more information
 *
 * - fifo_en;       FIFO enable. (0 - Disable FIFO access from serial interface. To disable FIFO writes by DMA, use FIFO_EN
register.)
 * 
 * - i2c_if_dis;    I2C interface disable/SPI only.
 * 
 * - fifo_rst;      FIFO reset. Reset is asynchronous. This bit auto clears after one clock cycle of the
internal 20 MHz clock.
 * 
 * - sig_cond_rst;  Signal path reset. (Gyro/Temp/Accel, also reset sensor registers)
 *
 * @param userControl Pointer to the struct IAM20680HP_userControl_t where the user control values will be stored
 * @param writeConfig If true, the user control values will be written to the device, if false, the user control values will be read from the device
 * @retval IAM20680HP_OK if the user control is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpUserControl(IAM20680HP_userControl_t *userControl, bool writeConfig);

/*! @brief Reads or writes the power management. See page 43/44 of datasheet for more information
 *
 * - reset;        Device reset: Reset the internal registers and restores the default settings.
 * 
 * - sleep;        Sleep mode.
 * 
 * - accelCycle;   Accelerometer cycle mode between sleep and taking samples in between when set to 1. Only takes effect when the device is not in sleep or standby mode.
 * 
 * - gyroStandby;  Gyro drive and pll circuitry are enabled, but the sense paths are disabled. This is a low power mode that allows quick enabling of the gyros.
 * 
 * - tempDis;      Temperature sensor disable when set to 1.
 * 
 * - clockSel;     Clock source select. 0 & 6: Internal 20MHz oscillator, 1-5: Auto selects the best available clock source – PLL if ready, else use the Internal oscillator, 7: Stops the clock and keeps the timing generator in reset.
 * 
 * - fifo_lp_en;   FIFO low power enable.
 * 
 * - stby_xa;      X-axis accelerometer disabler.
 * 
 * - stby_ya;      Y-axis accelerometer disabler.
 * 
 * - stby_za;      Z-axis accelerometer disabler.
 * 
 * - stby_xg;      X-axis gyroscope disabler.
 * 
 * - stby_yg;      Y-axis gyroscope disabler.
 * 
 * - stby_zg;      Z-axis gyroscope disabler.
 * 
 * @param powerManagement Pointer to the struct IAM20680HP_powerManagement_t where the power management values will be stored
 * @param writeConfig If true, the power management values will be written to the device, if false, the power management values will be read from the device
 * @retval IAM20680HP_OK if the power management is read/set
 * @retval IAM20680HP_ERR_INVALID_PARAM if the parameter is invalid
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 * @note When all accelerometer axes are disabled via PWR_MGMT_2 register bits and cycle is enabled, the chip will wake up at the rate determined by the respective registers above, but will not take any samples.
 */
IAM20680HP_err_t iam20680hpPowerManagement(IAM20680HP_powerManagement_t *powerManagement, bool writeConfig);

/*! @brief Reads the FiFo count register. See page 44 of datasheet for more information
 *
 * @param fifoCount Pointer to the value (uint16_t) where the FiFo count will be stored
 * @retval IAM20680HP_OK if the FiFo count is read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadFifoCount(uint16_t *fifoCount);

/*! @brief Reads the FiFo data. See page 44 of datasheet for more information
 *
 * If the FIFO buffer is empty, reading register FIFO_DATA will return a unique value of 0xFF until new data are available. Normal data 
 * are precluded from ever indicating 0xFF, so 0xFF gives a trustworthy indication of FIFO empty. 0x00 indicates FiFo off.
 *
 * @param fifoData Pointer to the struct IAM20680HP_fifoData_t where the FiFo data will be stored, writing is not yet supported by code
 * @retval IAM20680HP_OK if the FiFo data is read
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpReadFifoData(IAM20680HP_fifoData_t *fifoData);

//TODO -  FiFo write functions not implemented yet

/*! @brief Reads or writes the Accelerometer Offset data. See page 45 of datasheet for more information
 *
 * ±16g Offset cancellation in all Full-Scale modes, 15 bit 0.98-mg steps. The offset cancellation is performed by adding 
 * the offset value from the sensor data.
 * 
 * @param offSet Pointer to the struct IAM20680HP_accelOffset_t where the accelerometer offset values will be stored
 * @param writeConfig If true, the accelerometer offset values will be written to the device, if false, the accelerometer offset values will be read from the device
 * @retval IAM20680HP_OK if the accelerometer offset is read/set
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpAccelerometerOffset(IAM20680HP_accelOffset_t *offSet, bool writeConfig);

/*! @brief Initialises the device with the standard settings
 *
 * @note Sometimes it is wise to disable (HAL_NVIC_DisableIRQ) the IRQ function and clear pending IRQs (NVIC_ClearPendingIRQ) before 
 * calling this function, because after resetting the device the IRQ pin could be triggered.
 *
 * @retval IAM20680HP_OK if the device is initialised
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpInit(void);

/*! @brief Enables the wake on motion function
 *
 * @retval IAM20680HP_OK if the wake on motion function is enabled
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpEnableWomModeFunction(void);

/*! @brief Disables the wake on motion function
 *
 * @retval IAM20680HP_OK if the wake on motion function is disabled
 * @retval IAM20680HP_ERR_I2C if there is an error during I2C communication
 */
IAM20680HP_err_t iam20680hpDisableWomModeFunction(void);

#endif // IAM20680HP_H
