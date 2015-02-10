#ifndef _GROWUP_COMMON_TIME_TIME_H_
#define _GROWUP_COMMON_TIME_TIME_H_

#include <time.h>

namespace growup{namespace common{namespace time
{
	time_t LocalTmToStamp(tm* _tm);
	time_t LocalTmToStamp(int year, int month, int day, int hour, int minute, int second);
	time_t getCurrentUTCStamp();

	void stampToLocalTm(const time_t tm_t,tm* _tm);
	void getCurrentLocalTm(tm *tmptm);
	void getCurrentUTCTm(tm *tmptm);
	unsigned int getCurrentMilliSecond();
	struct time_struct
	{
		int year;
		char month;
		char day;
		char hour;
		char minute;
		char second;
	};
}}}

#endif//_GROWUP_COMMON_TIME_TIME_H_