#ifndef LOTOS2_THING_H
#define LOTOS2_THING_H

#include "config.h"

#include <string>

#include <stdint.h>


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

#endif /* LOTOS2_THING_H */
