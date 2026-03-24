/*
 * LIS3DH.h
 *
 *  Created on: Oct 17, 2025
 *      Author: erick
 */

#ifndef INC_LIS3DH_H_
#define INC_LIS3DH_H_


#include "stm32h7xx_hal.h"
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// LIS3DH I2C Address

#define LIS3DH_ADDRESS         0x18 << 1  // Change according to your wiring (SA0 pin)


////////////////////////////////////////////////////////////////////////////////////////////////////


// LIS3DH  Map Register and their Addresses

#define STATUS_REG_AUX      0x07
#define OUT_ADC1_L          0x08
#define OUT_ADC1_H          0x09
#define OUT_ADC2_L          0x0A
#define OUT_ADC2_H          0x0B
#define OUT_ADC3_L          0x0C
#define OUT_ADC3_H          0x0D
#define INT_COUNTER_REG     0x0E
#define WHO_AM_I            0x0F
#define TEMP_CFG_REG        0x1F
#define CTRL_REG1           0x20
#define CTRL_REG2           0x21
#define CTRL_REG3           0x22
#define CTRL_REG4           0x23
#define CTRL_REG5           0x24
#define CTRL_REG6           0x25
#define REFERENCE           0x26
#define STATUS_REG          0x27
#define OUT_X_L             0x28
#define OUT_X_H             0x29
#define OUT_Y_L             0x2A
#define OUT_Y_H             0x2B
#define OUT_Z_L             0x2C
#define OUT_Z_H             0x2D
#define FIFO_CTRL_REG       0x2E
#define FIFO_SRC_REG        0x2F
#define INT1_CFG            0x30
#define INT1_SOURCE         0x31
#define INT1_THS            0x32
#define INT1_DURATION       0x33
#define INT2_CFG            0x34
#define INT2_SOURCE         0x35
#define INT2_THS            0x36
#define INT2_DURATION       0x37
#define CLICK_CFG           0x38
#define CLICK_SRC           0x39
#define CLICK_THS           0x3A
#define TIME_LIMIT          0x3B
#define TIME_LATENCY        0x3C
#define TIME_WINDOW         0x3D
#define ACT_THS             0x3E
#define ACT_DUR             0x3F


////////////////////////////////////////////////////////////////////////////////////////////////////
// Register desciptions and settings



//////////////////////////////////////////////////////////////
/* Status Register (STATUS_AUX) bits 0x07
 * Description of each bit in the STATUS_AUX_REG register
 *
 *
 *
 * */
// Bit 7: 123OR - Axis data overrun
#define ENABLE_321_DATA_OVERRUN    0x80
#define DISABLE_321_DATA_OVERRUN   0x00

// Bit 6: 3OR - 3-axis data overrun
#define ENABLE_3_AXIS_DATA_OVERRUN    0x40
#define DISABLE_3_AXIS_DATA_OVERRUN   0x00

// Bit 5: 2OR - 2-axis data overrun
#define ENABLE_2_AXIS_DATA_OVERRUN    0x20
#define DISABLE_2_AXIS_DATA_OVERRUN   0x00

// Bit 4: 1OR - 1-axis data overrun
#define ENABLE_1_AXIS_DATA_OVERRUN    0x10
#define DISABLE_1_AXIS_DATA_OVERRUN   0x00

// Bit 3: 321DA - New data available for all axes
#define ENABLE_321_NEW_DATA_AVAILABLE    0x08
#define DISABLE_321_NEW_DATA_AVAILABLE   0x00

// Bit 2: 3DA - New data available for 3-axis
#define ENABLE_3_NEW_DATA_AVAILABLE    0x04
#define DISABLE_3_NEW_DATA_AVAILABLE   0x00

// Bit 1: 2DA - New data available for 2-axis
#define ENABLE_2_NEW_DATA_AVAILABLE    0x02
#define DISABLE_2_NEW_DATA_AVAILABLE   0x00

// Bit 0: 1DA - New data available for 1-axis
#define ENABLE_1_NEW_DATA_AVAILABLE    0x01
#define DISABLE_1_NEW_DATA_AVAILABLE   0x00

/////////////////////////////////////////////////////////////////



/* Temperature Configuration Register (TEMP_CFG_REG) bits 0x1F
 * Description of each bit in the TEMP_CFG_REG register
 *
 *
 *
 * */
#define ENABLE_TEMPERATURE_SENSOR    0xC0
#define DISABLE_TEMPERATURE_SENSOR   0x00

/////////////////////////////////////////////////////////////////

