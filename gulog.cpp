#include <boost/filesystem.hpp>
#include "gulog.h"
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
#pragma warning(disable : 4996)
#include <Windows.h>
#else
#include <sys/syscall.h>
#include <sys/time.h>
#endif

namespace growup{namespace log
{
	unsigned int getCurrentThreadId()
	{
#if (defined(_WIN32) || defined(__WIN32__) || defined(WIN32))
		return (unsigned int)GetCurrentThreadId();
#else
		return (unsigned int)syscall(SYS_gettid);
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

	const char* log_type_str[6] = 
	{
		" [TRACE]",
		" [DEBUG]",
		" [INFO ]",
		" [WARN ]",
		" [ERROR]",
		" [FATAL]"
	};

	log_mgr& growup::log::log_mgr::getInstance()
	{
		static log_mgr instance;
		return instance;
	}
	log_mgr::~log_mgr()
	{
		_ofs.close();
	}

	void log_mgr::init( const std::string& filename,unsigned int filesize,unsigned short filecount,log_type loglevel )
	{
		_file_name = filename;
		_cur_file_count = 1;
		_file_count_limit = filecount;
		_file_size_limit = filesize;
		_logtype = loglevel;
		_crash_times = GetCrashTimes();
		OpenFile(_cur_file_count,_crash_times);
	}
	void log_mgr::init( const std::string& filename,unsigned int filesize,unsigned short filecount,const std::string& loglevel )
	{
		_file_name = filename;
		_cur_file_count = 1;
		_file_count_limit = filecount;
		_file_size_limit = filesize;
		if(loglevel == "INFO")
			_logtype = GROWUP_LOG_INFO;
		else if(loglevel == "WARNING")
			_logtype = GROWUP_LOG_WARNING;
		else if(loglevel == "ERROR")
			_logtype = GROWUP_LOG_ERROR;
		else if(loglevel == "FATAL")
			_logtype = GROWUP_LOG_FATAL;
		else if(loglevel == "TRACE")
			_logtype = GROWUP_LOG_TRACE;
		else _logtype = GROWUP_LOG_DEBUG;
		_crash_times = GetCrashTimes();
		OpenFile(_cur_file_count,_crash_times);
	}
	void log_mgr::OpenFile(unsigned int file_number,unsigned int crash_number)
	{
		char buffer[32];
		sprintf(buffer,".%d.%d",crash_number,file_number);
		_ofs.open((_file_name+buffer).c_str());
	}
	void log_mgr::CloseFile()
	{
		if(_ofs.is_open())
			_ofs.close();
	}
	void log_mgr::RemoveFile(unsigned int file_number,unsigned int crash_number)
	{
		char buffer[32];
		sprintf(buffer,".%d.%d",crash_number,file_number);
		remove((_file_name+buffer).c_str());
	}

	unsigned int log_mgr::GetCrashTimes()
	{
		unsigned int crash_time = 0;
		boost::filesystem::path dirpath = boost::filesystem::absolute(_file_name).branch_path();
		if(boost::filesystem::is_directory(dirpath))
		{
			boost::filesystem::directory_iterator end;
			for(boost::filesystem::directory_iterator it(dirpath); it != end; ++it)
			{
				if(boost::filesystem::is_regular_file(it->path()) && 
					it->path().filename().string().find(boost::filesystem::absolute(_file_name).filename().string()) == 0)
				{
					std::size_t first = it->path().filename().string().find('.',boost::filesystem::absolute(_file_name).filename().string().size());
					if(first == boost::filesystem::absolute(_file_name).filename().string().size())
					{
						std::size_t second = it->path().filename().string().find('.',first+1);
						if(second != std::string::npos)
						{
							unsigned int tmpint = atoi(it->path().filename().string().substr(first+1,second-first-1).c_str());
							if(tmpint >= crash_time)
								crash_time = tmpint+1;
						}
					}
					else continue;
				}
			}
			return crash_time;
		}
		else
			return crash_time;
	}

	GrowupStream* log_mgr::getOstsByThread( unsigned int threadID )
	{
		boost::mutex::scoped_lock sl(_osts_mutex);
		std::map<unsigned int,GrowupStream*>::iterator it = _osts_map.find(threadID);
		if(it == _osts_map.end())
			it = _osts_map.insert(std::make_pair(threadID,new GrowupStream())).first;
		it->second->clear();
		return it->second;
	}

	void log_mgr::append( const std::vector<char>& buff )
	{
		boost::mutex::scoped_lock sl(_file_mutex);
		if(_ofs.is_open())
		{
			_ofs.write(&buff[0], buff.size());
			_ofs.flush();
			if(_ofs.tellp() > _file_size_limit)
			{
				CloseFile();
				OpenFile(++_cur_file_count,_crash_times);
				if(_cur_file_count > _file_count_limit)
					RemoveFile(_cur_file_count - _file_count_limit,_crash_times);
			}
		}
	}



	LogProcessItem::LogProcessItem(log_type log_level,const char* pFile,unsigned int line ) :_pOs(NULL)
	{
		if(log_level>=log_mgr::getInstance().getLogLevel())
		{
			unsigned int threadId = growup::log::getCurrentThreadId();

			_pOs = log_mgr::getInstance().getOstsByThread(threadId);

			char time_buffer[32];
			::tm tmp_tm;
			getCurrentLocalTm(&tmp_tm);
			sprintf(time_buffer,"\t%02d/%02d/%04d %02d:%02d:%02d:%03d",
				tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_year+1900,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec,
				getCurrentMilliSecond());
			(*_pOs)<<time_buffer;
			(*_pOs)<<log_type_str[log_level];
			(*_pOs)<<"[Thrd: "<<threadId<<"] ";
			(*_pOs)<<pFile<<":"<<line<<"] ";
		}
	}

	LogProcessItem::~LogProcessItem()
	{
		if(_pOs)
		{
			_pOs->addendl();
			log_mgr::getInstance().append(_pOs->getbuff());
		}
	}


	GrowupStream& GrowupStream::operator<<( double v )
	{
		int ret = sprintf(_tmp_buff,"%f",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( float v )
	{
		int ret = sprintf(_tmp_buff,"%f",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( int v )
	{
		int ret = sprintf(_tmp_buff,"%d",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( unsigned int v )
	{
		int ret = sprintf(_tmp_buff,"%u",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( long v )
	{
		int ret = sprintf(_tmp_buff,"%d",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( unsigned long v )
	{
		int ret = sprintf(_tmp_buff,"%u",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( short v )
	{
		int ret = sprintf(_tmp_buff,"%d",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( unsigned short v )
	{
		int ret = sprintf(_tmp_buff,"%u",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( char v )
	{
		int ret = sprintf(_tmp_buff,"%c",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( bool v )
	{
		int ret = sprintf(_tmp_buff,"%s",v?"true":"false");
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( unsigned char v )
	{
		int ret = sprintf(_tmp_buff,"%u",v);
		_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		return *this;
	}

	GrowupStream& GrowupStream::operator<<( const buffer_hex& v )
	{
		int ret;
		int row = 0;
		addendl();
		(*this)<<"*\t0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f";
		for(unsigned int pos = 0; pos < v.len; pos++)
		{
			if(pos%16==0)
			{
				addendl();
				(*this)<<row++<<"\t";
			}
			ret = sprintf(_tmp_buff,"%02hhX ",(const unsigned char)v.pbuff[pos]);
			_buff.insert(_buff.end(),_tmp_buff,_tmp_buff+ret);
		}
		return *this;
	}

}}
