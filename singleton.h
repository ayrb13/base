#ifndef _SINGLETON_H_
#define _SINGLETON_H_

template<typename T>
struct singleton
{
	static T& instance()
	{
		static T t;
		return t;
	}
};

#endif//_SINGLETON_H_