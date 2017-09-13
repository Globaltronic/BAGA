/*
* File:    BMP280.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/

/***
* BMP280 A1
*
* BMP280 driver
*
* HW Compatibility: A2
***/

#ifndef BMP280_H
#define BMP280_H

/* Include Files */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>


#define 	BMP280_SEA_LEVEL_PRESSURE_PA				101325
#define 	BMP280_SEA_LEVEL_PRESSURE_MBAR				1013.25

#define		BMP280_I2C_ADDRESS						0x76

#define		BMP280_MEASUREMENTS_TIMEOUT_MS			50

// Registers
#define 	BMP280_REG_CALIB_00						0x88
#define 	BMP280_REG_CALIB_01						0x89
#define 	BMP280_REG_CALIB_02						0x8A
#define 	BMP280_REG_CALIB_03						0x8B
#define 	BMP280_REG_CALIB_04						0x8C
#define 	BMP280_REG_CALIB_05						0x8D
#define 	BMP280_REG_CALIB_06						0x8E
#define 	BMP280_REG_CALIB_07						0x8F
#define 	BMP280_REG_CALIB_08						0x90
#define 	BMP280_REG_CALIB_09						0x91
#define 	BMP280_REG_CALIB_10						0x92
#define 	BMP280_REG_CALIB_11						0x93
#define 	BMP280_REG_CALIB_12						0x94
#define 	BMP280_REG_CALIB_13						0x95
#define 	BMP280_REG_CALIB_14						0x96
#define 	BMP280_REG_CALIB_15						0x97
#define 	BMP280_REG_CALIB_16						0x98
#define 	BMP280_REG_CALIB_17						0x99
#define 	BMP280_REG_CALIB_18						0x9A
#define 	BMP280_REG_CALIB_19						0x9B
#define 	BMP280_REG_CALIB_20						0x9C
#define 	BMP280_REG_CALIB_21						0x9D
#define 	BMP280_REG_CALIB_22						0x9E
#define 	BMP280_REG_CALIB_23						0x9F
#define 	BMP280_REG_CALIB_24						0xA0
#define 	BMP280_REG_CALIB_25						0xA1
#define 	BMP280_REG_ID							0xD0
#define 	BMP280_REG_RESET						0xE0
#define 	BMP280_REG_STATUS						0xF3
#define 	BMP280_REG_CTRL_MEASURE					0xF4
#define 	BMP280_REG_CONFIG						0xF5
#define 	BMP280_REG_PRESS_MSB					0xF7
#define 	BMP280_REG_PRESS_LSB					0xF8
#define 	BMP280_REG_PRESS_XLSB					0xF9
#define 	BMP280_REG_TEMP_MSB						0xFA
#define 	BMP280_REG_TEMP_LSB						0xFB
#define 	BMP280_REG_TEMP_XLSB					0xFC


// BMP280_REG_RESET
#define 	BMP280_RESET							0xB6 

// BMP280_REG_STATUS
#define		BMP280_NVM_UPDATE_BIT_MASK				0x01
#define		BMP280_NVM_UPDATE_DONE					0x00
#define		BMP280_MEASURING_BIT_MASK				0x04
#define		BMP280_MEASURING_DONE					0x00

// BMP280_REG_CTRL_MEASURE

#define		BMP280_MODE_BYTE_MASK					0x03
#define		BMP280_MODE_SLEEP						0x00
#define		BMP280_MODE_FORCED						0x01
#define		BMP280_MODE_NORMAL						0x03

#define		BMP280_PRESS_OVERSAMPLING_BYTE			2
#define		BMP280_PRESS_OVERSAMPLING_BYTE_MASK		0x1C
#define		BMP280_PRESS_OVERSAMPLING_OFF			0x00
#define		BMP280_PRESS_OVERSAMPLING_1				0x01 // Pressure resolution 16 bit / 0.0050 °C
#define		BMP280_PRESS_OVERSAMPLING_2				0x02 // Pressure resolution 17 bit / 0.0025 °C
#define		BMP280_PRESS_OVERSAMPLING_4				0x03 // Pressure resolution 18 bit / 0.0012 °C
#define		BMP280_PRESS_OVERSAMPLING_8				0x04 // Pressure resolution 19 bit / 0.0006 °C
#define		BMP280_PRESS_OVERSAMPLING_16			0x05 // Pressure resolution 20 bit / 0.0003 °C

#define		BMP280_TEMP_OVERSAMPLING_BYTE			5
#define		BMP280_TEMP_OVERSAMPLING_BYTE_MASK		0xE0
#define		BMP280_TEMP_OVERSAMPLING_OFF			0x00
#define		BMP280_TEMP_OVERSAMPLING_1				0x01 // Temperature resolution 16 bit / 0.0050 °C
#define		BMP280_TEMP_OVERSAMPLING_2				0x02 // Temperature resolution 17 bit / 0.0025 °C
#define		BMP280_TEMP_OVERSAMPLING_4				0x03 // Temperature resolution 18 bit / 0.0012 °C
#define		BMP280_TEMP_OVERSAMPLING_8				0x04 // Temperature resolution 19 bit / 0.0006 °C
#define		BMP280_TEMP_OVERSAMPLING_16				0x05 // Temperature resolution 20 bit / 0.0003 °C



class BMP280
{
	public:
		BMP280();
		
		byte 			begin(void); // Initializes BMP280 device
		bool 			isEnabled(void); // Check if the device is initialized
		byte 			reset(); // Reset device to its POR state
		byte 			makeSingleMeasurement(void); // Trigger the device to make a single measurement of temperature and pressure
		float 			readTemperature(void); // Read temperature from last measurement
		signed long		readPressure(void); // Read pressure from last measurement
		float 			calcAltitude(float absPressureMBar, float seaLevelPressureMBar, float temperatureC); // Calculate altitude for a given pressure and temperature
		bool 			isMeasurementDone(void); // Check if the measurement is completed (not valid for Forced Mode)
		bool 			isNVMDataReady(void); // Check if the NVM data was copied to image registers
		byte 			loadCalibrationData(void); // Load factory calibration data
		byte 			getMode(void); // Get current device mode value
		void 			setMode(byte mode); //  Set device mode
		void			setTempOversampling(byte tempOversampling); // Set device temperature oversampling setting
		void 			setPressureOversampling(byte pressureOversampling); // Set device pressure oversampling setting
		
	private:
		bool 			_isEnabled;
		bool 			_dataUpdated;
		byte 			_mode, _pressureOversampling, _temperatureOversampling;
		
		signed long 	_rawTemperature, _rawPressure, _tFine;
		float			_temperature;
		signed long		_pressure;
		
		unsigned short 	_calibT1, _calibP1; // Calibration data
		signed short 		_calibT2, _calibT3, _calibP2, _calibP3, _calibP4, _calibP5, _calibP6, _calibP7, _calibP8, _calibP9; // Calibration data
		byte			_oversampling;
		
		

		byte 			readInt(unsigned char address, signed short *value); // Read 16-bit signed integer from device
		byte 			readUInt(unsigned char address, unsigned short *value); // Read 16-bit unsigned integer from device
		byte 			readByte(unsigned char address, unsigned char *value); // Read byte from device
		byte 			readByteArray(unsigned char address, unsigned char * array, unsigned char length); // Read byte array from device
		byte 			writeByte(unsigned char address, unsigned char value); // Write byte to device
		byte 			writeByteArray(unsigned char address, unsigned char * array, unsigned char length); // Write byte array to device
};


















#endif