#include <Wire.h>
#include <BMP280.h>
#include <Si7020.h>
#include <LowPower.h>
#include <BAGA.h>
#include <SPI.h>
#include <SdFat.h>
#include <SWClock.h>

/************************************************************************
* BAGA A2
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
unsigned long ReadIntervalS = (ReadIntervalM * 60);

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

/* Macros */
#define LF					0x0A
#define CR					0x0D
#define MAX_LINE_LENGTH		31

/* Classes objects */
BAGA baga; 									// BAGA instance
SdFat sd;									// SD card instance
SdFile logFile;								// Log file
SdFile configFile;							// Configuration file
SWClock clock;								// SWCLock instance

/* Globals */
char fileString[MAX_LINE_LENGTH + 1];		// String to read each line from file
char clockString[21];						// String to save the converted date and time
unsigned long timestamp;					// Timestamp
unsigned long ID = 0; 						// Measurements counter

/* Strings */
#if defined(BAGA_DEBUG)
char strOpeningSd[27] = "Opening log file..........";
char strOk[7] = "[ OK ]";
char strFail[7] = "[FAIL]";
#endif

/*************
 * FUNCTIONS *
 *************/
int readLine(SdFile *file, char * outputBuffer)
{
	int data;
	int count = 0;

	/* Read next char */
	data = (*file).read();

	/* Check if is the end of the file */
	if (data == EOF)
	{
		return EOF;
	}
	
	/* Read until an EOF, end of line or carriage return is reached */
	while((data != EOF) && (data != LF) && (data != CR))
	{
		outputBuffer[count++] = data;
		data = (*file).read();
	}

	outputBuffer[count] = '\0';

	return count;
}

/*********
 * SETUP *
 *********/

void setup() {
	// put your setup code here, to run once:
	TXLED1;  // Disable TX LED to Save Power
    RXLED1;	 // Disable RX LED to Save Power

#if defined(BAGA_DEBUG)
	DebugSerial.begin(9600);
	//while(!DebugSerial);
#endif

	if (baga.begin())
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
	
		/* Read lines from file */
		while((count = readLine(&configFile, fileString)) != EOF)
		{	
			/* Check the number of bytes of each line */
			if (count > 0)
			{
				/* Check if it's a Timestamp configuration line */
				if ((fileString[0] == 'T') ||  (fileString[0] == 't'))
				{
					/* Initialize SWClock */
					clock.begin(fileString+1);
					
					/* Save current millis value */
					timestamp = millis();

					#if defined(BAGA_DEBUG)
					DebugSerial.print("Time: ");
					DebugSerial.print(fileString+1);
					DebugSerial.print(" (");
					DebugSerial.print(count, DEC);
					DebugSerial.println(")");
					#endif
				}
				/* Check if it's a sampling Period configuration line */
				else if ((fileString[0] == 'P') ||  (fileString[0] == 'p'))
				{
					/* convert the string */
					ReadIntervalM = (unsigned long)atol(fileString+1);
					
					/* Read interval must be at least 1 minute */
					if(ReadIntervalM < 1)
					{
						ReadIntervalM = 1;
					}

					/* Calculate the new interval value in seconds */
					ReadIntervalS = (ReadIntervalM * 60);

					#if defined(BAGA_DEBUG)
					DebugSerial.print("Period: ");
					DebugSerial.print(fileString+1);
					DebugSerial.print("Converted value: ");
					DebugSerial.println(ReadIntervalM, DEC);
					#endif
				}
			}
		}
		
		/* Close the file */
		configFile.close();

		#if defined(BAGA_DEBUG)
		Serial.println(configString);
		#endif
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

	/* Turn system LED on */
	baga.setLedOn(); // Turn LED on to indicate that a measurement is in progress
	
	/* Read the sensors */
	baga.readSensors();
	
	/* Turn system LED off */
	baga.setLedOff();
	
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

		#if !defined(BAGA_DEBUG)
		Serial1.print("Data: ");
		Serial1.println(dataString);
		#endif
		
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

	if(clock.isInitialized() == true)
	{
		/* Update the emulated date and time */
		clock.addTime(0,0,0,0,0,(int)((millis()-timestamp)/1000)+ReadIntervalS);
		timestamp = millis();
	}
}
