#include <boost/filesystem.hpp>
#include <growup/thread/thread.h>
#include <growup/common/datetime/time.h>
#include <growup/log/log.h>
namespace growup{namespace log
{
	const char* log_type_str[] = 
	{
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

	void log_mgr::OpenFile(unsigned int file_number,unsigned int crash_number)
	{
		char buffer[32];
		sprintf(buffer,".%d.%d",crash_number,file_number);
		_ofs.open(_file_name+buffer);
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
					std::size_t first = it->path().filename().string().rfind('.');
					if(first != std::string::npos&&first != 0)
					{
						std::size_t second = it->path().filename().string().rfind('.',first - 1);
						if(second != std::string::npos&&second != 0)
						{
							unsigned int tmpint = atoi(it->path().filename().string().substr(second+1,first-second-1).c_str());
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

	void log_mgr::checkFrontAndLock()
	{
		_file_mutex.lock();
		if(!_ofs.is_open())
		{
			OpenFile(++_cur_file_count,_crash_times);
			if(_cur_file_count > _file_count_limit)
				RemoveFile(_cur_file_count - _file_count_limit,_crash_times);
		}
	}

	void log_mgr::checkTailAndUnlock()
	{
		_ofs.flush();
		if(_ofs.tellp() > _file_size_limit)
			CloseFile();
		_file_mutex.unlock();
	}

	std::ostringstream& log_mgr::getOstsByThread( unsigned int threadID )
	{
		boost::mutex::scoped_lock sl(_osts_mutex);
		return _osts_map[threadID];
	}



	LogProcessItem::LogProcessItem(log_type log_level,const char* pFile,unsigned int line ) :_pOs(NULL)
	{
		if(log_level>=log_mgr::getInstance().getLogLevel())
		{
			unsigned int threadId = growup::thread::getCurrentThreadId();

			_pOs = &log_mgr::getInstance().getOstsByThread(threadId);

			_pOs->str("");
			char time_buffer[32];
			::tm tmp_tm;
			growup::common::time::getCurrentLocalTm(&tmp_tm);
			sprintf(time_buffer,"\t%02d/%02d/%04d %02d:%02d:%02d:%03d",
				tmp_tm.tm_mon+1,tmp_tm.tm_mday,tmp_tm.tm_year+1900,tmp_tm.tm_hour,tmp_tm.tm_min,tmp_tm.tm_sec,
				growup::common::time::getCurrentMilliSecond());
			(*_pOs)<<time_buffer;
			(*_pOs)<<log_type_str[log_level];
			(*_pOs)<<"[Thrd: "<<threadId<<"] ";
			(*_pOs)<<pFile<<":"<<line<<":";
		}
	}

	LogProcessItem::~LogProcessItem()
	{
		if(_pOs)
		{
			log_mgr::getInstance().checkFrontAndLock();
			log_mgr::getInstance()._ofs<<_pOs->str()<<std::endl;
			log_mgr::getInstance().checkTailAndUnlock();
		}
	}

}}