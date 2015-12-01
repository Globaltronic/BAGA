/*
* File:    BMP280.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/

/* Include Files */
#include "BMP280.h"

BMP280::BMP280() // Constructor
{
	_isEnabled = false;
	_dataUpdated = false;
	_mode = BMP280_MODE_SLEEP; // POR mode value 
	_pressureOversampling = BMP280_PRESS_OVERSAMPLING_1; // POR pressure oversampling value 
	_temperatureOversampling = BMP280_TEMP_OVERSAMPLING_1; // POR temperature oversampling value 
	_rawTemperature = 0;
	_rawPressure = 0;
	_tFine = 0;
}


/***
* Function: begin(void)
* Description: Initializes BMP280 device
* Params: none
* Returns: 0 if the initializing process succeeds, 1 if not
***/
byte BMP280::begin(void)
{
	byte result = 0;
	
	Wire.begin(); // Configure I2C
		
	setMode(BMP280_MODE_FORCED); // Disable pressure oversampling
	setPressureOversampling(BMP280_PRESS_OVERSAMPLING_1); // Disable pressure oversampling
	setTempOversampling(BMP280_TEMP_OVERSAMPLING_1); // Disable temperature oversampling
	
	result |= loadCalibrationData(); // Load factory calibration values
	
	if (result == 0)
	{
		_isEnabled = true;
	}
	
	return result;
}


/***
* Function: isEnabled(void)
* Description: Check if the device is initialized
* Params: none
* Returns: true if the device is initialized, false if not
***/
bool BMP280::isEnabled(void)
{
	return _isEnabled; // Return state
}


/***
* Function: reset(void)
* Description: Reset device to its POR state
* Params: none
* Returns: true if the device was reset, false if not
***/
byte BMP280:: reset()
{
	byte result = 0;
	
	result = writeByte(BMP280_REG_RESET, BMP280_RESET);
	
	if (result == 0)
	{
		_isEnabled = false;
	}
	return result;
}
	

/***
* Function: makeSingleMeasurement(void)
* Description: Trigger the device to make a single measurement of temperature and pressure
* Params: none
* Returns: 0 if measurement was successful, 1 if not
***/
byte BMP280::makeSingleMeasurement(void)
{
	byte result = 1;
	byte settings = _mode | (_pressureOversampling << 2) | (_temperatureOversampling << 5);
	signed long var1, var2;
	unsigned long time;
	unsigned char rawData[6];
	
	result = writeByte(BMP280_REG_CTRL_MEASURE, settings);
	
	time = millis(); // Timestamp
	
	if (result == 0) // Byte successfully written
	{
		result = 1;
		do
		{
			if (getMode() != BMP280_MODE_SLEEP) // Check measurement done bit
			{
				result = 0;
				break;
			}
		} while((millis() - time) < BMP280_MEASUREMENTS_TIMEOUT_MS); // Wait for the timeout to expire
			
		//if (isMeasurementDone() == true)
		if (result == 0)
		{
			result = readByteArray(BMP280_REG_PRESS_MSB, rawData, 6); // Read raw data
					
			if (result == 0)
			{
				_dataUpdated = true;
				_rawPressure = (((unsigned long) rawData[0]) << 12) + (((unsigned long) rawData[1]) << 4) + (((unsigned long) rawData[2]) >> 4);
				_rawTemperature = (((unsigned long) rawData[3]) << 12) + (((unsigned long) rawData[4]) << 4) + (((unsigned long) rawData[5]) >> 4);
				
					
				var1 = ((((_rawTemperature >> 3) - ((signed long) _calibT1 << 1))) * ((signed long) _calibT2)) >> 11;
				var2 = (((((_rawTemperature >> 4) - ((signed long) _calibT1)) * ((_rawTemperature >> 4) - ((signed long) _calibT1))) >> 12) * ((signed long) _calibT3)) >> 14;
				_tFine = var1 + var2;
			}
			else
			{
				Serial1.println("[ERROR]");
			}
		}
		else
		{
			Serial1.println("[TIMEOUT]");
		}
	}

	return result;
}


/***
* Function: readTemperature(void)
* Description: Read temperature from last measurement
* Params: none
* Returns: Temperature value in Celsius degrees
***/
float BMP280::readTemperature(void)
{
	signed long temperatureVal;

    temperatureVal = (_tFine * 5 + 128) >> 8;

	_temperature = ((float)temperatureVal)/100;
	
	return _temperature;
}


