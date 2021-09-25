#include "Thing.h"


using namespace LotosPP::Common;


Thing::Thing()
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