/* Control Register 1 (CTRL_REG1) bits 0x20
 * Description of each bit in the CTRL_REG1 register
 *
 *
 * Here is current consumption for different data rates:
 *
 *  Operation Mode (Hz) | Low Power Mode (uA)  | Normal Power Mode (uA)  | High-Resolution Mode (uA)
 * ---------------------|----------------------|-------------------------|---------------------------
 *      1               |          2           |            2            |           2
 *      10              |          3           |            4            |           4
 *      25              |          4           |            6            |           6
 *      50              |          6           |            11           |           11
 *      100             |         10           |            20           |           20
 *      200             |         18           |            38           |           38
 *      400             |         36           |            73           |           73
 *      1344            |        ---           |           185           |          185
 *      1620            |        100           |           ---           |          ---
 *      5376            |        185           |           ---           |          ---
 *
 *
 *
 *
 *
 *
 * */


// Bit 7-4: ODR3, ODR2, ODR1, ODR0 - Data rate selection
#define ODR_POWER_DOWN        0x00  // Power-down mode
#define ODR_1HZ               0x10  // 1 Hz
#define ODR_10HZ              0x20  // 10 Hz
#define ODR_25HZ              0x30  // 25 Hz
#define ODR_50HZ              0x40  // 50 Hz
#define ODR_100HZ             0x50  // 100 Hz
#define ODR_200HZ             0x60  // 200 Hz
#define ODR_400HZ             0x70  // 400 Hz
#define ODR_1344HZ            0x80  // 1344 Hz
#define ODR_1620HZ_LP         0x90  // 1620 Hz Low Power
#define ODR_5376HZ_LP         0xA0  // 5376 Hz Low Power

// Bit 3: LPen - Low-power mode enable
#define ENABLE_LOW_POWER_MODE      0x08
#define ENABLE_NORMAL_POWER_MODE   0x00

// Bit 2: Zen - Z-axis enable
#define ENABLE_Z_AXIS      0x04 //  0000 0100
#define DISABLE_Z_AXIS     0x00

// Bit 1: Yen - Y-axis enable
#define ENABLE_Y_AXIS      0x02
#define DISABLE_Y_AXIS     0x00

// Bit 0: Xen - X-axis enable
#define ENABLE_X_AXIS      0x01
#define DISABLE_X_AXIS     0x00

// conbined axis enable
#define ENABLE_ALL_AXES    0x07
#define ENABLE_XY_AXES     0x03
#define ENABLE_XZ_AXES     0x05
#define ENABLE_YZ_AXES     0x06


/////////////////////////////////////////////////////////////////

/* Control Register 2 (CTRL_REG2) bits 0x21
 * Description of each bit in the CTRL_REG2 register
 *
 *
 *
 *
 *
 * */

// Bit 7-6: HPM1 HPM0 - High-pass filter mode
#define HPM_NORMAL_MODE_After_RESET    0x00
#define HPM_REF_SIGNAL                 0x40
#define HPM_NORMAL_MODE                0x80
#define HPM_AUTORESET                  0xC0

// Bit 5-4: HPCF2 HPCF1 - High-pass filter cutoff frequency
/*
 * Table of cutoff frequencies
 *
 *      HCPF2 HPCF1 | 1Hz ODR | 10Hz ODR | 25Hz ODR | 50Hz ODR | 100Hz ODR | 200Hz ODR | 400Hz ODR | 1600Hz ODR | 5000Hz ODR
 *    --------------|---------|----------|----------|----------|-----------|-----------|-----------|------------|------------
 *         00       |   0.02  |   0.2    |   0.5    |    1     |     2     |     4     |     8     |    32      |    100
 *         01       |   0.008 |   0.08   |   0.2    |  0.5     |     1     |     2     |     4     |    16      |     50
 *         10       |   0.004 |   0.04   |   0.1    |  0.2     |    0.5    |     1     |     2     |     8      |     25
 *         11       |   0.002 |   0.02   |   0.05   |  0.1     |    0.2    |    0.5    |     1     |     4      |     12
 *
 */

#define HPCF_CUTOFF_0    0x00
#define HPCF_CUTOFF_1    0x10
#define HPCF_CUTOFF_2    0x20
#define HPCF_CUTOFF_3    0x30

// Bit 3: FDS - Filtered data selection
#define ENABLE_FILTERED_DATA_SELECTION    0x08
#define DISABLE_FILTERED_DATA_SELECTION   0x00

// Bit 2: HPCLICK - High-pass filter for click
#define ENABLE_HPF_CLICK    0x04
#define DISABLE_HPF_CLICK   0x00

// Bit 1: HPIS2 - High-pass filter for AOI function interrupt 2
#define ENABLE_HPF_AOI_INT2    0x02
#define DISABLE_HPF_AOI_INT2   0x00

