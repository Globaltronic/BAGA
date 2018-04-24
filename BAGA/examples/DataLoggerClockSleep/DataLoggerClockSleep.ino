#include <Wire.h>
#include <BMP280.h>
#include <Si7020.h>
#include <LowPower.h>
#include <BAGA.h>
#include <SPI.h>
#include <SdFat.h>
#include <SWClock.h>

/************************************************************************
* BAGA A1
* Data Logger Clock Sleep Example 
*
* This example reads data periodically, saves it to the MicroSD Card and
* enters in sleep mode. The timestamp can be generated from an emulated
* software clock that uses a configuration file to set the date and time.
*
* HW Compatibility:
* 	Schematic: A2
*   PCB: A3/A4
*************************************************************************/

// Select the desired debug serial port:
// * Serial (USB-CDC)
// * Serial1 (J3 connector on board)

/**********************
 * BAGA Configuration *
 **********************/

/* Enable debug */
//#define BAGA_DEBUG		

/* Log filename */
char logFileName[] = "log.csv";

/* Configuration filename (used for the SWClock) */
char configFilename[] = "config.txt";

/* Sensor reading periodicity in minutes */
unsigned long ReadIntervalM = 1;

/* Sensor reading periodicity in seconds */
unsigned long ReadIntervalS (ReadIntervalM * 60);

/* Log file header */
char strDataString[] = "Timestamp,Temp_C,Temp_Apar_C,Hum_Abs,Hum_Rel,Press_Abs_mBar,Rad_Sol,Vol_Bat";

/* Select the desired debug serial port */
/* Serial (USB-CDC) or Serial1 (J3 connector on board) */
#if defined(BAGA_DEBUG)
#define DebugSerial  	Serial
#endif

/*************
 * Variables *
 *************/

/* Classes objects */

BAGA baga; 									// BAGA instance
SdFat sd;									// SD card instance
SdFile logFile;								// Log file
SdFile configFile;							// Configuration file
SWClock clock;								// SWCLock instance

/* Globals */
char configString[21];						// String to read the configuration to
char clockString[21];						// String to save the converted date and time
unsigned long timestamp;					// Timestamp
unsigned long ID = 0; 						// Measurements counter


/* Strings */
#if defined(BAGA_DEBUG)
char strOpeningSd[27] = "Opening log file..........";
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";
#endif

/*********
 * SETUP *
 *********/

void setup() {
	// put your setup code here, to run once:
	byte result;

	TXLED1;  // Disable TX LED to Save Power
    RXLED1;	 // Disable RX LED to Save Power

#if defined(BAGA_DEBUG)
	DebugSerial.begin(9600);
	//while(!DebugSerial);
#endif

	//pinMode(20,OUTPUT);
	//digitalWrite(20,LOW);

	result = baga.begin();
	if (result)
	{
		#if defined(BAGA_DEBUG)
		DebugSerial.println(strFail);
		#endif
		baga.blinkForever(3);
	}
	else
	{
		#if defined(BAGA_DEBUG)
		DebugSerial.println(strOk);
		#endif
	}

	/* Initialize SD card (SPI @ 10MHz)*/
	if (!sd.begin(BAGA_SDCARD_CS_PIN, SD_SCK_MHZ(10)))
	{
		#if defined(BAGA_DEBUG)
		sd.initErrorHalt();
		#endif
		baga.blinkForever(4);
	}
	
	/* Open configuration file */
	if (!configFile.open(configFilename, O_READ))
	{
		#if defined(BAGA_DEBUG)
		sd.errorHalt("Error opening config file");
		#endif
		baga.blinkForever(5);
	}
	else
	{
		int data;
		int count = 0;
		
		/* Parse the configuration */
		while(((data = configFile.read()) >= 0) && (count < 21))
		{
			configString[count++] = data;
		}
		
		/* Close the file */
		configFile.close();

		#if defined(BAGA_DEBUG)
		Serial.println(configString);
		#endif

		/* Initialize SWClock */
		clock.begin(configString);
		timestamp = millis();
	}

	/* Open log file */
	if (!logFile.open(logFileName, O_WRITE | O_CREAT | O_APPEND | O_AT_END))
	{
		#if defined(BAGA_DEBUG)
		sd.errorHalt("Error opening file");
		#endif
		baga.blinkForever(6);
	}

	/* Print header to the log file */
	logFile.println(strDataString); // Print to file		

	/* Close the file */
	logFile.close();

	#if defined(BAGA_DEBUG)
	DebugSerial.println(strDataString); // Print to the serial port
	#endif

	/* Blink all the LEDs to signal the correct setup */
	TXLED0;
	RXLED0;
	baga.setLedOn();
	delay(200);
	TXLED1;
	RXLED1;
	baga.setLedOff();
	delay(200);
	TXLED0;
	RXLED0;
	baga.setLedOn();
	delay(200);
	TXLED1;
	RXLED1;
	baga.setLedOff();
}

/*************
 * MAIN LOOP *
 *************/

void loop() {
	// put your main code here, to run repeatedly:

	/* Disable LEDs to save power */
	TXLED1;
    RXLED1;
	
	//digitalWrite(20,HIGH);

	/* Turn system LED on */
	baga.setLedOn(); // Turn LED on to indicate that a measurement is in progress
	
	/* Read the sensors */
	baga.readSensors();
	
	/* Turn system LED off */
	baga.setLedOff();

	//digitalWrite(20,LOW);
	
	/* Open log file */
	if (!logFile.open(logFileName, O_WRITE | O_CREAT | O_APPEND | O_AT_END))
	{
		#if defined(BAGA_DEBUG)
		DebugSerial.println("errorHalt");
		sd.errorHalt("Error opening file");
		#endif
		baga.blinkForever(7);
	}
	else
	{
		String dataString = "";
	
		if(clock.isInitialized() == true)
		{
			/* Get the currente emulated date and time */
			clock.addTime(0,0,0,0,0,(int)((millis()-timestamp)/1000)+ReadIntervalS);
			timestamp = millis();
			
			/* Build the timestamp from the emulated clock */
			clock.getDateTimeString(clockString);

			/* Print timestamp */
			dataString += String(clockString);
			dataString += ",";
		}
		else
		{
			/* Print the current millis value as system timestamp */
			dataString += String(millis());
			dataString += ",";
		}
		
		/* Build the string with the values from the sensors */
		
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
		
		/* Increment measurements counter */
		ID++;
		
		/* Print all the values to the log file */
		logFile.println(dataString);

		/* Close the file */
		logFile.close();

		#if defined(BAGA_DEBUG)
		DebugSerial.println(dataString); // Print to the serial port	
		#endif
	}
  	
	/* Enter sleep mode to save power */
    baga.sleep(ReadIntervalS*1000, ADC_OFF);
}
