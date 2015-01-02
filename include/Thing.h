#ifndef LOTOS2_THING_H
#define LOTOS2_THING_H

#include "config.h"

#include <string>

#include <stdint.h>


namespace lotos2 {

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

} // namespace lotos2

#endif /* LOTOS2_THING_H */