// Bit 0: HPIS1 - High-pass filter for AOI function interrupt 1
#define ENABLE_HPF_AOI_INT1    0x01
#define DISABLE_HPF_AOI_INT1   0x00

/////////////////////////////////////////////////////////////////


/* Control Register 3 (CTRL_REG3) bits 0x22
 * Description of each bit in the CTRL_REG3 register
 *
 *
 *
 *
 *
 * */

// Bit 7: I1_CLICK - Interrupt 1 click enable
#define ENABLE_INT1_CLICK    0x80
#define DISABLE_INT1_CLICK   0x00

// Bit 6: I1_AOI1 - Interrupt 1 AOI1 enable
#define ENABLE_INT1_AOI1    0x40
#define DISABLE_INT1_AOI1   0x00

// Bit 5: I1_AOI2 - Interrupt 1 AOI2 enable
#define ENABLE_INT1_AOI2    0x20
#define DISABLE_INT1_AOI2   0x00

// Bit 4: I1_DRDY1 - Interrupt 1 data ready 1 enable
#define ENABLE_INT1_DATA_READY1    0x10
#define DISABLE_INT1_DATA_READY1   0x00

// Bit 3: I1_DRDY2 - Interrupt 1 data ready 2 enable
#define ENABLE_INT1_DATA_READY2    0x08
#define DISABLE_INT1_DATA_READY2   0x00

// Bit 2: I1_WTM - Interrupt 1 FIFO watermark enable
#define ENABLE_INT1_FIFO_WATERMARK    0x04
#define DISABLE_INT1_FIFO_WATERMARK   0x00

// Bit 1: I1_OVERRUN - Interrupt 1 overrun enable
#define ENABLE_INT1_OVERRUN    0x02
#define DISABLE_INT1_OVERRUN   0x00

// Bit 0: I1_EMPTY - Interrupt 1 FIFO empty enable



/////////////////////////////////////////////////////////////////


/* Control Register 4 (CTRL_REG4) bits 0x23
 * Description of each bit in the CTRL_REG4 register
 *
 *
 *
 *
 *
 * */

// Bit 7: BDU - Block data update
#define ENABLE_BLOCK_DATA_UPDATE    0x80
#define DISABLE_BLOCK_DATA_UPDATE   0x00

// Bit 6: BLE - Big/little endian data selection
#define ENABLE_LITTLE_ENDIAN    0x00
#define ENABLE_BIG_ENDIAN       0x40

// Bit 5-4: FS1 FS0 - Full-scale selection
#define FS_2G      0x00
#define FS_4G      0x10
#define FS_8G      0x20
#define FS_16G     0x30

// Bit 3: HR - High-resolution output mode
#define ENABLE_HIGH_RESOLUTION_MODE    0x08
#define DISABLE_HIGH_RESOLUTION_MODE   0x00

// Bit 2: ST1 - Self-test enable 1
#define ENABLE_SELF_TEST1    0x04

// Bit 1: ST0 - Self-test enable 0
#define ENABLE_SELF_TEST0    0x02
#define DISABLE_SELF_TEST    0x00 // Normal Mode

// Bit 0: SIM - SPI serial interface mode selection
#define ENABLE_4_WIRE_SPI    0x00
#define ENABLE_3_WIRE_SPI    0x01



/////////////////////////////////////////////////////////////////

/* Control Register 5 (CTRL_REG5) bits 0x24
 * Description of each bit in the CTRL_REG5 register
 *
 *
 *
 *
 *
 * */

// Bit 7: BOOT - Reboot memory content
#define REBOOT_MEMORY_CONTENT    0x80
#define NORMAL_OPERATION         0x00

// Bit 6: FIFO_EN - FIFO enable
#define ENABLE_FIFO    0x40
#define DISABLE_FIFO   0x00

// Bit 3: LIR_INT1 - Latch interrupt request (LIR) on INT1_SRC register
#define ENABLE_LATCHED_INT1    0x08
#define DISABLE_LATCHED_INT1   0x00

// Bit 2: D4D_INT1 - 4D detection on INT1
#define ENABLE_4D_INT1    0x04
#define DISABLE_4D_INT1   0x00



//////////////////////////////////////////////////////////////////

/* Control Register 6 (CTRL_REG6) bits 0x25
 * Description of each bit in the CTRL_REG6 register
 *
 *
 *
 *
 *
 * */

// Bit 7: I2_CLICK - Interrupt 2 click enable
#define ENABLE_INT2_CLICK    0x80
#define DISABLE_INT2_CLICK   0x00

// Bit 6: I2_AOI1 - Interrupt 2 AOI1 enable
#define ENABLE_INT2_AOI1    0x40
#define DISABLE_INT2_AOI1   0x00

