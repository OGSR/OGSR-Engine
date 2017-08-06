////////////////////////////////////////////////////////////////////////////
//	Module 		: date_time.h
//	Created 	: 08.05.2004
//  Modified 	: 08.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Date and time routines
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

u64	generate_time	(u32 years, u32 months, u32 days, u32 hours, u32 minutes, u32 seconds, u32 milliseconds)
{
	THROW					(years > 0);
	THROW					(months > 0);
	THROW					(days > 0);
	u64						t1,t2,t3,t4;
	t1						= years/400;
	t2						= years/100;
	t3						= years/4;
	bool					a1,a2,a3;
	a1						= !(years%400);
	a2						= !(years%100);
	a3						= !(years%4);
	t4						= a3 && (!a2 || a1) ? 1 : 0;
	u64						result = u64(years - 1)*u64(365) + t1 - t2 + t3;

	if (months >  1) result	+= u64(31);
	if (months >  2) result	+= u64(28 + t4);
	if (months >  3) result	+= u64(31);
	if (months >  4) result	+= u64(30);
	if (months >  5) result	+= u64(31);
	if (months >  6) result	+= u64(30);
	if (months >  7) result	+= u64(31);
	if (months >  8) result	+= u64(31);
	if (months >  9) result	+= u64(30);
	if (months > 10) result	+= u64(31);
	if (months > 11) result	+= u64(30);
	result					+= u64(days - 1);
	result					= result*u64(24) + u64(hours);
	result					= result*u64(60) + u64(minutes);
	result					= result*u64(60) + u64(seconds);
	result					= result*u64(1000) + u64(milliseconds);

#if 0
	u32 _years; u32 _months; u32 _days; u32 _hours; u32 _minutes; u32 _seconds; u32 _milliseconds;
	split_time				(result,_years,_months,_days,_hours,_minutes,_seconds,_milliseconds);
	VERIFY					(years == _years);
	VERIFY					(months == _months);
	VERIFY					(days == _days);
	VERIFY					(hours == _hours);
	VERIFY					(minutes == _minutes);
	VERIFY					(seconds == _seconds);
	VERIFY					(milliseconds == _milliseconds);
#endif

	return					(result);
}

void split_time		(u64 time, u32 &years, u32 &months, u32 &days, u32 &hours, u32 &minutes, u32 &seconds, u32 &milliseconds)
{
	milliseconds			= u32(time%1000);
	time					/= 1000;
	seconds					= u32(time%60);
	time					/= 60;
	minutes					= u32(time%60);
	time					/= 60;
	hours					= u32(time%24);
	time					/= 24;
	years					= u32(u64(400)*time/u64(365*400 + 100 - 4 + 1) + 1);
	u64						t1,t2,t3,t4;
	t1						= years/400;
	t2						= years/100;
	t3						= years/4;
	bool					a1,a2,a3;
	a1						= !(years%400);
	a2						= !(years%100);
	a3						= !(years%4);
	t4						= a3 && (!a2 || a1) ? 1 : 0;
	u64						result = u64(years - 1)*u64(365) + t1 - t2 + t3;
	time					-= result;
	++time;
	months					= 1;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 28 + t4) {
		++months;
		time				-= 28 + t4;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 30) {
		++months;
		time				-= 30;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 30) {
		++months;
		time				-= 30;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 30) {
		++months;
		time				-= 30;
	if (time > 31) {
		++months;
		time				-= 31;
	if (time > 30) {
		++months;
		time				-= 30;
	}}}}}}}}}}}
	days					= u32(time);
}
