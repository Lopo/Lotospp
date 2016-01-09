#include "log/Logger.h"

#include <iostream>

#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "Singleton.h"
#include "globals.h"


using namespace lotospp::log;
namespace logging=boost::log;


BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(processid, "ProcessID", logging::attributes::current_process_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(threadid, "ThreadID", logging::attributes::current_thread_id::value_type)


Logger* Logger::getInstance()
{
	static Singleton<Logger> instance;
	return instance.get();
}

bool Logger::init()
{
	namespace expr=boost::log::expressions;
	namespace sinks=boost::log::sinks;
	namespace kwd=boost::log::keywords;

	boost::shared_ptr<logging::core> core=logging::core::get();
	core->remove_all_sinks();

	logging::formatter fmt=expr::stream
			<< expr::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S.%f")
			<< " <" << processid << "/" << threadid << "> : [" << severity << "] "
			<< expr::smessage;

	typedef sinks::synchronous_sink<sinks::text_ostream_backend> textSink;
	typedef sinks::synchronous_sink<sinks::text_file_backend> fileSink;

	boost::shared_ptr<textSink> cSink=boost::make_shared<textSink>();
	cSink->set_formatter(fmt);
	cSink->set_filter(severity>=lotospp::log::severity_t::to_severity(options.get("global.log.console.level", "")));
	boost::shared_ptr<std::ostream> cStream(&std::clog, boost::null_deleter());
	cSink->locked_backend()->add_stream(cStream);
	core->add_sink(cSink);

	boost::shared_ptr<sinks::text_file_backend> fBck=boost::make_shared<sinks::text_file_backend>(
		kwd::file_name=options.get("global.log.dir", "")+"/%Y-%m-%d.log",
		kwd::time_based_rotation=sinks::file::rotation_at_time_point(0, 0, 0),
		kwd::auto_flush=true,
		kwd::open_mode=std::ios_base::app|std::ios_base::out|std::ios_base::ate // append, don't overwrite
		);
	boost::shared_ptr<fileSink> fSink(new fileSink(fBck));
	fSink->set_formatter(fmt);
	fSink->set_filter(severity>=lotospp::log::severity_t::to_severity(options.get("global.log.file.level", "")));
	core->add_sink(fSink);

	logging::add_common_attributes();
	return true;
}