// Bit 5: I2_AOI2 - Interrupt 2 AOI2 enable
#define ENABLE_INT2_AOI2    0x20
#define DISABLE_INT2_AOI2   0x00

// Bit 4: BootI1nt2 - Boot interrupt 2 enable
#define ENABLE_BOOT_INT2    0x10
#define DISABLE_BOOT_INT2   0x00

// Bit 3: I2_ACTI - Interrupt 2 activity enable
#define ENABLE_INT2_ACTIVITY    0x08
#define DISABLE_INT2_ACTIVITY   0x00

// Bit 1 Int1 and Int2 pin polarity configuration
#define ENABLE_INT_Polarity    0x02
#define DISABLE_INT_Polarity     0x00



//////////////////////////////////////////////////////////////////

/* Status Register (STATUS_REG) bits 0x27
 * Description of each bit in the Status register
 *
 *
 *
 *
 *
 * */
// Bit 7: ZYXOR - X, Y, Z axis data overrun
#define STATUS_REG_ZYXOR    0x80

// Bit 6: ZOR - Z axis data overrun
#define STATUS_REG_ZOR     0x40

// Bit 5: YOR - Y axis data overrun
#define STATUS_REG_YOR     0x20

// Bit 4: XOR - X axis data overrun
#define STATUS_REG_XOR     0x10

// Bit 3: ZYXDA - X, Y, Z axis new data available
#define STATUS_REG_ZYXDA    0x08

// Bit 2: ZDA - Z axis new data available
#define STATUS_REG_ZDA     0x04

// Bit 1: YDA - Y axis new data available
#define STATUS_REG_YDA     0x02

// Bit 0: XDA - X axis new data available
#define STATUS_REG_XDA     0x01



//////////////////////////////////////////////////////////////////

/* FIFO Control Register (FIFO_CTRL_REG) bits 0x2E
 * Description of each bit in the FIFO Control Register
 * Sets Up FIFO mode selection and watermark level
 *
 *
 *
 *
 * */
// Bit 7-6: FM1 FM0 - FIFO mode selection
#define BYPASS_MODE         0x00 // Best for real-time single reading
#define FIFO_MODE           0x40 // Best for not losing any samples
#define STREAM_MODE         0x80 // Always have the latest data
#define STREAM_TO_FIFO_MODE 0xC0 // Capture before + after trigger event

// Bit 5 Trigger selection
#define ENABLE_TRIGGER_INT1    0x20
#define ENABLE_TRIGGER_INT2   0x00

// Bit 4-0: FTH4 FTH3 FTH2 FTH1 FTH0 - Sets up the sample of data stashed in FIFO
// before a FIFO watermark interrupt is generated
#define FIFO_WATERMARK_LEVEL(level)   (level & 0x1F)

//////////////////////////////////////////////////////////////////

/* FIFO Source Register (FIFO_SRC_REG) bits 0x2F
 * Description of each bit in the FIFO Source Register
 * Provides information about the FIFO status
 * Updated at every ODR and provide information about the FIFO buffer status
 *
 *
 *
 *
 * */

// Bit 7: WTM - Watermark status
#define EXCEEDED_WTM     0x80

// Bit 6: OVRN_FIFO - Overrun bit
#define FIFO_OVERRUN     0x40

// Bit 5: EMPTY - FIFO empty status
#define FIFO_EMPTY       0x20

// Bit 4-0: FSS4 FSS3 FSS2 FSS1 FSS0 - FIFO stored data level
#define FIFO_STORED_DATA_LEVEL(level)   (level & 0x1F)

//////////////////////////////////////////////////////////////////

/* Interrupt 1 Configure Register (INT1_CFG_REG) bits 0x30
 * Description of each bit in the Interrupt 1 Configure Register
 *
 *
 *
 *
 *
 *
 * */

// BIT 7: AOI - And/Or combination of interrupt events
#define INT1_OR_COMBINATION        0x00
#define INT1_AND_COMBINATION       0x80

// Bit 6: 6D - 6-direction detection
#define ENABLE_INT1_6D_DETECTION    0x40
#define DISABLE_INT1_6D_DETECTION   0x00

// Bit 5: ZHIE/ZUPE - Z high event
#define ENABLE_INT1_Z_HIGH_EVENT    0x20
#define DISABLE_INT1_Z_HIGH_EVENT   0x00

// Bit 4: ZLIE/ZDOWNE - Z low event
#define ENABLE_INT1_Z_LOW_EVENT    0x10
#define DISABLE_INT1_Z_LOW_EVENT   0x00

