#ifndef __LOTOS2_TEMPLATES_H__
#define __LOTOS2_TEMPLATES_H__

#include <set>

#include <stdint.h>

#include <boost/thread/recursive_mutex.hpp>

template<class T>
class AutoList
{
public:
	AutoList() {}

	~AutoList()
	{
		list.clear();
	}

	void addList(T* t)
	{
		list[t->getID()]=t;
	}

	void removeList(uint32_t _id)
	{
		list.erase(_id);
	}

	typedef std::map<uint32_t, T*> list_type;
	list_type list;

	typedef typename list_type::iterator listiterator;
};

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
	}

	virtual ~AutoID()
	{
		list_type::iterator it=list.find(auto_id);
		if (it!=list.end()) {
			list.erase(it);
			}
	}

	typedef std::set<uint32_t> list_type;

	uint32_t auto_id;
	static boost::recursive_mutex autoIDLock;

protected:
	static uint32_t count;
	static list_type list;
};

#endif
