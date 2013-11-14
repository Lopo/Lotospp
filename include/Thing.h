#ifndef LOTOS2_THING_H
#define LOTOS2_THING_H

#include <string>

#include <stdint.h>

// Forward declaration
class User;

class Thing
{
protected:
	Thing();

public:
	virtual ~Thing();

	void addRef();
	void unRef();

	virtual bool isRemoved() const;

private:
	int32_t m_refCount;
};

#endif	/* LOTOS2_THING_H */