// Bit 3: YHIE/YUPE - Y high event
#define ENABLE_INT1_Y_HIGH_EVENT    0x10
#define DISABLE_INT1_Y_HIGH_EVENT   0x00

// Bit 2: YLIE/YDOWNE - Y low event
#define ENABLE_INT1_Y_LOW_EVENT    0x04
#define DISABLE_INT1_Y_LOW_EVENT   0x00

// Bit 1: XHIE/XUPE - X high event
#define ENABLE_INT1_X_HIGH_EVENT    0x08
#define DISABLE_INT1_X_HIGH_EVENT   0x00

// Bit 0: XLIE/XDOWNE - X low event
#define ENABLE_INT1_X_LOW_EVENT    0x01
#define DISABLE_INT1_X_LOW_EVENT   0x00

//////////////////////////////////////////////////////////////////

/* Interrupt 1 Source Register (INT1_SRC_REG) bits 0x31
 * Description of each bit in the Interrupt 1 Source Register
 *
 *
 *
 *
 *
 *
 * */

// Bit 6 : IA - Interrupt active
#define INT1_ACTIVE     0x40
#define INT1_NOT_ACTIVE 0x00

// Bit 5: ZH - Z high
#define INT1_Z_HIGH     0x20
#define INT1_Z_NOT_HIGH 0x00

// Bit 4: ZL - Z low
#define INT1_Z_LOW      0x10
#define INT1_Z_NOT_LOW  0x00

// Bit 3: YH - Y high
#define INT1_Y_HIGH     0x08
#define INT1_Y_NOT_HIGH 0x00

// Bit 2: YL - Y low
#define INT1_Y_LOW      0x04
#define INT1_Y_NOT_LOW  0x00

// Bit 1: XH - X high
#define INT1_X_HIGH     0x02
#define INT1_X_NOT_HIGH 0x00

// Bit 0: XL - X low
#define INT1_X_LOW      0x01
#define INT1_X_NOT_LOW  0x00

//////////////////////////////////////////////////////////////////

/* Interrupt 1 Threshold Register (INT1_THS_REG) bits 0x32
 * Description of each bit in the Interrupt 1 Threshold Register
 * Sets the threshold value for interrupt 1
 *
 *
 *
 *
 *
 * */

// Bit 7-0: THS6 THS5 THS4 THS3 THS2 THS1 THS0 - Threshold value for interrupt 1
#define INT1_THRESHOLD_VALUE(value)   (value & 0x7F)

//////////////////////////////////////////////////////////////////

/* Interrupt 1 Duration Register (INT1_DURATION_REG) bits 0x33
 * Description of each bit in the Interrupt 1 Duration Register
 * Set the duration value for interrupt 1
 *
 *
 *
 *
 *
 * */

// Bit 7-0: D7 D6 D5 D4 D3 D2 D1 D0 - Duration value for interrupt 1
#define INT1_DURATION_VALUE(value)   (value & 0xFF)

//////////////////////////////////////////////////////////////////

/* Interrupt 2 Configure Register (INT2_CFG_REG) bits 0x34
 * Description of each bit in the Interrupt 2 Configure Register
 *
 *
 *
 *
 *
 *
 * */

// BIT 7: AOI - And/Or combination of interrupt events
#define INT2_OR_COMBINATION        0x00
#define INT2_AND_COMBINATION       0x80

// Bit 6: 6D - 6-direction detection
#define ENABLE_INT2_6D_DETECTION    0x40
#define DISABLE_INT2_6D_DETECTION   0x00

// Bit 5: ZHIE/ZUPE - Z high event
#define ENABLE_INT2_Z_HIGH_EVENT    0x20
#define DISABLE_INT2_Z_HIGH_EVENT   0x00

// Bit 4: ZLIE/ZDOWNE - Z low event
#define ENABLE_INT2_Z_LOW_EVENT    0x10
#define DISABLE_INT2_Z_LOW_EVENT   0x00

// Bit 3: YHIE/YUPE - Y high event
#define ENABLE_INT2_Y_HIGH_EVENT    0x10
#define DISABLE_INT2_Y_HIGH_EVENT   0x00

// Bit 2: YLIE/YDOWNE - Y low event
#define ENABLE_INT2_Y_LOW_EVENT    0x04
#define DISABLE_INT2_Y_LOW_EVENT   0x00

// Bit 1: XHIE/XUPE - X high event
#define ENABLE_INT2_X_HIGH_EVENT    0x08
#define DISABLE_INT2_X_HIGH_EVENT   0x00

// Bit 0: XLIE/XDOWNE - X low event
#define ENABLE_INT2_X_LOW_EVENT    0x01
#define DISABLE_INT2_X_LOW_EVENT   0x00

//////////////////////////////////////////////////////////////////

