#ifndef _GROWUP_COMMON_MEMORY_ANY_H_
#define _GROWUP_COMMON_MEMORY_ANY_H_

namespace growup{namespace common{namespace memory
{
	class Holder_Base
	{
	public:
		virtual ~Holder_Base(){}
		virtual Holder_Base* clone() = 0;
	};

	template<typename T>
	class Holder_Type :public Holder_Base
	{
	public:
		Holder_Type()
		{
		}
		Holder_Type(const T& t):_v(t)
		{
		}
		virtual Holder_Base* clone()
		{
			return new Holder_Type(_v);
		}
		operator T()
		{
			return _v;
		}
	private:
		T _v;
	};

	class Any
	{
	public:
		Any():_pv(0)
		{
		}
		Any(const Any& a)
		{
			_pv = a._pv->clone();
		}
		template<typename T>
		Any(const T& t)
		{
			_pv = new Holder_Type<T>(t);
		}
		~Any()
		{
			delete _pv;
		}
		template<typename T>
		Any& operator=(const T& t)
		{
			if(_pv)
				delete _pv;
			_pv = new Holder_Type<T>(t);
			return *this;
		}
		template<typename T>
		Any& operator=(const Any& t)
		{
			if(_pv)
				delete _pv;
			_pv = a._pv->clone();
			return *this;
		}
		template<typename T>
		operator T()
		{
			if(!_pv)
			{
				_pv = new Holder_Type<T>();
			}
			return (T)(*((Holder_Type<T>*)_pv));
		}
	private:
		Holder_Base* _pv;
	};
}}}

#endif//_GROWUP_COMMON_MEMORY_ANY_H_
