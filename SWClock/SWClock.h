/*
* File:    SWClock.h
* Author:  Hugo Cunha
* Company: Globaltronic SA
*/


/***
* SWClock A1
*
* Software Clock library
***/


#ifndef SWCLOCK_H
#define SWCLOCK_H



/* Include Files */
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SWClock
{
	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;
	bool initialized;
	
	public:
		SWClock();
		void 			begin(int newYear, int newMonth, int newDay, int newHour, int newMinute, int newSecond);
		void 			begin(char *string);
		bool			isInitialized();
		byte			addTime(int sumYear, int sumMonth, int sumDay, int sumHour, int sumMinute, int sumSecond);
		void			getDateTimeString(char *output);
		bool			isLeapYear(int Year);

	private:


};
























#endif