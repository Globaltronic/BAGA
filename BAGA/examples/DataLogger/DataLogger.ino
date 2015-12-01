#include <Wire.h>
#include <BMP280.h>
#include <Si7020.h>
#include <BAGA.h>
#include <SPI.h>
#include <SdFat.h>

/***
* BAGA A1
* Data Logger Example
*
* This example reads data periodically and saves it to the MicroSD Card
*
* HW Compatibility: A2
***/

// Select the desired debug serial port:
// * Serial (USB-CDC)
// * Serial1 (J3 connector on board)

#define DebugSerial  	Serial


BAGA baga; // BAGA instance
SdFat sd;
SdFile logFile;

char logFileName[] = "log.csv";
unsigned long ID = 0; // Measurements counter


// Strings

char strOpeningSd[27] = "Opening log file..........";
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";
char strDataString[] = "ID,TmpC,TmpF,TmpK,HtIdxC,HtIdxF,HtIdxK,AbH,RH,AbPrmBar,AbPrPa,AbPrPSI,SunRd,BatV";

int updateIntervalMilliseconds = 5000; // Read data every 5 seconds

void setup() {
	// put your setup code here, to run once:
	byte result;
	
	DebugSerial.begin(9600); 

	result = baga.begin();
	if (result)
	{
		DebugSerial.println(strFail);
		while(true);
	}
	else
	{
		DebugSerial.println(strOk);
	}

	if (!sd.begin(BAGA_SDCARD_CS_PIN, SPI_HALF_SPEED)) // Initialize SD card
	{
		sd.initErrorHalt();
	}
	
	if (!logFile.open(logFileName, O_RDWR | O_CREAT | O_AT_END)) // Open file
	{
		sd.errorHalt("Error opening file");
	}

	logFile.println(strDataString); // Print to file		
	DebugSerial.println(strDataString); // Print to the serial port
	logFile.close();


}

void loop() {
	// put your main code here, to run repeatedly:
		
	baga.setLedOn(); // Turn LED on to indicate that a measurement is in progress
	baga.readSensors(); // This method reads all sensors and saves data in the variables
	baga.setLedOff(); // Turn LED off
	
	if (!logFile.open(logFileName, O_RDWR | O_CREAT | O_AT_END))
	{
		DebugSerial.println("errorHalt");
		sd.errorHalt("Error opening file");
		baga.setLedOn(); // Turn LED on
	}
	else
	{
		String dataString = "";
	
		dataString += String(ID);
		dataString += ",";
		dataString += String(baga.readTemperatureC());
		dataString += ",";
		dataString += String(baga.readTemperatureF());
		dataString += ",";
		dataString += String(baga.readTemperatureK());
		dataString += ",";
		
		dataString += String(baga.readHeatIndexC());
		dataString += ",";
		dataString += String(baga.readHeatIndexF());
		dataString += ",";
		dataString += String(baga.readHeatIndexK());
		dataString += ",";
		
		dataString += String(baga.readAbsHumidity());
		dataString += ",";
		dataString += String(baga.readRelHumidity());
		dataString += ",";
		
		dataString += String(baga.readAbsPressuremBar());
		dataString += ",";
		dataString += String(baga.readAbsPressurePa());
		dataString += ",";
		dataString += String(baga.readAbsPressurePsi());
		dataString += ",";
				
		dataString += String(baga.readSolarRadiation());
		dataString += ",";
		
		dataString += String(baga.readBatteryVoltage());
		
		ID++;
		
		logFile.println(dataString);// Print to file
		logFile.close(); // Close file

		DebugSerial.println(dataString); // Print to the serial port	
	}
  
	delay(updateIntervalMilliseconds);
}
