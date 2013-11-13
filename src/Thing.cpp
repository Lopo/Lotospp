#include "Lotos2/Thing.h"

Thing::Thing()
	: m_refCount(0)
{
	//
}

Thing::~Thing()
{
}

void Thing::addRef()
{
	++m_refCount;
}

void Thing::unRef()
{
	--m_refCount;
	if (m_refCount<=0) {
		delete this;
		}
}

bool Thing::isRemoved() const
{
	return false;
}
