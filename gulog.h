#ifndef _GROWUP_LOG_H_
#define _GROWUP_LOG_H_

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <time.h>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

namespace growup{namespace log
{
	unsigned int getCurrentThreadId();

	void getCurrentLocalTm(tm *tmptm);

	unsigned int getCurrentMilliSecond();

	enum log_type
	{
		_GROWUP_LOG_TRACE = 0,
		_GROWUP_LOG_DEBUG = 1,
		_GROWUP_LOG_INFO = 2,
		_GROWUP_LOG_WARNING = 3,
		_GROWUP_LOG_ERROR = 4,
		_GROWUP_LOG_FATAL = 5
	};

	extern const char* log_type_str[6];

	struct buffer_hex
	{
		buffer_hex(const char* p, unsigned int l)
			:pbuff(p)
			,len(l)
		{
		}
		const char* pbuff;
		unsigned int len;
	};

	class GrowupStream
	{
	public:
		GrowupStream& operator<<(double v);
		GrowupStream& operator<<(float v);
		GrowupStream& operator<<(int v);
		GrowupStream& operator<<(unsigned int v);
		GrowupStream& operator<<(long v);
		GrowupStream& operator<<(unsigned long v);
		GrowupStream& operator<<(short v);
		GrowupStream& operator<<(unsigned short v);
		GrowupStream& operator<<(char v);
		GrowupStream& operator<<(unsigned char v);
		GrowupStream& operator<<(bool v);
		GrowupStream& operator<<(const buffer_hex& v);
		GrowupStream& operator<<(const char* v)
		{
			_buff.insert(_buff.end(),v,v+strlen(v));
			return *this;
		}
		GrowupStream& operator<<(const std::string& v)
		{
			_buff.insert(_buff.end(),v.c_str(),v.c_str()+v.size());
			return *this;
		}
		const std::vector<char>& getbuff()
		{
			return _buff;
		}
		void clear()
		{
			_buff.clear();
		}
		void addendl()
		{
			_buff.push_back('\n');
		}
	private:
		std::vector<char> _buff;
		char _tmp_buff[128];
	};

	class LogProcessItem
	{
	public:
		LogProcessItem(log_type logtype,const char* pFile,unsigned int line);
		~LogProcessItem();
		template<typename T>
		friend const LogProcessItem& operator<<(const LogProcessItem& lpi,T v);
	private:
		GrowupStream* _pOs;
	};

	class log_mgr
	{
	public:
		friend class LogProcessItem;
		template<typename T>
		friend const LogProcessItem& operator<<(const LogProcessItem& lpi,T v);
	public:
		static log_mgr& getInstance();
		~log_mgr();
	public:
		void init(const std::string& filename,unsigned int filesize,unsigned short filecount,log_type loglevel);
		void init(const std::string& filename,unsigned int filesize,unsigned short filecount,const std::string& loglevel);
		void append(const std::vector<char>& buff);
		inline log_type getLogLevel()
		{
			return _logtype;
		}
		inline void setLogLevel(log_type loglevel)
		{
			_logtype = loglevel;
		}
	private:
		GrowupStream* getTlsOst();
		void OpenFile(unsigned int file_number,unsigned int crash_number);
		void CloseFile();
		void RemoveFile(unsigned int file_number,unsigned int crash_number);
		unsigned int GetCrashTimes();
	private:
		static const int _s_timval = 1000;
	private:
		boost::thread_specific_ptr<GrowupStream> _tls_stream;
		boost::mutex _file_mutex;
		std::ofstream _ofs;
		std::string _file_name;
		unsigned int _file_size_limit;
		unsigned int _crash_times;
		unsigned int _file_count_limit;
		unsigned int _cur_file_count;
		log_type _logtype;
	};

	template<typename T>
	inline const growup::log::LogProcessItem& operator<<(const growup::log::LogProcessItem& lpi,T v)
	{
		if(lpi._pOs)
			(*lpi._pOs)<<v;
		return lpi;
	}
#define GROWUP_HEX_DUMP(ptr,len) \
	growup::log::buffer_hex(ptr,len)

#define GROWUP_LOG_INIT(LogPath,LogSize,LogCount,LogLevel) \
	::growup::log::log_mgr::getInstance().init(LogPath,LogSize,LogCount,LogLevel)

#define GROWUP_LOG_TRACE ::growup::log::_GROWUP_LOG_TRACE
#define GROWUP_LOG_DEBUG ::growup::log::_GROWUP_LOG_DEBUG
#define GROWUP_LOG_INFO ::growup::log::_GROWUP_LOG_INFO
#define GROWUP_LOG_WARNING ::growup::log::_GROWUP_LOG_WARNING
#define GROWUP_LOG_ERROR ::growup::log::_GROWUP_LOG_ERROR
#define GROWUP_LOG_FATAL ::growup::log::_GROWUP_LOG_FATAL

#define GROWUP_LOG(LogLevel) \
	::growup::log::LogProcessItem(LogLevel, __FILE__,__LINE__)


class FunctionEntry  
{
public:
    /** 
        * Constructor
        *
        * @param functName  The name of the function
        * @param file       The file name (supplied by FUNCTION_ENTRY)
        * @param line       The line number (supplied by FUNCTION_ENTRY)
        *
        */
	FunctionEntry(const char* funcName, const char* file, int line)
		: line_(-1)
		, filename_(file)
		, funcname_(funcName)
	{
		::growup::log::LogProcessItem(GROWUP_LOG_TRACE, filename_.c_str(), line) << "FUNCTION ENTER: " << funcname_;
	}
    /**
        * Destructor
        */
    virtual ~FunctionEntry()
	{
		::growup::log::LogProcessItem(GROWUP_LOG_TRACE, filename_.c_str(), line_) << "FUNCTION EXIT : " << funcname_;
	}
    /** 
        * setExitLine
        *
        * Sets the line number to be displayed in the FunctionExit LOG
        * message. Called by the FUNCTION_EXIT macro.
        *
        * @param line  The line number where the function will exit from.
        */
    void setExitLine(int line)
    {
        line_ = line;
    }
private:
    unsigned int line_;				// The line number
	std::string filename_;			// The filename
    std::string funcname_;			// The function name
};

/**
  * FUNCTION_ENTRY
  *
  * Used to log the entry into a member function of a class.
  *
  * @param functname  The name of the function.
  */
#define FUNCTION_ENTRY(functname) ::growup::log::FunctionEntry fe(functname, __FILE__, __LINE__)
/**
  * FUNCTION_EXIT
  *
  * Used to log the exit of a function.
  */
#define FUNCTION_EXIT fe.setExitLine(__LINE__);

}}



#endif//_GROWUP_LOG_H_