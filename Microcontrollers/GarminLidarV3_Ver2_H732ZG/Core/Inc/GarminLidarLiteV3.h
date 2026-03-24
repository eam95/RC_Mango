/*
 * GarminLidarLiteV3.h
 *
 *  Created on: Nov 9, 2025
 *      Author: erick
 */

#ifndef INC_GARMINLIDARLITEV3_H_
#define INC_GARMINLIDARLITEV3_H_



#include "main.h"


////////////////////////////////////////////////////////////////////////////////////////////////////

// I2C address for Garmin Lidar Lite V3
#define LIDARLITEV3_I2C_ADDRESS       (0x62 << 1) // 7-bit address shifted for HAL

// Register addresses
#define ACQ_COMMAND       0x00
#define STATUS            0x01
#define SIG_COUNT_VAL     0x02
#define ACQ_CONFIG_REG    0x04
#define VELOCITY          0x09
#define PEAK_CORR         0x0C
#define NOISE_PEAK        0x0D
#define SIGNAL_STRENGTH   0x0E
#define FULL_DELAY_HIGH   0x0F
#define FULL_DELAY_LOW    0x10
#define OUTER_LOOP_COUNT  0x11
#define REF_COUNT_VAL     0x12
#define LAST_DELAY_HIGH   0x14
#define LAST_DELAY_LOW    0x15
#define UNIT_ID_HIGH      0x16
#define UNIT_ID_LOW       0x17
#define I2C_ID_HIGH       0x18
#define I2C_ID_LOW        0x19
#define I2C_SEC_ADDR      0x1A
#define THRESHOLD_BYPASS  0x1C
#define I2C_CONFIG        0x1E
#define COMMAND           0x40
#define MEASURE_DELAY     0x45
#define PEAK_BCK          0x4C
#define CORR_DATA         0x52
#define CORR_DATA_SIGN    0x53
#define ACQ_SETTINGS      0x5D
#define POWER_CONTROL     0x65

////////////////////////////////////////////////////////////////////////////////////////////////////
// Register descriptions and settings



//////////////////////////////////////////////////////////////
/* Device Command (AQR_COMMAND) bits 0x00
 * Description of each bit in the AQR_COMMAND register
 *
 *
 *
 * */
// bit 3-0: Command to the device
#define TAKE_DISTANCE_MEASUREMENT_WITH_BIAS_CORRECTION    0x04
#define TAKE_DISTANCE_MEASUREMENT_NO_BIAS_CORRECTION      0x03
#define RESET_DEVICE                                      0x00



///////////////////////////////////////////////////////////////
/* System Status (STAUS) bits 0x01
 * Description of each bit in the STATUS register
 *
 *
 *
 * */
// bit 6: Process error flag
#define PROCESS_ERROR_FLAG    0x40

// bit 5: Health Flag
#define HEALTH_FLAG           0x20

// bit 4: Secondary return flag
#define SECONDARY_RETURN_FLAG 0x10

// bit 3: Invalid signal flag
#define INVALID_SIGNAL_FLAG   0x08

// bit 2: Signal overflow flag
#define SIGNAL_OVERFLOW_FLAG  0x04

// bit 1: Reference overflow flag
#define REFERENCE_OVERFLOW_FLAG 0x02

// bit 0: Busy flag
#define BUSY_FLAG             0x01

///////////////////////////////////////////////////////////////
/* Acquisition count (SIG_COUNT_VAL) bits 0x02
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Acquisition Mode Control (ACQ_CONFIG_REG) bits 0x04
 *
 *
 *
 * */
// Bit 6 : Enable Reference Process during measurement
#define ENABLE_REFERENCE_PROCESS    0x00
#define DISABLE_REFERENCE_PROCESS   0x40

// Bit 5 : Measure Delay
#define ENABLE_MEASURE_DELAY    0x20
#define DISABLE_MEASURE_DELAY   0x00

// Bit 4 : Reference Filter
#define ENABLE_REFERENCE_FILTER    0x10
#define DISABLE_REFERENCE_FILTER   0x00

// Bit 3 : Enable Quick Termination
#define ENABLE_QUICK_TERMINATION    0x08
#define DISABLE_QUICK_TERMINATION   0x00

// Bit 2 : Use Reference Acquisition Count
#define ENABLE_REFERENCE_ACQ_COUNT    0x04
#define DISABLE_REFERENCE_ACQ_COUNT   0x00

