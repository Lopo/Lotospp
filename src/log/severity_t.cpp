#include "config.h"


#include "log/severity_t.h"

#include <stdexcept>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/program_options/errors.hpp>


using namespace lotos2::log;


severity_level severity_t::to_severity(const std::string& name)
{
	severityBimap::right_const_iterator it=severityMap.right.find(boost::algorithm::to_upper_copy(name));
	if (it!=severityMap.right.end()) {
		return it->second;
		}
	using poverr=boost::program_options::validation_error;
	throw poverr(poverr::invalid_option_value);
}

const std::string severity_t::to_string() const
{
	severityBimap::left_const_iterator it=severityMap.left.find(level);
	if (it!=severityMap.left.end()) {
		return it->second;
		}
	throw std::runtime_error("can't convert severity to string");
}

const std::string severity_t::to_string(const severity_level lvl)
{
	severityBimap::left_const_iterator it=severityMap.left.find(lvl);
	if (it!=severityMap.left.end()) {
		return it->second;
		}
	throw std::runtime_error("can't convert severity lvl to string");
}

std::ostream& lotos2::log::operator<<(std::ostream& os, const severity_t& sev)
{
	os << sev.to_string();
	return os;
}

std::istream& lotos2::log::operator>>(std::istream& is, severity_t& sev)
{
	std::string name;
	is >> name;
	sev.level=severity_t::to_severity(name);
	return is;
}

namespace lotos2 { namespace log { // why are these required ?
template<typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& strm, severity_level lvl)
{
	try {
		strm << severity_t::to_string(lvl);
		}
	catch (std::runtime_error) {
		strm << static_cast<int>(lvl);
		}
	return strm;
}
}} // namespaces
