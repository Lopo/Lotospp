#ifndef LOTOSPP_COMMON_SINGLETON_H
#define	LOTOSPP_COMMON_SINGLETON_H

#include "config.h"
#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>


namespace LotosPP::Common {

/**
 * The Singleton class is a simple class that can be used to initialize once a
 * resource. The resource being hold in it is initialized only when
 * "getInstance()" is called for the first time, avoiding initialization fiasco.
 */
template<typename T>
class Singleton
	: boost::noncopyable
{
	/// Initialize a resource of type T
	static void initialize()
	{
		m_pointer.reset(new T);
	};

public:
	/**
	 * Initialize the internal instance if still not initialized and returns it
	 *
	 * @return A pointer to the current instance
	 * @throw Any exception the resource can throw during construction or any exception during calling the "new" operator
	 */
	static T* get()
	{
		boost::call_once(initialize, m_flag);
		return m_pointer.get();
	};

private:
	/// A scoped pointer holding the actual resource.
	static inline boost::scoped_ptr<T> m_pointer{nullptr};

	/// One-time initialization flag
	static inline boost::once_flag m_flag{BOOST_ONCE_INIT};
};

	}

#endif
