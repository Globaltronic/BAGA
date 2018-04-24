/*
* File:    BAGA.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/

/* Include Files */
#include "BAGA.h"
#include "LowPower.h"

// Select the desired debug serial port:
// * Serial (USB-CDC)
// * Serial1 (J3 connector on board)

BMP280 _sensorPressureTemperature();
Si7020 _sensorHumidityTemperature();
#ifdef LIGHT_SENSOR_SI1132
Si1132 _sensorLight();
#endif

BAGA::BAGA()
{
	_bagaRelativeHumidity = 0;
	_bagaTemperatureC = 0;
	_bagaPressuremBar = 0;
	_bagaPressurePa = 0;
}

/***
* Function: begin(void)
* Description: Initializes BAGA board
* Params: none
* Returns: 0 if the initializing process succeeds, 1 if not
***/
byte BAGA::begin(void)
{
	byte result = 0;
	byte resultTemp = 0;
	
	pinMode(BAGA_LED_PIN, OUTPUT); // Configure LED pin as output
    pinMode(BAGA_3V_SHDN_PIN, OUTPUT); // Configure 3V shutdown pin as output
    pinMode(BAGA_LS_OE, OUTPUT); // Configure level shifter OE control pin as output

	digitalWrite(BAGA_LED_PIN, HIGH);   // Set the LED on	
	digitalWrite(BAGA_3V_SHDN_PIN, HIGH); // Enable 3V power supply
    digitalWrite(BAGA_LS_OE, LOW); // Enable level shifter OE

	Wire.begin(); // Configure I2C
	
	if (result == 0) // I2C correctly configured
	{
#ifdef DEBUG
		DebugSerial.print("Initializing BMP280.......");
#endif
		resultTemp = _sensorPressureTemperature.begin(); // Init pressure and temperature sensor
		result |= resultTemp;
#ifdef DEBUG
		if(resultTemp)
		{
			DebugSerial.println(strBagaFail);
		}
		else
		{
			DebugSerial.println(strBagaOk);
		}
#endif

#ifdef DEBUG
		DebugSerial.print("Initializing Si7020.......");
#endif
		resultTemp = _sensorHumidityTemperature.begin(); // Init humidity and temperature sensor
		result |= resultTemp;
#ifdef DEBUG
		if(resultTemp)
		{
			DebugSerial.println(strBagaFail);
		}
		else
		{
			DebugSerial.println(strBagaOk);
		}
#endif
		
#ifdef LIGHT_SENSOR_SI1132
#ifdef DEBUG
		DebugSerial.print("Initializing Si1132.......");
#endif
		resultTemp = _sensorLight.begin(); // Init UV sensor
		result |= resultTemp;
#ifdef DEBUG
		if(resultTemp)
		{
			DebugSerial.println(strBagaFail);
		}
		else
		{
			DebugSerial.println(strBagaOk);
		}
#endif
#endif
	}
	
	return result;
}


/***
* Function: readSensors(void)
* Description: Makes a single measurement from all sensors
*              and saves data in the private variables
* Params: none
* Returns: the measured temperature in Celsius degrees
***/
byte BAGA::readSensors(void)
{
	_sensorPressureTemperature.makeSingleMeasurement(); // BMP280 measurement
	
	_bagaRelativeHumidity = _sensorHumidityTemperature.readRelHumidity();
	_bagaTemperatureC = _sensorHumidityTemperature.readTemperaturePostHumidity();
	_bagaPressurePa = _sensorPressureTemperature.readPressure();
	_bagaPressuremBar = ((float)_bagaPressurePa) / 100;
}

/***
* Function: readTemperatureC(void)
* Description: Reads the temperature in Celsius degrees
*              from the previous readSensors() call
* Params: none
* Returns: the measured temperature in Celsius degrees
***/
float BAGA::readTemperatureC(void)
{
	return _bagaTemperatureC;
}


/***
* Function: readTemperatureF(void)
* Description: Reads the temperature in Fahrenheit degrees
*              from the previous readSensors() call
* Params: none
* Returns: the measured temperature in Fahrenheit degrees
***/
float BAGA::readTemperatureF(void)
{
	return BAGA_TEMPERATURE_C_TO_F(_bagaTemperatureC);
}


