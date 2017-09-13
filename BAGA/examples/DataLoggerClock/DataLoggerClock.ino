#include <Wire.h>
#include <BMP280.h>
#include <Si7020.h>
#include <BAGA.h>
#include <SPI.h>
#include <SdFat.h>
#include <SWClock.h>

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
SdFile configFile;
SWClock clock;
char configString[21];
char clockString[21];
unsigned long timestamp;

char configFilename[] = "config.txt";
char logFileName[] = "log.csv";
unsigned long ID = 0; // Measurements counter


// Strings

char strOpeningSd[27] = "Opening log file..........";
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";
char strDataString[] = "Timestamp,Temp_C,Temp_Apar_C,Hum_Abs,Hum_Rel,Press_Abs_mBar,Rad_Sol,Vol_Bat";

int updateIntervalMilliseconds = 5000; // Read data every 5 min

void setup() {
	// put your setup code here, to run once:
	byte result;
	
	DebugSerial.begin(9600); 
	
	//while(!Serial);

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
	
	if (!configFile.open(configFilename, O_READ)) // Open file
	{
		//sd.errorHalt("Error opening config file");
	}
	else
	{
		int data;
		int count = 0;
		
		while(((data = configFile.read()) >= 0) && (count < 21))
		{
			configString[count++] = data;
		}
		
		configFile.close();
		//Serial.println(configString);
		clock.begin(configString);
		timestamp = millis();
	}
	
	if (!logFile.open(logFileName, O_RDWR | O_CREAT | O_AT_END)) // Open file
	{
		sd.errorHalt("Error opening file");
	}

	logFile.println(strDataString); // Print to file		
	DebugSerial.println(strDataString); // Print to the serial port
	logFile.close();
	
	baga.setLedOn(); // Turn LED on to indicate that a measurement is in progress
	delay(2000);
	baga.setLedOff(); // Turn LED off
	
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
	
		if(clock.isInitialized() == true)
		{
			clock.addTime(0,0,0,0,0,(int)((millis()-timestamp)/1000));
			timestamp = millis();
			clock.getDateTimeString(clockString);
			dataString += String(clockString);
			dataString += ",";
		}
		else
		{
			dataString += String(millis());
			dataString += ",";
		}
		
		
		dataString += String(baga.readTemperatureC());
		dataString += ",";
		
		dataString += String(baga.readHeatIndexC());
		dataString += ",";
		
		dataString += String(baga.readAbsHumidity());
		dataString += ",";
		dataString += String(baga.readRelHumidity());
		dataString += ",";
		
		dataString += String(baga.readAbsPressuremBar());
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
