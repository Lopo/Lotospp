#ifndef LOTOSPP_AUTOLIST_H
#define LOTOSPP_AUTOLIST_H


#include "config.h"

#include <cstdint>
#include <map>


namespace lotospp {

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

} // namespace lotospp

#endif // LOTOSPP_AUTOLIST_H
