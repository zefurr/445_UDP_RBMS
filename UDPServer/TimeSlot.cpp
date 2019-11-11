#include "pch.h"
#include "TimeSlot.h"

TimeSlot::TimeSlot()
{
	this->hour = 0;
	this->mday = 1;
	this->month = 1;
	this->year = 4200;

	this->yday = 1;
}

TimeSlot::TimeSlot(int hour, int mday, int month, int year) // Build using month and mday
{
	this->hour = hour;
	this->mday = mday;
	this->month = month;
	this->year = year;

	// Leap year
	// Year divisible by 4
	// Year cannot be divided by 100, unless it is also divisible by 400
	bool isLeapYear = ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)));
	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (isLeapYear) daysInMonth[1]++;

	// figure out the day of the year for easier comparison later
	this->yday = mday;
	for (int i = 1; i < month; i++) {
		this->yday += daysInMonth[i];
	}
};

TimeSlot::TimeSlot(int hour, int yday, int year) // Build using yday
{
	this->hour = hour;
	this->yday = yday;
	this->year = year;

	// Leap year
	// Year divisible by 4
	// Year cannot be divided by 100, unless it is also divisible by 400
	bool isLeapYear = ((year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0)));
	int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (isLeapYear) daysInMonth[1]++;

	int temp = yday; // For calculating day of month
	int i; // Iterator for months
	for (i = 0; temp > daysInMonth[i]; i++)
	{
		temp -= daysInMonth[i];
	}

	this->mday = temp;
	this->month = i + 1;
};