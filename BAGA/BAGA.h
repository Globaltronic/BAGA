/*
* File:    BAGA.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/


/***
* BAGA A1
*
* BAGA Board Framework
*
* HW Compatibility: A2
***/


#ifndef BAGA_H
#define BAGA_H



/* Include Files */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <BMP280.h>
#include <Si7020.h>

//#define 	LIGHT_SENSOR_SI1132
#define 	LIGHT_SENSOR_PHOTO_TRANSISTOR

#define 	BAGA_SOLAR_RADIATION_PIN 					A0
#define 	BAGA_BATTERY_VOLTAGE_PIN 					A5
#define 	BAGA_LED_PIN 								5
#define 	BAGA_3V_SHDN_PIN 							6
#define 	BAGA_SDCARD_CS_PIN							12

#define		BAGA_TEMPERATURE_C_TO_F(x) 					((x * 1.8) + 32)
#define		BAGA_TEMPERATURE_C_TO_K(x) 					(x + 273.75)
#define		BAGA_TEMPERATURE_F_TO_C(x) 					((x - 32) / 1.8)
#define		BAGA_TEMPERATURE_F_TO_K(x) 					((x + 459.67) * 5 / 9)

#define 	BAGA_PRESSURE_MBAR_TO_KPA(x)				(x * 100)
#define 	BAGA_PRESSURE_MBAR_TO_PSI(x)				(x * 0.0145037738)

#define		BAGA_SEA_LEVEL_PRESSURE_PA					101325
#define		BAGA_SEA_LEVEL_PRESSURE_MBAR				1013.25



class BAGA
{
	public:
		BAGA();
		
		byte 			begin(void);
		byte			readSensors(void);
		
		float			readTemperatureC(void);
		float 			readTemperatureF(void);
		unsigned long	readTemperatureK(void);
		float 			readHeatIndexC(void);
		float 			readHeatIndexF(void);
		float 			readHeatIndexK(void);
		float 			readAbsHumidity(void);
		float 			readRelHumidity(void);

		float 			readAbsPressuremBar(void);
		float 			readAbsPressureBar(void);
		signed long		readAbsPressurePa(void);
		float 			readAbsPressureKPa(void);
		float 			readAbsPressurePsi(void);
		float 			readRelPressuremBar(void);
		float 			readRelPressureBar(void);
		signed long		readRelPressurePa(void);
		float 			readRelPressureKPa(void);
		float 			readRelPressurePsi(void);

#ifdef LIGHT_SENSOR_SI1132	
		unsigned int 	readUVIndex(void);
		unsigned long 	readIRLight(void);
		unsigned int 	readVisibleLight(void);
#endif
		
		int 			readSolarRadiationAdc(void);
		unsigned long 	readSolarRadiation(void);
		unsigned long 	readBatteryVoltage(void);
		void 			setLedOn(void);	// Turn LED on
		void 			setLedOff(void); // Turn LED off
		
	private:
		float 			_bagaRelativeHumidity; // Relative Humidity in %
		float			_bagaTemperatureC; // Temperature in Celsius degrees
		float 			_bagaPressuremBar; // Pressure in millibar
		signed long 	_bagaPressurePa; // Pressure in Pa
	
		BMP280 _sensorPressureTemperature;
		Si7020 _sensorHumidityTemperature;
#ifdef LIGHT_SENSOR_SI1132
		Si1132 _sensorLight;
#endif
};
























#endif