/***
* Function: readTemperatureK(void)
* Description: Reads the temperature in Kelvin degrees
*              from the previous readSensors() call
* Params: none
* Returns: the measured temperature in Kelvin degrees
***/
unsigned long BAGA::readTemperatureK(void)
{
	return BAGA_TEMPERATURE_C_TO_K(_bagaTemperatureC);
}


/***
* Function: readHeatIndexC(void)
* Description: Reads the heat index in Celsius degrees
*              from the previous readSensors() call
* Params: none
* Returns: the heat index ("feels like" temperature) in Celsius degrees
***/
float BAGA::readHeatIndexC(void)
{
	return BAGA_TEMPERATURE_F_TO_C(readHeatIndexF());
}


/***
* Function: readHeatIndexF(void)
* Description: Reads the heat index in Fahrenheit degrees
*              from the previous readSensors() call
* Params: none
* Returns: the heat index ("feels like" temperature) in Fahrenheit degrees
***/
float BAGA::readHeatIndexF(void)
{
	float heatIndex;
	float temperature;
	float relativeHumidity;
	
	temperature = readTemperatureF();
	relativeHumidity = readRelHumidity();
	
	heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (relativeHumidity * 0.094));
	
	if (heatIndex >= 80)
	{
		heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (relativeHumidity * 0.094));
		
		if (heatIndex >= 80)
		{
			heatIndex = -42.379 + (2.04901523 * temperature) + (10.14333127 * relativeHumidity) - (0.22475541 * temperature * relativeHumidity) - (0.00683783 * temperature * temperature) - (0.05481717 * relativeHumidity * relativeHumidity) + (0.00122874 * temperature * temperature * relativeHumidity) + (0.00085282 * temperature * relativeHumidity * relativeHumidity) - (0.00000199 * temperature * temperature * relativeHumidity * relativeHumidity);
		}
		
		if ((relativeHumidity < 13) && (temperature > 80) && (temperature < 112))
		{
			heatIndex = ((13 - relativeHumidity) / 4) * sqrt((17 - abs(temperature - 95)) / 17);
		}
		else if ((relativeHumidity > 85) && (temperature >= 80) && (temperature <= 87))
		{
			heatIndex = ((relativeHumidity - 85) / 10) * ((87 - temperature) / 5);
		}
	}
	
	return heatIndex;
}


/***
* Function: readHeatIndexK(void)
* Description: Reads the heat index in Kelvin degrees
*              from the previous readSensors() call
* Params: none
* Returns: the heat index ("feels like" temperature) in Kelvin degrees
***/
float BAGA::readHeatIndexK(void)
{
	return BAGA_TEMPERATURE_F_TO_K(readHeatIndexF());
}


/***
* Function: readAbsHumidity(void)
* Description: Reads the absolute humitidy in grams per cubic
*              meters from the previous readSensors() call
* Params: none
* Returns: The absolute humidity in grams per cubic meter
*/
float BAGA::readAbsHumidity(void)
{
	return 2.16679 * ((float)_bagaPressurePa) / BAGA_TEMPERATURE_C_TO_K(_bagaTemperatureC);
}


/***
* Function: readRelHumidity(void)
* Description: Reads the relative humidity from
*              the previous readSensors() call
* Params: none
* Returns: The relative humidity in grams per cubic meter
*/
float BAGA::readRelHumidity(void)
{
	return _bagaRelativeHumidity;
}


/***
* Function: readAbsPressuremBar(void)
* Description: Reads the absolute pressure in millibar
*              from the previous readSensors() call
* Params: none
* Returns: The absolute pressure in millibar
*/
float BAGA::readAbsPressuremBar(void)
{
	return _bagaPressuremBar;
}


/***
* Function: readAbsPressureBar(void)
* Description: Reads the absolute pressure in bar
*              from the previous readSensors() call
* Params: none
* Returns: The absolute pressure in bar
*/
float BAGA::readAbsPressureBar(void)
{
	return _bagaPressuremBar/1000;
}


