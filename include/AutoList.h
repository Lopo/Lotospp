#ifndef LOTOS2_AUTOLIST_H
#define LOTOS2_AUTOLIST_H


#include "config.h"

#include <cstdint>
#include <map>


namespace lotos2 {

template<class T>
class AutoList
{
public:
	AutoList() {};

	~AutoList()
	{
		list.clear();
	};

	void addList(T* t)
	{
		list[t->getID()]=t;
	};

	void removeList(uint32_t _id)
	{
		list.erase(_id);
	};

	typedef std::map<uint32_t, T*> list_type;
	list_type list;

	typedef typename list_type::iterator listiterator;
};

} // namespace lotos2

#endif // LOTOS2_AUTOLIST_H
