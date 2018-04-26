#include <Wire.h>
#include <BMP280.h>
#include <Si7020.h>
#include <BAGA.h>

/************************************************************************
* BAGA A3
* Data Display Example
*
* This example reads data periodically and prints it to the serial port.
*
* HW Compatibility:
* 	Schematic: A2
*   PCB: A3/A4
*************************************************************************/

/**********************
 * BAGA Configuration *
 **********************/

/* Sensor reading periodicity in seconds */
unsigned long ReadIntervalS = 5;

/* Sensor reading periodicity in seconds */
unsigned long SerialBaudRate = 115200;

/* Select the desired debug serial port */
/* Serial (USB-CDC) or Serial1 (J3 connector on board) */
#define DebugSerial  	Serial


/*************
 * Variables *
 *************/

/* Classes objects */
BAGA baga; // BAGA instance

/* Strings */
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";

/*********
 * SETUP *
 *********/

void setup() {
	// put your setup code here, to run once:
	byte result;

	DebugSerial.begin(SerialBaudRate); // Initialize serial port
	while (!DebugSerial)
	{
		; // wait for serial port to connect. Needed for Leonardo only
	}

	DebugSerial.println("###########   BAGA   ###########");
	DebugSerial.print("Initializing BAGA.........");
  
	if (baga.begin())
	{
		DebugSerial.println(strFail);
	}
	else
	{
		DebugSerial.println(strOk);
	}
}

/*************
 * MAIN LOOP *
 *************/

void loop() {
	// put your main code here, to run repeatedly:
	
	DebugSerial.println("################################");
	
	baga.setLedOn(); // Turn LED on to indicate that a measurement is in progress
	baga.readSensors(); // This method reads all sensors and saves data in the variables
	baga.setLedOff(); // Turn LED off
	
	DebugSerial.print("Timestamp: ");
	DebugSerial.println(millis());
	
	DebugSerial.print("Temperature (C):        ");
	DebugSerial.println(baga.readTemperatureC());
	DebugSerial.print("Temperature (F):        ");
	DebugSerial.println(baga.readTemperatureF());
	DebugSerial.print("Temperature (K):        ");
	DebugSerial.println(baga.readTemperatureK());
		
	DebugSerial.println("--------------------------------");
	
	DebugSerial.print("Heat Index (C):         ");
	DebugSerial.println(baga.readHeatIndexC());
	DebugSerial.print("Heat Index (F):         ");
	DebugSerial.println(baga.readHeatIndexF());
	DebugSerial.print("Heat Index (K):         ");
	DebugSerial.println(baga.readHeatIndexK());
	
	DebugSerial.println("--------------------------------");
  
	DebugSerial.print("Abs Humidity (g/m3):    ");
	DebugSerial.println(baga.readAbsHumidity());
	DebugSerial.print("Rel Humidity (%):       ");
	DebugSerial.println(baga.readRelHumidity());

	DebugSerial.println("--------------------------------");
	
	DebugSerial.print("Abs Pressure (mBar):    ");
	DebugSerial.println(baga.readAbsPressuremBar());
	DebugSerial.print("Abs Pressure (Bar):     ");
	DebugSerial.println(baga.readAbsPressureBar());
	DebugSerial.print("Abs Pressure (Pa):      ");
	DebugSerial.println(baga.readAbsPressurePa());
	DebugSerial.print("Abs Pressure (KPa):     ");
	DebugSerial.println(baga.readAbsPressureKPa());
	
	DebugSerial.println("--------------------------------");
	
	DebugSerial.print("Rel Pressure (mBar):    ");
	DebugSerial.println(baga.readRelPressuremBar());
	DebugSerial.print("Rel Pressure (Bar):     ");
	DebugSerial.println(baga.readRelPressureBar());
	DebugSerial.print("Rel Pressure (Pa):      ");
	DebugSerial.println(baga.readRelPressurePa());
	DebugSerial.print("Rel Pressure (KPa):     ");
	DebugSerial.println(baga.readRelPressureKPa());
	
	DebugSerial.println("--------------------------------");
	
	DebugSerial.print("Solar Radiation:        ");
	DebugSerial.println(baga.readSolarRadiationAdc()); // Debug
	DebugSerial.print("Solar Radiation(W/m2):  ");
	DebugSerial.println(baga.readSolarRadiation());
  
	DebugSerial.println("--------------------------------");
	
  	DebugSerial.print("Battery Voltage (mV):   ");
	DebugSerial.println(baga.readBatteryVoltage());
  
	DebugSerial.println("________________________________");
  
	delay(ReadIntervalS * 1000);
}
