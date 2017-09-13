#include <Wire.h>
#include <Si7020.h>

// Select the desired debug serial port:
// * Serial (USB-CDC)
// * Serial1 (J3 connector on board)

#define DebugSerial  	Serial



/***
* Si7020 A1
* Read Data Example
*
* This example reads relative humidity and temperature data
* from Si7020 sensor periodically and prints it to
* the serial port
*
* HW Compatibility: A2
***/

Si7020 sensor; // Si7020 instance


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
	DebugSerial.print("Initializing Si7020.......");
  
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
	float rh;
	float temperature;

	
	rh = sensor.readRelHumidity();
	temperature = sensor.readTemperaturePostHumidity();
	
	DebugSerial.print("RH = \t");
	DebugSerial.print(rh,2);
	DebugSerial.print(" %\t");
	DebugSerial.print("T = \t");
	DebugSerial.print(temperature,2);
	DebugSerial.println(" degC\t");
	
	delay(updateIntervalMilliseconds);
}