/***
* Function: readAbsPressurePa(void)
* Description: Reads the absolute pressure in Pascal
*              from the previous readSensors() call
* Params: none
* Returns: The absolute pressure in Pa
*/
signed long BAGA::readAbsPressurePa(void)
{
	return _bagaPressurePa;
}


/***
* Function: readAbsPressureKPa(void)
* Description: Reads the absolute pressure in KiloPascal
*              from the previous readSensors() call
* Params: none
* Returns: The absolute pressure in KPa
*/
float BAGA::readAbsPressureKPa(void)
{
	return ((float)_bagaPressurePa)/1000;
}


/***
* Function: readAbsPressurePsi(void)
* Description: Reads the absolute pressure in PSI
*              from the previous readSensors() call
* Params: none
* Returns: The absolute pressure in PSI
*/
float BAGA::readAbsPressurePsi(void)
{
	return BAGA_PRESSURE_MBAR_TO_PSI(_bagaPressuremBar);
}


/***
* Function: readRelPressuremBar(void)
* Description: Reads the relative pressure in millibar
*              from the previous readSensors() call
* Params: none
* Returns: The relative pressure in millibar
*/
float BAGA::readRelPressuremBar(void)
{
	return _bagaPressuremBar - BAGA_SEA_LEVEL_PRESSURE_MBAR;
}


/***
* Function: readRelPressureBar(void)
* Description: Reads the relative pressure in bar
*              from the previous readSensors() call
* Params: none
* Returns: The relative pressure in bar
*/
float BAGA::readRelPressureBar(void)
{
	return (_bagaPressuremBar - BAGA_SEA_LEVEL_PRESSURE_MBAR)/1000;
}


/***
* Function: readRelPressurePa(void)
* Description: Reads the relative pressure in Pascal
*              from the previous readSensors() call
* Params: none
* Returns: The relative pressure in Pa
*/
signed long BAGA::readRelPressurePa(void)
{
	return _bagaPressurePa - BAGA_SEA_LEVEL_PRESSURE_PA;
}


/***
* Function: readRelPressureKPa(void)
* Description: Reads the relative pressure in KiloPascal
*              from the previous readSensors() call
* Params: none
* Returns: The relative pressure in KPa
*/
float BAGA::readRelPressureKPa(void)
{
	return ((float)(_bagaPressurePa - BAGA_SEA_LEVEL_PRESSURE_PA))/1000;
}


/***
* Function: readRelPressurePsi(void)
* Description: Reads the relative pressure in PSI
*              from the previous readSensors() call
* Params: none
* Returns: The relative pressure in PSI
*/
float BAGA::readRelPressurePsi(void)
{
	return BAGA_PRESSURE_MBAR_TO_PSI(_bagaPressuremBar - BAGA_SEA_LEVEL_PRESSURE_MBAR);
}

#ifdef LIGHT_SENSOR_SI1132

/***
* Function: readUVIndex(void)
* Description: Reads the UV Index
*              from the previous readSensors() call
* Params: none
* Returns: The UV Index value
*/
unsigned int BAGA::readUVIndex(void)
{
	return 0;
}


/***
* Function: readIRLight(void)
* Description: Reads the IR light intensity
*              from the previous readSensors() call
* Params: none
* Returns: The IR light intensity value
*/
unsigned long BAGA::readIRLight(void)
{
	return 0;
}


/***
* Function: readVisibleLight(void)
* Description: Reads the visible light intensity
*              from the previous readSensors() call
* Params: none
* Returns: The visible light intensity value
*/
unsigned int BAGA::readVisibleLight(void)
{
	return 0;
}

#endif


/***
* Function: readSolarRadiationAdc(void)
* Description: Reads the Solar radiation raw value
* Params: none
* Returns: The solar radiation raw ADC value (for debug purposes)
*/
int BAGA::readSolarRadiationAdc(void)
{
	return analogRead(BAGA_SOLAR_RADIATION_PIN);
}