/* Interrupt 2 Source Register (INT2_SRC_REG) bits 0x35
 * Description of each bit in the Interrupt 2 Source Register
 *
 *
 *
 *
 *
 *
 * */

// Bit 6 : IA - Interrupt active
#define INT2_ACTIVE     0x40
#define INT2_NOT_ACTIVE 0x00

// Bit 5: ZH - Z high
#define INT2_Z_HIGH     0x20
#define INT2_Z_NOT_HIGH 0x00

// Bit 4: ZL - Z low
#define INT2_Z_LOW      0x10
#define INT2_Z_NOT_LOW  0x00

// Bit 3: YH - Y high
#define INT2_Y_HIGH     0x08
#define INT2_Y_NOT_HIGH 0x00

// Bit 2: YL - Y low
#define INT2_Y_LOW      0x04
#define INT2_Y_NOT_LOW  0x00

// Bit 1: XH - X high
#define INT2_X_HIGH     0x02
#define INT2_X_NOT_HIGH 0x00

// Bit 0: XL - X low
#define INT2_X_LOW      0x01
#define INT2_X_NOT_LOW  0x00

//////////////////////////////////////////////////////////////////

/* Interrupt 2 Threshold Register (INT2_THS_REG) bits 0x36
 * Description of each bit in the Interrupt 2 Threshold Register
 * Sets the threshold value for interrupt 2
 *
 *
 *
 *
 *
 * */

// Bit 7-0: THS6 THS5 THS4 THS3 THS2 THS1 THS0 - Threshold value for interrupt 2
#define INT2_THRESHOLD_VALUE(value)   (value & 0x7F)

//////////////////////////////////////////////////////////////////

/* Interrupt 2 Duration Register (INT2_DURATION_REG) bits 0x37
 * Description of each bit in the Interrupt 2 Duration Register
 * Set the duration value for interrupt 2
 *
 *
 *
 *
 *
 * */

// Bit 7-0: D7 D6 D5 D4 D3 D2 D1 D0 - Duration value for interrupt 2
#define INT2_DURATION_VALUE(value)   (value & 0xFF)

//////////////////////////////////////////////////////////////////

/* Click Configuration Register (CLICK_CFG_REG) bits 0x38
 * Description of each bit in the Click Configuration Register
 *
 *
 *
 *
 *
 *
 * */

// Bit 5: ZD - Enable Z double click detection
#define ENABLE_Z_DOUBLE_CLICK_DETECTION    0x20
#define DISABLE_Z_DOUBLE_CLICK_DETECTION   0x00

// Bit 4: ZS - Enable Z single click detection
#define ENABLE_Z_SINGLE_CLICK_DETECTION    0x10
#define DISABLE_Z_SINGLE_CLICK_DETECTION   0x00

// Bit 3: YD - Enable Y double click detection
#define ENABLE_Y_DOUBLE_CLICK_DETECTION    0x08
#define DISABLE_Y_DOUBLE_CLICK_DETECTION   0x00

// Bit 2: YS - Enable Y single click detection
#define ENABLE_Y_SINGLE_CLICK_DETECTION    0x04
#define DISABLE_Y_SINGLE_CLICK_DETECTION   0x00

// Bit 1: XD - Enable X double click detection
#define ENABLE_X_DOUBLE_CLICK_DETECTION    0x02
#define DISABLE_X_DOUBLE_CLICK_DETECTION   0x00

// Bit 0: XS - Enable X single click detection
#define ENABLE_X_SINGLE_CLICK_DETECTION    0x01
#define DISABLE_X_SINGLE_CLICK_DETECTION   0x00

//////////////////////////////////////////////////////////////////

/* Click Source Register (CLICK_SRC_REG) bits 0x39
 * Description of each bit in the Click Source Register
 *
 *
 *
 *
 *
 *
 * */

// Bit 6 : IA - Click interrupt active
#define CLICK_INT_ACTIVE     0x40
#define CLICK_INT_NOT_ACTIVE 0x00

// Bit 5: DCLICK - Double click detected
#define DOUBLE_CLICK_DETECTED     0x20
#define DOUBLE_CLICK_NOT_DETECTED 0x00

// Bit 4: SCLICK - Single click detected
#define SINGLE_CLICK_DETECTED     0x10
#define SINGLE_CLICK_NOT_DETECTED 0x00

// Bit3 : SIGN - Click sign
#define CLICK_SIGN_POSITIVE     0x00
#define CLICK_SIGN_NEGATIVE     0x08

// Bit 2: Z - Z Click Detection
#define CLICK_Z_DETECTED     0x08
#define CLICK_Z_NOT_DETECTED 0x00

