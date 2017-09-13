/*
* File:    SWClock.c
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/

/* Include Files */
#include "SWClock.h"

byte SWClockMonthsLength[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

SWClock::SWClock()
{
	second = 0;
	minute = 0;
	hour = 0;
	day = 0;
	month = 0;
	year = 0;
	initialized = false;
}

/***
* Function: begin(int newYear, int newMonth, int newDay, int newHour, int newMinute, int newSecond)
* Description: Initializes SW Clock
* Params: 	newYear - Year
*			newMonth - Month
*			newDay - Day
*			newHour - Hour
*			newMinute - Minute
*			newSecond - Second
* Returns: none
***/
void SWClock::begin(int newYear, int newMonth, int newDay, int newHour, int newMinute, int newSecond)
{
	year = newYear;
	month = constrain(newMonth, 1, 12);
	day = constrain(newDay,1, SWClockMonthsLength[month - 1]);
	hour = constrain(newHour, 0, 23);
	minute = constrain(newMinute, 0, 59);
	second = constrain(newSecond, 0, 59);
	
	initialized = true;
}

void SWClock::begin(char *string)
{
	char *str = "0000";
	int clockYear, clockMonth, clockDay, clockHour, clockMinute, clockSecond;
	
	str = strtok(string, "/-:");
	
	if(str != NULL)
	{
		clockYear = atoi(str);
		str = strtok(NULL, "/-:");
		if(str != NULL)
		{
			clockMonth = atoi(str);
			str = strtok(NULL, "/-:");
			if(str != NULL)
			{
				clockDay = atoi(str);
				str = strtok(NULL, "/-:");
				if(str != NULL)
				{
					clockHour = atoi(str);
					str = strtok(NULL, "/-:");
					if(str != NULL)
					{
						clockMinute = atoi(str);
						str = strtok(NULL, "/-:");
						if(str != NULL)
						{
							clockSecond = atoi(str);
							str = strtok(NULL, "/-:");
						}
					}
				}
			}
		}
	}
	
	begin(clockYear, clockMonth, clockDay, clockHour, clockMinute, clockSecond);
}

bool SWClock::isInitialized()
{
	return initialized;
}

byte SWClock::addTime(int sumYear, int sumMonth, int sumDay, int sumHour, int sumMinute, int sumSecond)
{
	second += sumSecond;
	minute += sumMinute;
	hour += sumHour;
	day += sumDay;
	month += sumMonth;
	year += sumYear;
	
	while(second >= 60)
	{
		second -= 60;
		minute++;
	}
	
	while(minute >= 60)
	{
		minute -= 60;
		hour++;
	}
	
	while(hour >= 24)
	{
		hour -= 24;
		day++;
	}
	
	
	while(day > SWClockMonthsLength[month - 1])
	{
		if ((month == 2) && (isLeapYear(year) == true))
		{
			SWClockMonthsLength[month - 1] = 29;
		}
		
		day -= SWClockMonthsLength[month - 1];
		month++;
		
		if(month > 12)
		{
			month = 1;
			year++;
		}
	}
	
	while(month > 12)
	{
		month -= 12;
		year++;
	}
}

void SWClock::getDateTimeString(char *output)
{
	sprintf(output, "%4d/%02d/%02d-%02d:%02d:%02d", year, month, day, hour, minute, second);
	
	//*output = String(year) + "/" + String(month) + "/" + String(day) + "-" + String(hour) + ":" + String(minute) + ":" + String(second);
}

bool SWClock::isLeapYear(int Year)
{
    if (Year % 400 == 0)
        return true;
    else if (Year % 100 == 0)
        return false;
    else if (Year % 4 == 0)
        return true;
    else
        return false;
}