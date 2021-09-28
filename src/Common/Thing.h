#ifndef LOTOSPP_COMMON_THING_H
#define LOTOSPP_COMMON_THING_H

#include <cstdint>


namespace LotosPP::Common {

class Thing
{
protected:
	Thing();

public:
	virtual ~Thing();

	void addRef();
	void unRef();

private:
	int32_t m_refCount{0};
};

	}

#endif