// Bit 1-0 : Select Pin Function Control
#define DEFAULT_PWM_Mode             0x00
#define STATUS_OUTPUT_MODE           0x01
#define FIXED_DELAY_PWM_MODE         0x02
#define OSCILLATOR_OUTPUT_MODE       0x03

///////////////////////////////////////////////////////////////
/* Velocity output (VELOCITY) bits 0x09
 *
 *
 *
 * */

///////////////////////////////////////////////////////////////
/* Peak value Correlation (PEAK_CORR) bits 0x0C
 *
 *
 *
 * */

///////////////////////////////////////////////////////////////
/* Noise Peak (NOISE_PEAK) bits 0x0D
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Signal Strength (SIGNAL_STRENGTH) bits 0x0E
 *
 *
 *
 * */

///////////////////////////////////////////////////////////////
/* Distance Measurement High Byte (FULL_DELAY_HIGH) bits 0x0F
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Distance Measurement Low Byte (FULL_DELAY_LOW) bits 0x010
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Burst Measurement Count Control (OUTER_LOOP_COUNT) bits 0x11
 * Initial Value: 0x01
 *
 *
 * */
#define SINGLE_MEASUREMENT    0x01 // Single Measurement
#define CONT_MEASUREMENT     0xFF // Indefinite Repetition until stopped


///////////////////////////////////////////////////////////////
/* Reference Acquisition Count (REF_COUNT_VAL) bits 0x12
 * Initial Value: 0x05
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Previous Distance Measurement High Byte (LAST_DELAY_HIGH) bits 0x14
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Previous Distance Measurement Low Byte (LAST_DELAY_LOW) bits 0x15
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Serial Number High Byte (UNIT_ID_HIGH) bits 0x16
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Serial Number Low Byte (UNIT_ID_LOW) bits 0x17
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Write Serial # High Byte For I2C Addr Unlock  (I2C_ID_HIGH) bits 0x18
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Write Serial # Low Byte For I2C Addr Unlock  (I2C_ID_LOW) bits 0x19
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Write New I2C Addr After Unlock  (I2C_SEC_ADDR) bits 0x1A
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Peak Detection Threshold Bypass  (THRESHOLD_BYPASS) bits 0x1C
 *
 *
 *
 * */
#define TH_DEFAULT    0x00
// Recommended sensitivity settings
#define TH_HIGH_SENSITIVITY 0x20 // Higher sensitivity, longer range S=32
#define TH_LOW_SENSITIVITY 0x60 // Lower sensitivity, shorter range S=96
// Macro to set custom sensitivity level
#define TH_SENSITIVITY_LEVEL(level)   (level & 0xFF) // Set sensitivity level (S=0-255)


///////////////////////////////////////////////////////////////
/* Default Address Response Control  (I2C_CONFIG) bits 0x1E
 *
 *
 *
 * */
// Bit 3: Default Address Response Control
#define I2C_RESPOND_TO_DEFAULT_AND_CUSTOM    0x00  // Responds to 0x62 and custom address
#define I2C_RESPOND_TO_CUSTOM_ONLY           0x08  // Only responds to custom address, ignores 0x62

///////////////////////////////////////////////////////////////
/* State Command  (COMMAND) bits 0x40
 *
 *
 *
 * */
// Bit 2:0: Command to the device
#define TEST_MODE_ENABLE         0x07     // Test Mode Enable, allows download of correlation record.
#define TEST_MODE_DISABLE        0x00     // Test Mode Disable, normal operation.


///////////////////////////////////////////////////////////////
/* Delay between Automatic Measurement  (MEASURE_DELAY) bits 0x45
 * Initial Value: 0x14
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Second largest peak value in correlation record  (PEAK_BCK) bits 0x4C
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Correlation Record Data Low Byte  (CORR_DATA) bits 0x52
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Correlation Record Data High Byte  (CORR_DATA_SIGN) bits 0x53
 *
 *
 *
 * */


///////////////////////////////////////////////////////////////
/* Correlation Record Memory bank select  (ACQ_SETTINGS) bits 0x5D
 *
 *
 *
 * */
// Bit 7-6 Access to Correlation Record Memory Banks
#define ACCESS_CORR_MEM_BANK 0xC0
#define NO_ACCESS_CORR_MEM_BANK 0x00


///////////////////////////////////////////////////////////////
/* Power State Control  (POWER_CONTROL) bits 0x65
 * Initial Value: 0x80
 *
 *
 * */

// Bit 2: Device Wake mode
#define DEVICE_SLEEP    0x04
#define DEVICE_AWAKE    0x00

