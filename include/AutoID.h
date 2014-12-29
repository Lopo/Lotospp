#ifndef __LOTOS2_TEMPLATES_H__
#define __LOTOS2_TEMPLATES_H__

#include "config.h"

#include <set>

#include <stdint.h>

#include <boost/thread/recursive_mutex.hpp>


class AutoID
{
public:
	AutoID()
	{
		boost::recursive_mutex::scoped_lock lockClass(autoIDLock);
		count++;
		if (count>=0xFFFFFF) {
			count=1000;
			}

		while (list.find(count)!=list.end()) {
			if (count>=0xFFFFFF) {
				count=1000;
				}
			else {
				count++;
				}
			}
		list.insert(count);
		auto_id=count;
	};

	virtual ~AutoID()
	{
		list_type::iterator it=list.find(auto_id);
		if (it!=list.end()) {
			list.erase(it);
			}
	};

	typedef std::set<uint32_t> list_type;

	uint32_t auto_id;
	static boost::recursive_mutex autoIDLock;

protected:
	static uint32_t count;
	static list_type list;
};

#endif