/***
* Function: readPressure(void)
* Description: Read pressure from last measurement
* Params: none
* Returns: Pressure value in Pascal
***/
signed long BMP280::readPressure(void)
{
	signed long pressureVal;
	signed long var1, var2;
	
	if (_pressureOversampling == BMP280_PRESS_OVERSAMPLING_OFF)
	{
#ifdef DEBUG
			Serial1.println("Pressure sampling disabled");
#endif
		return 0;
	}

	var1 = (((signed long) _tFine) >> 1) - (signed long) 64000;
	var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((signed long) _calibP6);
	var2 = var2 + ((var1 * ((signed long) _calibP5)) << 1);
	var2 = (var2 >> 2)+(((signed long) _calibP4) << 16);
	var1 = (((_calibP3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((signed long) _calibP2) * var1) >> 1)) >> 18;
	var1 = ((((32768 + var1))*((signed long) _calibP1)) >> 15);
	if (var1 == 0)
	{
		return 0; // Avoid exception caused by division by zero
	}
	pressureVal = (((unsigned long) (((signed long) 1048576) - _rawPressure)-(var2 >> 12)))*3125;
	if (pressureVal < 0x80000000)
	{
		pressureVal = (pressureVal << 1) / ((unsigned long) var1);
	}
	else
	{
		pressureVal = (pressureVal / (unsigned long) var1) * 2;
	}
	var1 = (((signed long) _calibP9) * ((signed long) (((pressureVal >> 3) * (pressureVal >> 3)) >> 13))) >> 12;
	var2 = (((signed long) (pressureVal >> 2)) * ((signed long) _calibP8)) >> 13;
	pressureVal = (unsigned long) ((signed long) pressureVal + ((var1 + var2 + _calibP7) >> 4));

	_pressure = pressureVal;
	
	return _pressure; // Return pressure value in Pa
}

/***
* Function: calcAltitude(void)
* Description: Calculate altitude for a given pressure and temperature
* Params: absPressurePa - Absolute pressure in mbar
*         seaLevelPressurePa - Sea level pressure in mbar
*         temperatureC - Temperature in Celsius degrees
* Returns: Altitude value in meters
***/
float BMP280::calcAltitude(float absPressureMBar, float seaLevelPressureMBar, float temperatureC)
{
	//return(44330.0 * (1 - pow(((float)absPressurePa/100)/seaLevelPressurePa, 1/5.255)));
	//return (1 - pow((((float)absPressurePa)/100)/seaLevelPressurePa,0.190284)) * 44307.69396;
	return (((pow((seaLevelPressureMBar/absPressureMBar),(1.0/5.257)) - 1)*(temperatureC + 273.15))/0.0065);
}


/***
* Function: isMeasurementDone(void)
* Description: Check if the measurement is completed (not valid for Forced Mode)
* Params: none
* Returns: true if measurement is completed, false if not
***/
bool BMP280::isMeasurementDone(void)
{
	byte result = 0;
	byte status;

	result = readByte(BMP280_REG_STATUS, &status); // Read status register
	
	if (result == 0)
	{
		Serial1.print("Status:");
		Serial1.println(status);
		if((status & BMP280_MEASURING_BIT_MASK) == BMP280_MEASURING_DONE) // Check measurement bit
		{
			return true; // Measurement completed
		}
	}
	
	return false; // Measurement not completed
}


/***
* Function: isNVMDataReady(void)
* Description: Check if the NVM data was copied to image registers
* Params: none
* Returns: true if data was copied, false if not
***/
bool BMP280::isNVMDataReady(void)
{
	byte result = 0;
	byte status;

	result = readByte(BMP280_REG_STATUS, &status); // Read status register
	
	if (result == 0)
	{
		if((status & BMP280_NVM_UPDATE_BIT_MASK) == BMP280_NVM_UPDATE_DONE) // Check measurement bit
		{
			return true; // NVM data ready
		}
	}
	
	return false; // NVM data not ready
}

/***
* Function: loadCalibrationData(void)
* Description: Load factory calibration data
* Params: none
* Returns: 0 if data was loaded successfully, 1 if not
***/
byte BMP280::loadCalibrationData(void)
{
	byte result = 0;
	
	result |= readUInt(BMP280_REG_CALIB_00, &_calibT1);
	result |= readInt(BMP280_REG_CALIB_02, &_calibT2);
	result |= readInt(BMP280_REG_CALIB_04, &_calibT3);

	result |= readUInt(BMP280_REG_CALIB_06, &_calibP1);
	result |= readInt(BMP280_REG_CALIB_08, &_calibP2);
	result |= readInt(BMP280_REG_CALIB_10, &_calibP3);
	result |= readInt(BMP280_REG_CALIB_12, &_calibP4);
	result |= readInt(BMP280_REG_CALIB_14, &_calibP5);
	result |= readInt(BMP280_REG_CALIB_16, &_calibP6);
	result |= readInt(BMP280_REG_CALIB_18, &_calibP7);
	result |= readInt(BMP280_REG_CALIB_20, &_calibP8);
	result |= readInt(BMP280_REG_CALIB_22, &_calibP9);

	return result;
}


