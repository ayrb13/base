#ifndef _MUTEXFORVARRIANT_H_
#define _MUTEXFORVARRIANT_H_

#include <set>
#include "boost/noncopyable.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"

template<typename var_t>
class mutex_for_varriant : public boost::noncopyable
{
public:
	typedef typename std::set<var_t>::iterator key_t;
public:
	class scoped_lock : public boost::noncopyable
	{
	public:
		scoped_lock(mutex_for_varriant<var_t>& mx, const var_t& var)
			:mx_(mx)
		{
			key_ = mx_.lock(var);
		}
		~scoped_lock()
		{
			mx_.unlock(key_);
		}
	private:
		mutex_for_varriant<var_t>& mx_;
		key_t key_;
	};
public:
	mutex_for_varriant()
		:lock_all_(false)
	{

	}
	void lock()
	{
		boost::mutex::scoped_lock sl(mx_);
		while(!var_set_.empty() || lock_all_)
			cdt_.wait(sl);
		lock_all_ = true;
	}
	void unlock()
	{
		boost::mutex::scoped_lock sl(mx_);
		lock_all_ = false;
		cdt_.notify_all();
	}
	key_t lock(const var_t& var)
	{
		boost::mutex::scoped_lock sl(mx_);
		while(var_set_.find(var) != var_set_.end() || lock_all_)
			cdt_.wait(sl);
		return var_set_.insert(var).first;
	}
	void unlock(key_t key)
	{
		boost::mutex::scoped_lock sl(mx_);
		var_set_.erase(key);
		cdt_.notify_all();
	}
private:
	std::set<var_t> var_set_;
	boost::mutex mx_;
	boost::condition cdt_;
	bool lock_all_;
};

#endif//_MUTEXFORVARRIANT_H_
