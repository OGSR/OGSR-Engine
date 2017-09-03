#include "stdafx.h"

static LPSTR month_id[12] = {
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
};

static int days_in_month[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int start_day = 31;	// 31
static int start_month = 1;	// January
static int start_year = 1999;	// 1999

void compute_build_id()
{
	build_date = __DATE__;

	int					days;
	int					months = 0;
	int					years;
	string16			month;
	string256			buffer;
	strcpy_s(buffer, __DATE__);
	sscanf(buffer, "%s %d %d", month, &days, &years);

	for (int i = 0; i<12; i++) {
		if (_stricmp(month_id[i], month))
			continue;

		months = i;
		break;
	}

	build_id = (years - start_year) * 365 + days - start_day;

	for (int j = 0; j<months; ++j)
		build_id += days_in_month[j];

	for (int k = 0; k<start_month - 1; ++k)
		build_id -= days_in_month[k];
}