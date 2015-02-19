#include "config.h"
#include "Creature.h"


using namespace lotos2;
using lotos2::Creature;


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