// Bit 1: Y - Y Click Detection
#define CLICK_Y_DETECTED     0x04
#define CLICK_Y_NOT_DETECTED 0x00

// Bit 0: X - X Click Detection
#define CLICK_X_DETECTED     0x02
#define CLICK_X_NOT_DETECTED 0x00

///////////////////////////////////////////////////////////////////

/* Click Threshold Register (CLICK_THS_REG) bits 0x3A
 * Description of each bit in the Click Threshold Register
 * Sets the threshold value for click detection
 *
 *
 *
 *
 *
 * */

// Bit 6-0: THS6 THS5 THS4 THS3 THS2 THS1 THS0 - Click threshold
#define CLICK_THRESHOLD_VALUE(value)   (value & 0x7F)

///////////////////////////////////////////////////////////////////

/* Time Limit Register (TIME_LIMIT_REG) bits 0x3B
 * Description of each bit in the Time Limit Register
 * Sets the time limit for click detection
 *
 *
 *
 *
 *
 * */
// Bit 7-0: TLI7 TLI6 TLI5 TLI4 TLI3 TLI2 TLI1 TLI0 - Time limit value
#define CLICK_TIME_LIMIT_VALUE(value)   (value & 0xFF)

///////////////////////////////////////////////////////////////////

/* Time Latency Register (TIME_LATENCY_REG) bits 0x3C
 * Description of each bit in the Time Latency Register
 * Sets the time latency for click detection
 *
 *
 *
 *
 *
 * */

// Bit 7-0: TLA7 TLA6 TLA5 TLA4 TLA3 TLA2 TLA1 TLA0 - Time latency value
#define CLICK_TIME_LATENCY_VALUE(value)   (value & 0xFF)

///////////////////////////////////////////////////////////////////

/* Time Window Register (TIME_WINDOW_REG) bits 0x3D
 * Description of each bit in the Time Window Register
 * Sets the time window for click detection
 *
 *
 *
 *
 *
 * */

// Bit 7-0: TWI7 TWI6 TWI5 TWI4 TWI3 TWI2 TWI1 TWI0 - Time window value
#define CLICK_TIME_WINDOW_VALUE(value)   (value & 0xFF)

///////////////////////////////////////////////////////////////////

/* Activity Threshold Register (ACT_THS_REG) bits 0x3E
 * Description of each bit in the Activity Threshold Register
 * Sets the threshold for activity detection
 *
 *
 *
 *
 *
 * */
// Bit 7-0: ACTH7 ACTH6 ACTH5 ACTH4 ACTH3 ACTH2 ACTH1 ACTH0 - Activity threshold value
#define ACTIVITY_THRESHOLD_VALUE(value)   (value & 0xFF)

///////////////////////////////////////////////////////////////////

/* Activity Duration Register (ACT_DUR_REG) bits 0x3F
 * Description of each bit in the Activity Duration Register
 * Sets the duration for activity detection
 *
 *
 *
 *
 *
 * */

// Bit 7-0: ACTD7 ACTD6 ACTD5 ACTD4 ACTD3 ACTD2 ACTD1 ACTD0 - Activity duration value
#define ACTIVITY_DURATION_VALUE(value)   (value & 0xFF)

///////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////
// Function Prototypes

// Low-level functions
//HAL_StatusTypeDef LIS3DH_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data);
//HAL_StatusTypeDef LIS3DH_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data);
//
//void lis3dh_read_registers(uint8_t reg, uint8_t *buf, uint16_t len);
//void lis3dh_write_registers(uint8_t reg, uint8_t *buf, uint16_t len);

// Renamed low-level I2C helpers to avoid symbol collisions with other libraries
void lis3dh_i2c_write(uint8_t reg, uint8_t *buf, uint16_t len);
void lis3dh_i2c_read(uint8_t reg, uint8_t *buf, uint16_t len);

// High-level test/helper

void lis3dh_init(void);

void who_am_i(void);

uint8_t read_int_counter_reg(uint8_t print);

void config_temp_cfg_reg(uint8_t enable, uint8_t print);
uint8_t read_temp_cfg_reg(void);
void print_config_temp_cfg_reg(void);

void config_ctrl_reg1(uint8_t ODR, uint8_t PowerMode, uint8_t SetAxis, uint8_t print);
uint8_t read_ctrl_reg1(void);
void print_config_ctrl_reg1(void);

void config_ctrl_reg2(uint8_t HighPassMode, uint8_t CutoffFreq, uint8_t FilteredData, uint8_t HPF_Click,
						 uint8_t HPF_AOI2, uint8_t HPF_AOI1, uint8_t print);
uint8_t read_ctrl_reg2(void);
void print_config_ctrl_reg2(void);

