#ifndef _FASTBUFFER_H_
#define _FASTBUFFER_H_

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

class FastBuffer
{
public:
	typedef unsigned long size_type;
public:
	FastBuffer();
	FastBuffer(const FastBuffer& r);
	FastBuffer& operator=(const FastBuffer& r);
	~FastBuffer();
public:
	//inline
	inline void	clear()
	{
		_beg = 0;
		_end = 0;
	}
	inline size_type size() const
	{
		return _end - _beg;
	}
	inline bool empty() const
	{
		return _beg == _end;
	}
	//api for network
	inline const char* read_pos() const
	{
		return _buf + _beg;
	}
	inline char* write_pos()
	{
		return _buf + _end;
	}
	inline void pop_read(size_type l)
	{
		assert(l <= size());
		_beg += l;
	}
	inline size_type size_for_write() const
	{
		return _buf_size - _end;
	}
	inline void push_write(size_type l)
	{
		assert(l <= size_for_write());
		_end += l;
	}
public:
	//normal api
	size_type	read(char* b, size_type l);
	size_type	write(const char* b, size_type l);
	void		extend(size_type s);
private:
	char*		_buf;
	size_type	_buf_size;
	size_type	_beg;
	size_type	_end;
};

#endif//_FASTBUFFER_H_