// Bit 0 : Set Receiver Circuit
#define ENABLE_RECEIVER_CIRCUIT    0x01
#define DISABLE_RECEIVER_CIRCUIT   0x00





// Function prototypes

///////////////////////////////////////////////////////////////////////////////////////
// All register read/write and configuration functions

// I2C read and write functions
void GarLiteV3_i2c_write(uint8_t reg, uint8_t *buf, uint16_t len);
void GarLiteV3_i2c_read(uint8_t reg, uint8_t *buf, uint16_t len);

// Configuration functions

// ACQ_COMMAND register
void config_ACQ_COMMAND_reg(uint8_t command, uint8_t print);
uint8_t read_ACQ_COMMAND_reg(void);
void print_config_ACQ_COMMAND_reg(void);

// STATUS register
uint8_t read_STATUS_reg(uint8_t print);
void print_STATUS_reg(uint8_t regval);

// SIG_COUNT_VAL register
void config_SIG_COUNT_VAL_reg(uint8_t sigCountVal, uint8_t print);
uint8_t read_SIG_COUNT_VAL_reg(uint8_t print);

// ACQ_CONFIG_REG register
void config_ACQ_CONFIG_REG(uint8_t RefProcess, uint8_t delayMode, uint8_t RefFilter, uint8_t QuickTermination, uint8_t AcqCount, uint8_t PinFunc ,uint8_t print);
void print_config_ACQ_CONFIG_REG(void);

// VELOCITY register
uint8_t read_VELOCITY_reg(uint8_t print);

// PEAK_CORR register
uint8_t read_PEAK_CORR_reg(uint8_t print);

// NOISE_PEAK register
uint8_t read_NOISE_PEAK_reg(uint8_t print);

// SIGNAL_STRENGTH register
uint8_t read_SIGNAL_STRENGTH_reg(uint8_t print);

// FULL_DELAY_HIGH and FULL_DELAY_LOW registers
uint8_t read_FULL_DELAY_HIGH_reg(uint8_t print);
uint8_t read_FULL_DELAY_LOW_reg(uint8_t print);

// OUTER_LOOP_COUNT register
void config_OUTER_LOOP_COUNT_reg(uint8_t count, uint8_t print);

// REF_COUNT_VAL register
uint8_t read_REF_COUNT_VAL_reg(uint8_t print);

// LAST_DELAY_HIGH register
uint8_t read_LAST_DELAY_HIGH_reg(uint8_t print);

// LAST_DELAY_LOW register
uint8_t read_LAST_DELAY_LOW_reg(uint8_t print);

// UNIT_ID_HIGH register
void read_UNIT_ID_HIGH_reg(uint8_t print);

// UNIT_ID_LOW register
void read_UNIT_ID_LOW_reg(uint8_t print);

// I2C_ID_HIGH register
void read_I2C_ID_HIGH_reg(uint8_t print);

// I2C_ID_LOW register
void read_I2C_ID_LOW_reg(uint8_t print);

// I2C_SEC_ADDR register
void read_I2C_SEC_ADDR_reg(uint8_t print);

// THRESHOLD_BYPASS register
void config_THRESHOLD_BYPASS_reg(uint8_t sensitivity, uint8_t print);

// COMMAND register
void config_COMMAND_reg(uint8_t command, uint8_t print);

// MEASURE_DELAY register
void config_MEASURE_DELAY_reg(uint8_t delay, uint8_t print);

// PEAK_BCK register
uint8_t read_PEAK_BCK_reg(uint8_t print);

// CORR_DATA register
uint8_t read_CORR_DATA_reg(uint8_t print);

// CORR_DATA_SIGN register
uint8_t read_CORR_DATA_SIGN_reg(uint8_t print);

// ACQ_SETTINGS register
void config_ACQ_SETTINGS_reg(uint8_t bankAccess, uint8_t print);

// POWER_CONTROL register
void config_POWER_CONTROL_reg(uint8_t wakeMode, uint8_t receiverCircuit, uint8_t print);




//////////////////////////////////////////////////////////////////////////////////////
// High level functions

void GarLiteV3_Init(void);
void simple_measurement(uint16_t *distanceCm, uint8_t print);
void acq_count_measurements(uint16_t *distanceCm, uint8_t print);
void burst_measurements(uint16_t *distanceCm, uint16_t numBurst, uint8_t print);







#endif /* INC_GARMINLIDARLITEV3_H_ */
