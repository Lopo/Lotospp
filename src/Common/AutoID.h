#ifndef LOTOSPP_COMMON_AUTOID_H
#define LOTOSPP_COMMON_AUTOID_H

#include <boost/thread/recursive_mutex.hpp>
#include <set>
#include <cstdint>


namespace LotosPP::Common {

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
	static inline boost::recursive_mutex autoIDLock;

protected:
	static inline uint32_t count{1000};
	static inline list_type list{};
};

	}

#endif