/***
* Function: readSolarRadiation(void)
* Description: Reads the Solar radiation in Watt per square meter
* Params: none
* Returns: The solar radiation value
*/
unsigned long BAGA::readSolarRadiation(void)
{
	unsigned long voltage;
	
	voltage = (unsigned long)analogRead(BAGA_SOLAR_RADIATION_PIN);
	voltage = voltage * 45000 / 1023; // AVDD is aprox. 4500 mV
	voltage /= 25; // OpAmp gain is 2.5
	
	return voltage;
}


/***
* Function: readBatteryVoltage(void)
* Description: Reads the battery voltage in mV
* Params: none
* Returns: The batteries' voltage value in mV
*/
unsigned long BAGA::readBatteryVoltage(void)
{
	unsigned long voltage;
	
	voltage = (unsigned long)analogRead(BAGA_BATTERY_VOLTAGE_PIN);
	voltage = voltage * 4500 / 1023; // AVDD is aprox. 4500 mV
	
	return  voltage;
}


/***
* Function: setLedOn(void)
* Description: Turns the LED on
* Params: none
* Returns: none
*/
void BAGA::setLedOn(void)
{
	digitalWrite(BAGA_LED_PIN, HIGH); // Turn LED on
}


/***
* Function: setLedOff(void)
* Description: Turns the LED off
* Params: none
* Returns: none
*/
void BAGA::setLedOff(void)
{
	digitalWrite(BAGA_LED_PIN, LOW); // Turn LED off
}

/***
* Function: blink(void)
* Description: Blink the LED
* Params: none
* Returns: none
*/
void BAGA::blink(int times)
{
	int i = 0;
	for (i = 0; i <times; i++)
	{
		digitalWrite(BAGA_LED_PIN, HIGH); // Turn LED on
		delay(200);
		digitalWrite(BAGA_LED_PIN, LOW); // Turn LED off
		delay(200);
	}
}

/***
* Function: blink(void)
* Description: Blink the LED
* Params: none
* Returns: none
*/
void BAGA::blinkForever(int times)
{
	int i = 0;
	while(1)
	{
		for (i = 0; i <times; i++)
		{
			digitalWrite(BAGA_LED_PIN, HIGH); // Turn LED on
			delay(200);
			digitalWrite(BAGA_LED_PIN, LOW); // Turn LED off
			delay(200);
		}
		delay(1000);
	}
}

/***
* Function: sleep(unsigned long periodMs, adc_t acdState)
* Description: Enter sleep mode (power down)
* Params: periodMs - period in milliseconds
*         acdState - ADC state on sleep mode
* Returns: none
*/
void BAGA::sleep(unsigned long periodMs, adc_t acdState)
{
	period_t sleepPeriod;
	
//#ifdef LowPower_h

	do{
		if (periodMs > 8000)
		{
			sleepPeriod = SLEEP_8S;
			periodMs -= 8000;
		}
		else if (periodMs > 4000)
		{
			sleepPeriod = SLEEP_4S;
			periodMs -= 4000;
		}
		else if (periodMs > 2000)
		{
			sleepPeriod = SLEEP_2S;
			periodMs -= 2000;
		}
		else if (periodMs > 1000)
		{
			sleepPeriod = SLEEP_1S;
			periodMs -= 1000;
		}
		else if (periodMs > 500)
		{
			sleepPeriod = SLEEP_500MS;
			periodMs -= 500;
		}
		else if (periodMs > 250)
		{
			sleepPeriod = SLEEP_250MS;
			periodMs -= 250;
		}
		else if (periodMs > 120)
		{
			sleepPeriod = SLEEP_120MS;
			periodMs -= 120;
		}
		else if (periodMs > 60)
		{
			sleepPeriod = SLEEP_60MS;
			periodMs -= 60;
		}
		else if (periodMs > 30)
		{
			sleepPeriod = SLEEP_30MS;
			periodMs -= 30;
		}
		else if (periodMs > 15)
		{
			sleepPeriod = SLEEP_15MS;
			periodMs -= 15;
		}
		else
		{
			periodMs = 0;
			break; // Delays below 15ms are ignored
		}
		
		LowPower.powerDown(sleepPeriod, acdState, BOD_OFF);
		

	} while(periodMs > 0);

//#endif	
}
