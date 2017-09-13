/*
* File:    Si7020.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/


/***
* Si7020 A1
*
* Si7020 driver
*
* HW Compatibility: A2
***/

#ifndef SI7020_H
#define SI7020_H

/* Include Files */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>

#define		SI7020_I2C_ADDRESS						0x40

#define 	SI7020_CMD_MEASURE_RH_HOLD					0xE5
#define 	SI7020_CMD_MEASURE_RH_NO_HOLD				0xF5
#define 	SI7020_CMD_MEASURE_TEMP_HOLD				0xE3
#define 	SI7020_CMD_MEASURE_TEMP_NO_HOLD				0xF3
#define 	SI7020_CMD_MEASURE_TEMP_PREV_RH				0xE0

#define 	SI7020_CMD_RESET							0xFE

#define 	SI7020_CMD_WRITE_RHT_USER_REG				0xE6
#define 	SI7020_CMD_WRITE_HEATER_REG					0x51

#define 	SI7020_CMD_READ_RHT_USER_REG				0xE7
#define 	SI7020_CMD_READ_HEATER_REG					0x11
#define 	SI7020_CMD_READ_EID_BYTE_1A					0xFA
#define 	SI7020_CMD_READ_EID_BYTE_1B					0x0F
#define 	SI7020_CMD_READ_EID_BYTE_2A					0xFC
#define 	SI7020_CMD_READ_EID_BYTE_2B					0xC9
#define 	SI7020_CMD_READ_FW_REV_A					0x84
#define 	SI7020_CMD_READ_FW_REV_B					0xB8


// Measurement Resolution:
// RH Temp
// 00: 12 bit 14 bit
// 01: 8 bit 12 bit
// 10: 10 bit 13 bit
// 11: 11 bit 11 bit
#define		SI7020_RES_MASK								0x81
#define		SI7020_RES_RH12_TEMP14						0x00
#define		SI7020_RES_RH08_TEMP12						0x01
#define		SI7020_RES_RH10_TEMP13						0x80
#define		SI7020_RES_RH11_TEMP11						0x81

#define		SI7020_HEATER_MASK							0x04
#define		SI7020_HEATER_OFF							0x00
#define		SI7020_HEATER_ON							0x04
#define		SI7020_HEATER_LEVEL_MASK					0x0F


class Si7020
{
	public:
		Si7020();
	
		byte 	begin(void); // Initializes Si7020 device
		bool 	isEnabled(void); // Check if the device is initialized
		byte 	reset(void); // Reset device to its POR state
		float 	readRelHumidity(void); // Read relative humidity
		float 	readTemperature(void); // Read temperature in Celsius degrees
		float 	readTemperaturePostHumidity(void); // Read temperature from the previous humidity measurement
		byte 	setHeaterOff(void); // Turn heater off
		byte 	setHeaterOn(void); // Turn heater on
		byte 	setHeaterLevel(byte level); // Set heater level
		
	private:
		bool _isEnabled;
		unsigned short _rawRelativeHumidity;
		unsigned short _rawTemperaturePostHumidity;
		unsigned short _rawTemperature;
		byte _heaterLevel;
		
		byte 	setResolution(byte resolution); // Set measurement resolution
		byte 	setHeaterState(byte state); // Set heater state
		
		byte 	readInt(byte address, signed short *value); // Read 16-bit signed integer from device
		byte 	readUInt(byte address, unsigned short *value); // Read 16-bit unsigned integer from device
		byte 	readByte(byte address, byte *value); // Read byte from device
		byte 	readByteArray(byte address, unsigned char * array, byte length); // Read byte array from device
		byte 	writeByte(byte address, byte value); // Write byte to device
		byte 	writeByteArray(byte address, unsigned char * array, byte length); // Write byte array to device
};
























#endif