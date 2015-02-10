#include <growup/file/CsvFile.h>

namespace growup{namespace file{namespace csv
{
	CsvItem::CsvItem():m_data(""){}
	CsvItem::CsvItem(const std::string& data):m_data(data){}
	CsvItem::CsvItem(const CsvItem& data):m_data(data.m_data){}
	CsvItem& CsvItem::operator=(const CsvItem& data){
		m_data = data.m_data;
		return *this;
	}
	CsvItem& CsvItem::operator=(const std::string& data){
		m_data = data;
		return *this;
	}
	CsvItem::~CsvItem(){}

	CsvRow::CsvRow():m_data(){}
	CsvRow::CsvRow(const std::string& data):m_data()
	{
		*this = data;
	}
	CsvRow::CsvRow(const CsvRow& data):m_data(data.m_data)
	{
	}
	CsvRow& CsvRow::operator=(const CsvRow& data)
	{
		m_data = data.m_data;
		return *this;
	}
	CsvRow& CsvRow::operator=(const std::string& data)
	{
		m_data.clear();
		size_t last = 0;
		size_t index;
		while((index=data.find(",",last))!=std::string::npos)
		{
			push_back(CsvItem(data.substr(last,index-last)));
			last=index+1;
		}
		if (index-last>0)
		{
			push_back(CsvItem(data.substr(last,index-last)));
		}
		return *this;
	}
	CsvRow::~CsvRow(){}
	std::string CsvRow::toString()
	{
		std::string tmp("");
		for(iterator it = begin(); it != end(); ++it)
		{
			tmp+=it->toString();
			tmp+=",";
		}
		tmp.erase(tmp.end()-1);
		return tmp;
	}

	CCsvFile_ReadAll::CCsvFile_ReadAll():m_width(0)
	{
		clear();
	}
	CCsvFile_ReadAll::CCsvFile_ReadAll(const std::string& path):m_path(path),m_width(0)
	{
		readCsv(m_path);
	}
	CCsvFile_ReadAll::~CCsvFile_ReadAll()
	{
	}
	bool CCsvFile_ReadAll::readCsv(const std::string& path)
	{
		char szbuff[CSVFILE_ROW_BUFFER_MAXSIZE];
		CsvRow row;
		size_t size;
		m_data.clear();
		std::ifstream ifs;
		ifs.open(path.c_str(),std::ios_base::in);
		if (!ifs.is_open())
			return false;
		while(!ifs.eof())
		{
			ifs.getline(szbuff,CSVFILE_ROW_BUFFER_MAXSIZE);
			row = szbuff;
			size = row.size();
			if(m_width == 0)
			{
				m_width = size;
				m_data.push_back(row);
			}
			else if(m_width == size)
			{
				m_data.push_back(row);
			}
		}
		return true;
	}
	bool CCsvFile_ReadAll::saveCsv()
	{
		return saveCsv(m_path);
	}
	bool CCsvFile_ReadAll::saveCsv(const std::string& path)
	{
		std::ofstream ofs;
		ofs.open(path.c_str());
		std::string tmp;
		for(iterator it = begin(); it != end(); ++it)
		{
			tmp = it->toString();
			tmp += "\n";
			ofs.write(tmp.c_str(),tmp.size());
		}
		ofs.close();
		return true;
	}
	void CCsvFile_ReadAll::clear(){
		m_width=0;
		m_data.clear();
	}

	CCsvFile_ReadRow::CCsvFile_ReadRow(){}
	CCsvFile_ReadRow::CCsvFile_ReadRow(const std::string& path)
	{
		OpenForReadRow(path);
	}
	CCsvFile_ReadRow::~CCsvFile_ReadRow()
	{
		CloseForReadRow();
	}
	bool CCsvFile_ReadRow::OpenForReadRow(const std::string& path)
	{
		if(st_ifs.is_open()) st_ifs.close();
		st_ifs.open(path.c_str());
		return st_ifs.is_open();
	}
	bool CCsvFile_ReadRow::ReadRowFromFile(CsvRow& row)
	{
		if(!st_ifs.is_open()) return false;
		if(!st_ifs.eof())
		{
			st_ifs.getline(st_buffer,CSVFILE_ROW_BUFFER_MAXSIZE);
			row = st_buffer;
			return true;
		}
		else return false;
	}
	void CCsvFile_ReadRow::CloseForReadRow()
	{
		st_ifs.close();
	}
}}}
