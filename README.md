# iam20680hp for STM32 (HAL I2C)

- Add the library to your project.
- Change the I2C_HANDLER if necessary. 
- Check the correct I2C address IAM20680_LOGIC. 
- You can start with `iam20680hpInit()`.
- Readout by `iam20680hpReadAccelData()` and `iam20680hpReadGyroData()`.

For example:

```c
//Init the iam20680
if (iam20680hpInit() != IAM20680HP_OK){
  errorHandler();
}

IAM20680HP_accelData_t accelData;
IAM20680HP_gyroData_t gyroData;
IAM20680HP_err_t result;

while(1) {

  result = iam20680hpReadAccelData(&accelData);
  if (result != IAM20680HP_OK)
    errorHandler();
  result = iam20680hpReadGyroData(&gyroData);
  if (result != IAM20680HP_OK)
    errorHandler();

  HAL_Delay(50);
  
}

```
---


## Settings for initialisation

The basic settings for `iam20680hpInit()` are defined in the header file:

```c
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
```
---

## Tables as used in the datasheet

Table 17

| FCHOICE_B |      | DLPF_CFG | Gyroscope     |                       |                  | Temperature Sensor  |
|-----------|------|----------|---------------|-----------------------|------------------|---------------------|
| <1>       | <0>  |          | 3-dB BW (Hz)  | Noise BW (Hz)         | Rate (kHz)       | 3-dB BW (Hz)         |
|-----------|------|----------|---------------|-----------------------|------------------|---------------------|
| X         | 1    | X        | 8173          | 8595.1                | 32               | 4000                |
| 1         | 0    | X        | 3281          | 3451.0                | 32               | 4000                |
| 0         | 0    | 0        | 250           | 306.6                 | 8                | 4000                |
| 0         | 0    | 1        | 176           | 177.0                 | 1                | 188                 |
| 0         | 0    | 2        | 92            | 108.6                 | 1                | 98                  |
| 0         | 0    | 3        | 41            | 59.0                  | 1                | 42                  |
| 0         | 0    | 4        | 20            | 30.5                  | 1                | 20                  |
| 0         | 0    | 5        | 10            | 15.6                  | 1                | 10                  |
| 0         | 0    | 6        | 5             | 8.0                   | 1                | 5                   |
| 0         | 0    | 7        | 3281          | 3451.0                | 8                | 4000                |


Table 18

| ACCEL_FCHOICE_B | A_DLPF_CFG | 3-dB BW (Hz) | Noise BW (Hz) | Rate (kHz) |
|-----------------|------------|--------------|---------------|------------|
| 1               | X          | 1046.0       | 1100.0        | 4          |
| 0               | 0          | 218.1        | 235.0         | 1          |
| 0               | 1          | 218.1        | 235.0         | 1          |
| 0               | 2          | 99.0         | 121.3         | 1          |
| 0               | 3          | 44.8         | 61.5          | 1          |
| 0               | 4          | 21.2         | 31.0          | 1          |
| 0               | 5          | 10.2         | 15.5          | 1          |
| 0               | 6          | 5.1          | 7.8           | 1          |
| 0               | 7          | 420.0        | 441.6         | 1          |

