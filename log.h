#ifndef _GROWUP_LOG_H_
#define _GROWUP_LOG_H_

#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <boost/thread/mutex.hpp>

namespace growup{namespace log
{
	enum log_type
	{
		_GROWUP_LOG_DEBUG = 0,
		_GROWUP_LOG_INFO = 1,
		_GROWUP_LOG_WARNING = 2,
		_GROWUP_LOG_ERROR = 3,
		_GROWUP_LOG_FATAL = 4
	};

	const char* log_type_str[];

	class LogProcessItem
	{
	public:
		LogProcessItem(log_type logtype,const char* pFile,unsigned int line);
		~LogProcessItem();
		template<typename T>
		friend LogProcessItem& operator<<(LogProcessItem& lpi,T v);
	private:
		std::ostringstream* _pOs;
	};

	class log_mgr
	{
	public:
		friend class LogProcessItem;
		template<typename T>
		friend LogProcessItem& operator<<(LogProcessItem& lpi,T v);
	public:
		static log_mgr& getInstance();
		~log_mgr();
	public:
		void init(const std::string& filename,unsigned int filesize,unsigned short filecount,log_type loglevel);
		inline log_type getLogLevel()
		{
			return _logtype;
		}
		inline void setLogLevel(log_type loglevel)
		{
			_logtype = loglevel;
		}
	private:
		std::ostringstream& getOstsByThread(unsigned int threadID);
		void checkFrontAndLock();
		void checkTailAndUnlock();
		void OpenFile(unsigned int file_number,unsigned int crash_number);
		void CloseFile();
		void RemoveFile(unsigned int file_number,unsigned int crash_number);
		unsigned int GetCrashTimes();
	private:
		static const int _s_timval = 1000;
	private:
		std::map<unsigned int,std::ostringstream> _osts_map;
		boost::mutex _file_mutex;
		boost::mutex _osts_mutex;
		std::ofstream _ofs;
		std::string _file_name;
		unsigned int _file_size_limit;
		unsigned int _crash_times;
		unsigned short _file_count_limit;
		unsigned short _cur_file_count;
		log_type _logtype;
	};

	template<typename T>
	LogProcessItem& operator<<(LogProcessItem& lpi,T v)
	{
		if(lpi._pOs)
			(*lpi._pOs)<<v;
		return lpi;
	}


#define GROWUP_LOG_INIT(LogPath,LogSize,LogCount,LogLevel) \
	::growup::log::log_mgr::getInstance().init(LogPath,LogSize,LogCount,LogLevel)

#define GROWUP_LOG_DEBUG ::growup::log::_GROWUP_LOG_DEBUG
#define GROWUP_LOG_INFO ::growup::log::_GROWUP_LOG_INFO
#define GROWUP_LOG_WARNING ::growup::log::_GROWUP_LOG_WARNING
#define GROWUP_LOG_ERROR ::growup::log::_GROWUP_LOG_ERROR
#define GROWUP_LOG_FATAL ::growup::log::_GROWUP_LOG_FATAL

#define GROWUP_LOG(LogLevel) \
	::growup::log::LogProcessItem(LogLevel, __FILE__,__LINE__)
}}

#endif//_GROWUP_LOG_H_