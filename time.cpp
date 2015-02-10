#include <growup/common/datetime/time.h>

#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace growup{namespace common{namespace time
{
	time_t LocalTmToStamp(tm* _tm)
	{
		return mktime(_tm);
	}
	time_t LocalTmToStamp(int year, int month, int day, int hour, int minute, int second)
	{
		::tm tmptm;
		tmptm.tm_year = year - 1900;
		tmptm.tm_mon = month - 1;
		tmptm.tm_mday = day;
		tmptm.tm_hour = hour;
		tmptm.tm_min = minute;
		tmptm.tm_sec = second;
		return mktime(&tmptm);
	}
	time_t getCurrentUTCStamp()
	{
		return ::time(NULL);
	}
	void stampToLocalTm(const time_t tm_t,tm* _tm)
	{
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
		::localtime_s(_tm,&tm_t);
#else
		::localtime_r(&tm_t,_tm);
#endif
	}
	void getCurrentLocalTm(tm *tmptm)
	{
		time_t tmp = ::time(NULL);
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
		::localtime_s(tmptm,&tmp);
#else
		::localtime_r(&tmp,tmptm);
#endif
	}
	void getCurrentUTCTm(tm *tmptm)
	{
		time_t tmp = ::time(NULL);
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
		::gmtime_s(tmptm,&tmp);
#else
		::gmtime_r(&tmp,tmptm);
#endif
	}
	unsigned int getCurrentMilliSecond()
	{
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
		SYSTEMTIME currentTime;
		GetSystemTime(&currentTime);
		return currentTime.wMilliseconds;
#else
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return tv.tv_usec / 1000;
#endif
	}

}}}