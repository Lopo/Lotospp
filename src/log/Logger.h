#ifndef LOTOSPP_LOG_LOGGER_H
#define LOTOSPP_LOG_LOGGER_H


#include "config.h"

#include <string>
#ifdef __MINGW32__
#	define swprintf _snwprintf
#	define vswprintf _vsnwprintf
#endif

#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "log/severity_t.h"


namespace lotospp {
	namespace log {

class Logger
{
public:
	static Logger* getInstance();
	bool init();
	boost::log::sources::severity_logger_mt<lotospp::log::severity_level>& get() { return _logger;};
private:
	boost::log::sources::severity_logger_mt<lotospp::log::severity_level> _logger;
};

	} // namespace log
} // namespace lotospp

#define LOG(lvl) BOOST_LOG_STREAM_WITH_PARAMS((lotospp::log::Logger::getInstance()->get()), (::boost::log::keywords::severity=lotospp::log::lvl))

#endif // LOTOSPP_LOG_LOGGER_H
