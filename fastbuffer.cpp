#include <stdlib.h>
#include <string.h>
#include "fastbuffer.h"

FastBuffer::FastBuffer() :_buf((char*)malloc(1024))
	,_buf_size(1024)
	,_beg(0)
	,_end(0)
{

}

FastBuffer::FastBuffer( const FastBuffer& r ) :_buf((char*)malloc(r._buf_size))
	,_buf_size(r._buf_size)
	,_beg(r._beg)
	,_end(r._end)
{
	memcpy(_buf + _beg, r._buf + _beg, _end - _beg);
}

FastBuffer& FastBuffer::operator=( const FastBuffer& r )
{
	if(&r != this)
	{
		free(_buf);
		memcpy(_buf + _beg, r._buf + _beg, _end - _beg);
		_buf_size = r._buf_size;
		_beg = r._beg;
		_end = r._end;
	}
	return *this;
}

void FastBuffer::extend(size_type s)
{
	if(_buf_size - _end >= s)
		return;
	if(_beg >= s)
	{
		memmove(_buf,_buf + _beg, _end - _beg);
		_end = _end - _beg;
		_beg = 0;
		return;
	}
	size_type prebeg = _beg;
	size_type preend = _end;
	bool rloc = true;
	while(_buf_size - _end < s)
	{
		if(rloc && _end - _beg < _buf_size >> 1)
		{
			_end = _end - _beg;
			_beg = 0;
			rloc = false;
		}
		_buf_size = _buf_size << 1;
	}
	if(rloc)
	{
		_buf = (char*)realloc(_buf, _buf_size);
	}
	else
	{
		char* tmpbuf = (char*)malloc(_buf_size);
		memcpy(tmpbuf,_buf + prebeg, preend - prebeg);
		free(_buf);
		_buf = tmpbuf;
	}
	assert(_buf);
}

FastBuffer::size_type FastBuffer::read( char* b, size_type l )
{
	if(l > _end - _beg)
		l = _end - _beg;
	memcpy(b, _buf + _beg, l);
	_beg += l;
	return l;
}

FastBuffer::size_type FastBuffer::write( const char* b, size_type l )
{
	extend(l);
	memcpy(_buf + _end, b, l);
	_end += l;
	return l;
}

FastBuffer::~FastBuffer()
{
	free(_buf);
}
