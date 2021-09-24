#ifndef LOTOSPP_THING_H
#define LOTOSPP_THING_H


#include "config.h"

#include <cstdint>


namespace lotospp {

class Thing
{
protected:
	Thing();

public:
	virtual ~Thing();

	void addRef();
	void unRef();

private:
	int32_t m_refCount;
};

} // namespace lotospp

#endif // LOTOSPP_THING_H
