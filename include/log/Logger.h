#ifndef LOTOS2_LOG_LOGGER_H
#define LOTOS2_LOG_LOGGER_H


#include "config.h"

#include <string>

#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include "log/severity_t.h"


namespace lotos2 {
	namespace log {

class Logger
{
public:
	static Logger* getInstance();
	bool init();
	boost::log::sources::severity_logger_mt<lotos2::log::severity_level>& get() { return _logger;};
private:
	boost::log::sources::severity_logger_mt<lotos2::log::severity_level> _logger;
};

	} // namespace log
} // namespace lotos2

#define LOG(lvl) BOOST_LOG_STREAM_WITH_PARAMS((lotos2::log::Logger::getInstance()->get()), (::boost::log::keywords::severity=lotos2::log::lvl))

#endif // LOTOS2_LOG_LOGGER_H
