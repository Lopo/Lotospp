#ifndef LOTOSPP_SYSTEM_SYSTEM_H
#define LOTOSPP_SYSTEM_SYSTEM_H

#include <boost/date_time/posix_time/posix_time.hpp>


inline int64_t SYS_TIME()
{
	return (boost::posix_time::microsec_clock::universal_time()-boost::posix_time::ptime(boost::gregorian::date(1970,1,1))).total_milliseconds();
};

#endif
