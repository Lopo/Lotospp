#ifndef LOTOSPP_LOG_LOGGER_H
#define LOTOSPP_LOG_LOGGER_H

#include "severity_t.h"
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <string>


namespace LotosPP::Log {

class Logger
{
public:
	static Logger* getInstance();
	bool init();
	boost::log::sources::severity_logger_mt<LotosPP::Log::severity_level>& get()
	{
		return _logger;
	};
private:
	boost::log::sources::severity_logger_mt<LotosPP::Log::severity_level> _logger;
};

	}

#define LOG(lvl) BOOST_LOG_STREAM_WITH_PARAMS((LotosPP::Log::Logger::getInstance()->get()), (::boost::log::keywords::severity=LotosPP::Log::lvl))

#endif
