#include "Creature.h"


using namespace lotospp;


boost::recursive_mutex AutoID::autoIDLock;
uint32_t AutoID::count=1000;
AutoID::list_type AutoID::list;


Creature::Creature()
{
}

Creature::~Creature()
{
}

Creature* Creature::getCreature()
{
	return this;
}

const Creature* Creature::getCreature() const
{
	return this;
}

void Creature::setID()
{
	this->id=auto_id|this->idRange();
}

uint32_t Creature::getID() const
{
	return id;
}
