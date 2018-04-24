/*
* File:    Si7020.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/

/* Include Files */
#include "Si7020.h"


Si7020::Si7020() // Constructor
{
	_isEnabled = false;
	_rawRelativeHumidity = 0;
	_rawTemperaturePostHumidity = 0;
	_rawTemperature = 0;
	_heaterLevel = 0;	
}


/***
* Function: begin(void)
* Description: Initializes Si7020 device
* Params: none
* Returns: 0 if the initializing process succeeds, 1 if not
***/
byte Si7020::begin(void)
{
	Wire.begin(); // Configure I2C
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
    if (Wire.endTransmission() == 0)
	{
        _isEnabled = true;
    }
	
	return 0;
}

/***
* Function: isEnabled(void)
* Description: Check if the device is initialized
* Params: none
* Returns: true if the device is initialized, false if not
***/
bool Si7020::isEnabled(void)
{
	return _isEnabled; // Return state
}


/***
* Function: reset(void)
* Description: Reset device to its POR state
* Params: none
* Returns: true if the device was reset, false if not
***/
byte Si7020::reset(void)
{
	byte result = 0;
	
	result = writeByte(SI7020_CMD_WRITE_RHT_USER_REG, SI7020_CMD_RESET); //Reset user resister
	
	return result;
}


/***
* Function: readRelHumidity(void)
* Description: Read relative humidity
* Params: none
* Returns: Relative humidity value
***/
float Si7020::readRelHumidity(void)
{
	byte result = 0;
	
	// Measure the relative humidity 
	result = readUInt(SI7020_CMD_MEASURE_RH_HOLD, &_rawRelativeHumidity);
	
	if (result == 0)
	{
		_rawRelativeHumidity &= 0xFFFC; // Measured value is always 0bxxxxxxxxxxxxxx10
		return (125.0 * _rawRelativeHumidity / 65536) - 6;
	}

	return 0;
}


/***
* Function: readTemperature(void)
* Description: Read temperature in Celsius degrees
* Params: none
* Returns: Temperature value
***/
float Si7020::readTemperature(void)
{
	byte result = 0;
	
	// Measure the temeprature
	result = readUInt(SI7020_CMD_MEASURE_TEMP_HOLD, &_rawTemperature);
	
	if (result == 0)
	{
		_rawTemperature &= 0xFFFC; // Measured value is always 0bxxxxxxxxxxxxxx10
		return (175.25 * _rawTemperature / 65536) - 46.85;
	}
	
	return 0;
}


/***
* Function: readTemperaturePostHumidity(void)
* Description: Read temperature from the previous humidity measurement
* Params: none
* Returns: Temperature value
***/
float Si7020::readTemperaturePostHumidity(void)
{
	byte result = 0;
	
	// Measure the temperature from the previous humitidy measurement
	result = readUInt(SI7020_CMD_MEASURE_TEMP_PREV_RH, &_rawTemperaturePostHumidity);
	
	if (result == 0)
	{
		_rawTemperaturePostHumidity &= 0xFFFC; // Measured value is always 0bxxxxxxxxxxxxxx10
		return (175.25 * _rawTemperaturePostHumidity / 65536) - 46.85;
	}
	
	return 0;
}


/***
* Function: setHeaterOff(void)
* Description: Turn heater off
* Params: none
* Returns: 0 if state was successfully written, 1 if not
***/
byte Si7020::setHeaterOff(void)
{
	return setHeaterState(SI7020_HEATER_OFF);
}


/***
* Function: setHeaterOn(void)
* Description: Turn heater on
* Params: none
* Returns: 0 if state was successfully written, 1 if not
***/
byte Si7020::setHeaterOn(void)
{
	return setHeaterState(SI7020_HEATER_ON);
}


/***
* Function: setHeaterLevel(byte level)
* Description: Set heater level
* Params: level - Heater level
* Returns: 0 if level was successfully written, 1 if not
***/
byte Si7020::setHeaterLevel(byte level)
{
	byte result;
	byte regValue;
	
	result = readByte(SI7020_CMD_READ_HEATER_REG, &regValue);
	
	if (result == 0)
	{
		regValue &= ~(SI7020_HEATER_LEVEL_MASK);
		regValue |= (level & SI7020_HEATER_LEVEL_MASK);
		
		result = writeByte(SI7020_CMD_WRITE_HEATER_REG, regValue);
		
		if (result == 0)
		{
			_heaterLevel = level & SI7020_HEATER_LEVEL_MASK;
		}
	}
	return result;
}


/***
* Function: setResolution(byte resolution)
* Description: Set measurement resolution
* Params: none
* Returns: 0 if resolution setting was successfully written, 1 if not
***/
byte Si7020::setResolution(byte resolution)
{
	byte result = 0;
	byte regValue = 0;
	
	result = readByte(SI7020_CMD_WRITE_RHT_USER_REG, &regValue);
	
	if (result == 0)
	{
		regValue &= (~SI7020_RES_MASK); // Clear resolution bits
		regValue |= (resolution & SI7020_RES_MASK); // Set new resolution bits
		
		result = writeByte(SI7020_CMD_WRITE_RHT_USER_REG, regValue); // Write new value to register
	}
	
	return result;
}


/***
* Function: setHeaterState(byte state)
* Description: Set heater state
* Params: state - Heater state setting
* Returns: 0 if state was successfully written, 1 if not
***/
byte Si7020::setHeaterState(byte state)
{
	byte result = 0;
	byte regValue = 0;
	
	result = readByte(SI7020_CMD_WRITE_RHT_USER_REG, &regValue);
	
	if (result == 0)
	{
		regValue &= (~SI7020_HEATER_MASK); // Clear resolution bits
		regValue |= (state & SI7020_HEATER_MASK); // Set new resolution bits
		
		result = writeByte(SI7020_CMD_WRITE_RHT_USER_REG, regValue); // Write new value to register
	}
	
	return result;
}


// I2C R/W functions

/***
* Function: readInt(byte address, signed short *value)
* Description: Read 16-bit signed integer from device
* Params: address - Register address
*         value - Variable to save the value in
* Returns: 0 if the value was read successfully, 1 if not
***/
byte Si7020::readInt(byte address, signed short *value)
{
	byte result;
	byte byteCount = 0;
	byte data[2] = { 0, 0};
	
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if(readByteArray(address,data,2) == 0)
	{
		*value = (((unsigned short) data[0]) << 8) | data[1]; 
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
byte Si7020::readUInt(byte address, unsigned short *value)
{
	byte result;
	byte byteCount = 0;
	byte data[2] = { 0, 0};
	
	if(readByteArray(address,data,2) == 0)
	{
		*value = (((unsigned short) data[0]) << 8) | data[1]; 
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
byte Si7020::readByte(byte address, byte *value)
{
	byte result;
	
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if (result == 0)
	{
		Wire.requestFrom(SI7020_I2C_ADDRESS,1);
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
byte Si7020::readByteArray(byte address, unsigned char * array, byte length)
{
	byte result;
	byte byteCount = 0;
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
	Wire.write(address);
	result = Wire.endTransmission();
	
	if (result == 0)
	{
		byte i;
		
		Wire.requestFrom(SI7020_I2C_ADDRESS,(int)length);
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
byte Si7020::writeByte(byte address, byte value)
{
	byte result;
	
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
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
byte Si7020::writeByteArray(byte address, unsigned char * array, byte length)
{
	byte result;
	
	Wire.beginTransmission(SI7020_I2C_ADDRESS);
	Wire.write(address);
	Wire.write(array, length);
	result = Wire.endTransmission();

	return result;
}