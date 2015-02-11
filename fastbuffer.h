#ifndef _FASTBUFFER_H_
#define _FASTBUFFER_H_

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>

class fastbuffer
{
public:
	typedef unsigned long size_type;
public:
	fastbuffer();
	fastbuffer(const fastbuffer& r);
	fastbuffer& operator=(const fastbuffer& r);
	~fastbuffer();
public:
	//inline
	inline void	clear()
	{
		_beg = 0;
		_end = 0;
	}
	inline size_type size()
	{
		return _end - _beg;
	}
	inline bool empty()
	{
		return _beg == _end;
	}
	//api for network
	const char* read_pos()
	{
		return _buf + _beg;
	}
	char* write_pos()
	{
		return _buf + _end;
	}
	void pop_read(size_type l)
	{
		assert(l <= size());
		_beg += l;
	}
	size_type	size_for_write()
	{
		return _buf_size - _end;
	}
	void push_write(size_type l)
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