/***
* Function: getMode(void)
* Description: Get current device mode value
* Params: none
* Returns: Device mode value
***/
byte BMP280::getMode(void)
{
	byte currentModeValue = 0;
	
	readByte(BMP280_REG_CTRL_MEASURE, &currentModeValue); // Read value from register

	return currentModeValue & BMP280_MODE_BYTE_MASK;
}


/***
* Function: setMode(void)
* Description: Set device mode
* Params: New Mode setting
* Returns: none
***/
void BMP280::setMode(byte mode)
{
	_mode = mode;
}

/***
* Function: setTempOversampling(byte tempOversampling)
* Description: Set device temperature oversampling setting
* Params: New temperature oversampling setting
* Returns: none
***/
void BMP280::setTempOversampling(byte tempOversampling)
{
	_temperatureOversampling = tempOversampling;

}

/***
* Function: setPressureOversampling(byte pressureOversampling)
* Description: Set device pressure oversampling setting
* Params: New temperature oversampling setting
* Returns: none
***/
void BMP280::setPressureOversampling(byte pressureOversampling)
{
	_pressureOversampling = pressureOversampling;
}


// I2C R/W functions

/***
* Function: readInt(byte address, signed short *value)
* Description: Read 16-bit signed integer from device
* Params: address - Register address
*         value - Variable to save the value in
* Returns: 0 if the value was read successfully, 1 if not
***/
byte BMP280::readInt(byte address, signed short *value)
{
	byte result;
	byte byteCount = 0;
	byte data[2] = { 0, 0};
	
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if(readByteArray(address,data,2) == 0)
	{
		*value = ((unsigned short) data[1]) << 8;
		*value |= data[0];
	}
		
	return result;
}


/***
* Function: readUInt(byte address, unsigned short *value)
* Description: Read 16-bit unsigned integer from device
* Params: address - Register address
*         value - Variable to save the value in
* Returns: 0 if the value was read successfully, 1 if not
***/
byte BMP280::readUInt(unsigned char address, unsigned short *value)
{
	byte result;
	byte byteCount = 0;
	byte data[2] = { 0, 0};
	
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if(readByteArray(address,data,2) == 0)
	{
		*value = ((unsigned short) data[1]) << 8;
		*value |= data[0];
	}
		
	return result;
}


/***
* Function: readByte(unsigned char address, unsigned char *value)
* Description: Read byte from device
* Params: address - Register address
*         value - Variable to save the value in
* Returns: 0 if the value was read successfully, 1 if not
***/
byte BMP280::readByte(unsigned char address, unsigned char *value)
{
	byte result;
	
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if (result == 0)
	{
		Wire.requestFrom(BMP280_I2C_ADDRESS,1);
		while(!Wire.available()) ; // wait until bytes are ready
		while(Wire.available()) // Wait until bytes are ready to read
		{
			*value = Wire.read(); // Read byte
		}
	}
		
	return result;
}

/***
* Function: readByteArray(unsigned char address, unsigned char * array, unsigned char length)
* Description: Read byte array from device
* Params: address - Register address
*         array - Variable to save the values in
*         length - Number of bytes to read
* Returns: 0 if the values were read successfully, 1 if not
***/
byte BMP280::readByteArray(unsigned char address, unsigned char * array, unsigned char length)
{
	byte result;
	byte byteCount = 0;
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if (result == 0)
	{
		Wire.requestFrom(BMP280_I2C_ADDRESS,(int)length);
		while(Wire.available() != length) ; // wait until bytes are ready
		while(Wire.available()) // Wait until bytes are ready to read
		{
			array[byteCount++] = Wire.read(); // Read byte
		}
	}
		
	return result;
}


/***
* Function: writeByte(unsigned char address, unsigned char value)
* Description: Write byte to device
* Params: address - Register address
*         value - Value to write
* Returns: 0 if the value was written successfully, 1 if not
***/
byte BMP280::writeByte(unsigned char address, unsigned char value)
{
	byte result;
	
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	Wire.write(value);
	result = Wire.endTransmission();

	return result;
}


/***
* Function: writeByteArray(unsigned char address, unsigned char * array, unsigned char length)
* Description: Write byte array to device
* Params: address - Register address
*         value - Value to write
* Returns: 0 if the value was written successfully, 1 if not
***/
byte BMP280::writeByteArray(unsigned char address, unsigned char * array, unsigned char length)
{
	byte result;
	
	Wire.beginTransmission(BMP280_I2C_ADDRESS);
	Wire.write(address);
	Wire.write(array, length);
	result = Wire.endTransmission();

	return result;
}