void config_ctrl_reg3(uint8_t Int1_Click, uint8_t Int1_AOI1, uint8_t Int1_AOI2,
							uint8_t Int1_DataReady1, uint8_t Int1_DataReady2,
							uint8_t Int1_FifoWatermark, uint8_t Int1_Overrun,
							uint8_t print);
uint8_t read_ctrl_reg3(void);
void print_config_ctrl_reg3(void);

void config_ctrl_reg4(uint8_t BlockDataUpdate, uint8_t Endianness,
							uint8_t FullScale, uint8_t HighResMode,
							uint8_t SelfTest, uint8_t SPIMode,
							uint8_t print);
uint8_t read_ctrl_reg4(void);
void print_config_ctrl_reg4(void);

void config_ctrl_reg5(uint8_t RebootMemory, uint8_t FifoEnable,
							uint8_t LatchInt1, uint8_t D4DInt1,
							uint8_t print);
uint8_t read_ctrl_reg5(void);
void print_config_ctrl_reg5(void);

void config_ctrl_reg6(uint8_t Int2_Click, uint8_t Int2_AOI1,
							uint8_t Int2_AOI2, uint8_t BootInt2,
							uint8_t Int2_Activity, uint8_t IntPolarity,
							uint8_t print);
uint8_t read_ctrl_reg6(void);
void print_config_ctrl_reg6(void);

uint8_t read_status_reg(uint8_t print);
void print_status_reg(uint8_t status);

void config_fifo_ctrl_reg(uint8_t FifoMode, uint8_t TriggerSelection,
							uint8_t WatermarkLevel, uint8_t print);
uint8_t read_fifo_ctrl_reg(void);
void print_config_fifo_ctrl_reg(void);

uint8_t read_fifo_src_reg(uint8_t print);
void print_fifo_src_reg(uint8_t regval);

// New helper to drain FIFO when overrun occurs
void lis3dh_drain_fifo(void);
// New helper to reset output path (power down then restore) to clear overrun/state
void lis3dh_reset_output(void);

uint8_t read_status_reg(uint8_t print);
void print_status_reg(uint8_t status);

void config_fifo_ctrl_reg(uint8_t FifoMode, uint8_t TriggerSelection,
							uint8_t WatermarkLevel, uint8_t print);
uint8_t read_fifo_ctrl_reg(void);
void print_config_fifo_ctrl_reg(void);

uint8_t read_fifo_src_reg(uint8_t print);
void print_fifo_src_reg(uint8_t regval);

void config_int1_cfg_reg(uint8_t AndOrCombination, uint8_t SixDDetection,
							uint8_t ZHighEvent, uint8_t ZLowEvent,
							uint8_t YHighEvent, uint8_t YLowEvent,
							uint8_t XHighEvent, uint8_t XLowEvent,
							uint8_t print);
uint8_t read_int1_cfg_reg(void);
void print_config_int1_cfg_reg(void);

void read_int1_src_reg(uint8_t print);
void print_int1_src_reg(uint8_t int1_src);

void print_config_int1_cfg(void);

void config_int1_ths_reg(uint8_t ThresholdValue, uint8_t print);
void print_config_int1_ths_reg(void);
void config_int1_duration_reg(uint8_t DurationValue, uint8_t print);
void print_config_int1_duration_reg(void);
void print_config_int1_ths(void);
void print_config_int1_duration(void);
void print_config_click_cfg(void);
void print_config_click_ths(void);
void print_config_time_limit(void);
void print_config_time_latency(void);
void print_config_time_window(void);




//
void read_XYZ_Acceleration(int16_t *x, int16_t *y, int16_t *z);
void read_XY_Acceleration(int16_t *x, int16_t *y);
void read_XZ_Acceleration(int16_t *x, int16_t *z);
void read_YZ_Acceleration(int16_t *y, int16_t *z);
void read_X_Acceleration(int16_t *x);
void read_Y_Acceleration(int16_t *y);
void read_Z_Acceleration(int16_t *z);

void print_XYZ_Acceleration(int16_t x, int16_t y, int16_t z);
void print_XY_Acceleration(int16_t x, int16_t y);
void print_XZ_Acceleration(int16_t x, int16_t z);
void print_YZ_Acceleration(int16_t y, int16_t z);
void print_X_Acceleration(int16_t x);
void print_Y_Acceleration(int16_t y);
void print_Z_Acceleration(int16_t z);


void check_all_Registers();


int16_t lis3dh_read_temperature_raw(void);
void lis3dh_print_temperature(void);

void lis3dh_init_fifo_version(void);
void check_fifo_ready(void);



#endif /* INC_LIS3DH_H_ */
