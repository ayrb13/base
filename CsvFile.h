#ifndef _GROWUP_FILE_CSVFILE_H_
#define _GROWUP_FILE_CSVFILE_H_

#include <string>
#include <deque>
#include <fstream>

namespace growup{namespace file{namespace csv
{
	static const int CSVFILE_ROW_BUFFER_MAXSIZE = 10240;
	class CsvItem{
	public:
		CsvItem();
		CsvItem(const std::string& data);
		CsvItem(const CsvItem& data);
		~CsvItem();
		CsvItem& operator=(const CsvItem& data);
		CsvItem& operator=(const std::string& data);
	public:
		inline const std::string& toString() const{
			return m_data;
		}
	private:
		std::string m_data;
	};
	class CsvRow{
	public:
		typedef std::deque<CsvItem>::iterator iterator;
		typedef std::deque<CsvItem>::const_iterator const_iterator;
		typedef std::deque<CsvItem>::reverse_iterator reverse_iterator;
		typedef std::deque<CsvItem>::const_reverse_iterator const_reverse_iterator;
	public:
		inline iterator begin(){
			return m_data.begin();
		}
		inline iterator end(){
			return m_data.end();
		}
		inline const_iterator begin() const{
			return m_data.begin();
		}
		inline const_iterator end() const{
			return m_data.end();
		}
		inline reverse_iterator rbegin(){
			return m_data.rbegin();
		}
		inline reverse_iterator rend(){
			return m_data.rend();
		}
		inline const_reverse_iterator rbegin() const{
			return m_data.rbegin();
		}
		inline const_reverse_iterator rend() const{
			return m_data.rend();
		}
		inline void push_back(const CsvItem& _Val){
			m_data.push_back(_Val);
		}
		inline void push_front(const CsvItem& _Val){
			m_data.push_front(_Val);
		}
		inline void pop_back(){
			m_data.pop_back();
		}
		inline void pop_front(){
			m_data.pop_front();
		}
		inline iterator erase(iterator _Where){
			return m_data.erase(_Where);
		}
		inline iterator erase(iterator _First_arg, iterator _Last_arg){
			return m_data.erase(_First_arg,_Last_arg);
		}
		inline CsvItem& front(){
			return m_data.front();
		}
		inline CsvItem& back(){
			return m_data.back();
		}
		inline const CsvItem&  front() const{
			return m_data.front();
		}
		inline const CsvItem&  back() const{
			return m_data.back();
		}
		inline iterator insert(iterator _Where, const CsvItem& _Val){
			return m_data.insert(_Where,_Val);
		}
		inline CsvItem& operator[](size_t idx){
			return m_data[idx];
		}
		inline const CsvItem& operator[](size_t idx) const{
			return m_data[idx];
		}
		inline size_t size() const{
			return m_data.size();
		}
		inline void clear(){
			m_data.clear();
		}
	public:
		CsvRow();
		CsvRow(const std::string& data);
		CsvRow(const CsvRow& data);
		CsvRow& operator=(const CsvRow& data);
		CsvRow& operator=(const std::string& data);
		std::string toString();
		~CsvRow();
	private:
		std::deque<CsvItem> m_data;
	};
	class CCsvFile_ReadAll{
	public:
	public:
		typedef std::deque<CsvRow>::iterator iterator;
		typedef std::deque<CsvRow>::const_iterator const_iterator;
		typedef std::deque<CsvRow>::reverse_iterator reverse_iterator;
		typedef std::deque<CsvRow>::const_reverse_iterator const_reverse_iterator;
	public:
		inline iterator begin(){
			return m_data.begin();
		}
		inline iterator end(){
			return m_data.end();
		}
		inline const_iterator begin() const{
			return m_data.begin();
		}
		inline const_iterator end() const{
			return m_data.end();
		}
		inline reverse_iterator rbegin(){
			return m_data.rbegin();
		}
		inline reverse_iterator rend(){
			return m_data.rend();
		}
		inline const_reverse_iterator rbegin() const{
			return m_data.rbegin();
		}
		inline const_reverse_iterator rend() const{
			return m_data.rend();
		}
		inline void push_back(const CsvRow& _Val){
			m_data.push_back(_Val);
		}
		inline void push_front(const CsvRow& _Val){
			m_data.push_front(_Val);
		}
		inline void pop_back(){
			m_data.pop_back();
		}
		inline void pop_front(){
			m_data.pop_front();
		}
		inline iterator erase(iterator _Where){
			return m_data.erase(_Where);
		}
		inline iterator erase(iterator _First_arg, iterator _Last_arg){
			return m_data.erase(_First_arg,_Last_arg);
		}
		inline CsvRow& front(){
			return m_data.front();
		}
		inline CsvRow& back(){
			return m_data.back();
		}
		inline const CsvRow&  front() const{
			return m_data.front();
		}
		inline const CsvRow&  back() const{
			return m_data.back();
		}
		inline iterator insert(iterator _Where, const CsvRow& _Val){
			return m_data.insert(_Where,_Val);
		}
		inline CsvRow& operator[](size_t idx){
			return m_data[idx];
		}
		inline const CsvRow& operator[](size_t idx) const{
			return m_data[idx];
		}
		inline size_t size() const{
			return m_data.size();
		}
		inline size_t getWidth(){
			return m_width;
		}
	public:
		CCsvFile_ReadAll();
		CCsvFile_ReadAll(const std::string& path);
		~CCsvFile_ReadAll();
	public:
		bool readCsv(const std::string& path);
		bool saveCsv();
		bool saveCsv(const std::string& path);
		void clear();
	private:
		CCsvFile_ReadAll(const CCsvFile_ReadAll& file);
		CCsvFile_ReadAll& operator=(const CCsvFile_ReadAll& file);
		int m_width;
		std::string m_path;
		std::deque<CsvRow> m_data;
	};
	class CCsvFile_ReadRow
	{
		//因为上面的方法解析csv需要一次性解析所有内容，当csv文件;
		//过大的时候，占用内存量过大，以下方法为读出一条数据，解析一次;
		//严格执行open close操作。;
	public:
		CCsvFile_ReadRow();
		CCsvFile_ReadRow(const std::string& path);
		~CCsvFile_ReadRow();
		bool OpenForReadRow(const std::string& path);
		bool ReadRowFromFile(CsvRow& row);
		void CloseForReadRow();
	private:
		CCsvFile_ReadRow(const CCsvFile_ReadRow& file);
		CCsvFile_ReadRow& operator=(const CCsvFile_ReadRow& file);
		std::ifstream st_ifs;
		char st_buffer[CSVFILE_ROW_BUFFER_MAXSIZE];
	};
}}}
#endif//_GROWUP_FILE_CSVFILE_H_
