#include <Wire.h>
#include <BMP280.h>

// Select the desired debug serial port:
// * Serial (USB-CDC)
// * Serial1 (J3 connector on board)

#define DebugSerial  	Serial



/***
* BMP280 A1
* Read Data Example
*
* This example reads pressure and temperature data
* from BMP280 sensor periodically and prints it to
* the serial port
*
* HW Compatibility: A2
***/

BMP280 sensor; // BMP280 instance


// Strings
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";

int updateIntervalMilliseconds = 1000; // Read data every 5 seconds

void setup() {
	// put your setup code here, to run once:
	byte result;

	DebugSerial.begin(9600); // Initialize serial port
	while (!DebugSerial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}


	result = sensor.begin();
	DebugSerial.print("Initializing BMP280.......");
  
	if (result)
	{
		DebugSerial.println(strFail);
	}
	else
	{
		DebugSerial.println(strOk);
	}
}

void loop() {
	// put your main code here, to run repeatedly:
	float pressure;
	float temperature;

	
	if (sensor.makeSingleMeasurement() == 0)
	{		
		pressure = ((float)sensor.readPressure())/100;
		temperature = sensor.readTemperature();
		
		DebugSerial.print("T = \t");
		DebugSerial.print(temperature,2);
		DebugSerial.print(" degC\t");
		DebugSerial.print("P = \t");
		DebugSerial.print(pressure,2);
		DebugSerial.println(" mBar");
	}
	delay(updateIntervalMilliseconds);
}
