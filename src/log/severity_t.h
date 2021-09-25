#ifndef LOTOSPP_LOG_SEVERITY_T_H
#define LOTOSPP_LOG_SEVERITY_T_H


#include "config.h"

#include <iostream>
#include <string>

#include <boost/bimap.hpp>
#include <boost/assign/list_of.hpp>


namespace lotospp {
	namespace log {
		enum severity_level {
			LTRACE,
			LDEBUG,
			LINFO,
			LWARNING,
			LERROR,
			LFATAL
			};
		// Attribute value tag type
		struct severity_tag;
		
		typedef boost::bimap<severity_level, std::string> severityBimap;
		static severityBimap severityMap=boost::assign::list_of<severityBimap::relation>
			(LTRACE, "TRACE")
			(LDEBUG, "DEBUG")
			(LINFO, "INFO")
			(LWARNING, "WARNING")
			(LERROR, "ERROR")
			(LFATAL, "FATAL");

struct severity_t {
	severity_t()
		: level{LINFO}
		{};
	severity_t(const std::string& s)
		: level{to_severity(s)}
		{};
	severity_t(const severity_t& sev)
		: level{sev.level}
		{};
	severity_t& operator=(const severity_t& sev)
	{
		level=sev.level;
		return *this;
	}
	bool operator==(const severity_t& sev) const
	{
		return level==sev.level;
	}
	bool operator!=(const severity_t& sev) const
	{
		return level!=sev.level;
	}
	static severity_level to_severity(const std::string& name);
	const std::string to_string() const;
	static const std::string to_string(const severity_level sev);
	severity_level level;
	};

std::ostream& operator<<(std::ostream& os, const severity_t& sev);
std::istream& operator>>(std::istream& is, severity_t& sev);

	} // namespace log
} // namespace lotospp

#endif // LOTOSPP_LOG_SEVERITY_T_H
