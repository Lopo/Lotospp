/*
 * File:   exception.h
 * Author: lopo
 *
 * Created on August 18, 2013, 8:20 PM
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include <boost/exception/exception.hpp>
#include <boost/throw_exception.hpp>
#include <stdexcept>

#define THROW(x) BOOST_THROW_EXCEPTION(x)

namespace Lotos2
{
	class Exception : public virtual boost::exception, public virtual std::exception
	{
	};
}

#endif /* EXCEPTION_H */
