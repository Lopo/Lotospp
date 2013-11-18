#ifndef __LOTOS2_AUTOLIST_H__
#define __LOTOS2_AUTOLIST_H__

#include <map>

#include <stdint.h>

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

#endif /* __LOTOS2_AUTOLIST_H__ */
