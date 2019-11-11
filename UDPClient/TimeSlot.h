#pragma once

#include "pch.h"

struct TimeSlot
{
	int hour;	// Hour of the day (00H - 24H)
	int mday;	// Day of the month (1-31)
	int month;	// Month of the year (1-12)
	int year;	// Year

	int yday;	// Day of the year (1-366)

	TimeSlot();
	TimeSlot(int, int, int, int); // Build using month and mday
	TimeSlot(int, int, int); // Build using yday
};