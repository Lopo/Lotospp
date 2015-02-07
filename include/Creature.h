#ifndef LOTOS2_CREATURE_H
#define	LOTOS2_CREATURE_H


#include "config.h"

#include <stdint.h>

#include <string>

#include "AutoID.h"
#include "Thing.h"


namespace lotos2 {

class Creature
	: public AutoID,
		virtual public Thing
{
protected:
	Creature();

public:
	virtual ~Creature();

	virtual Creature* getCreature();
	virtual const Creature* getCreature() const;

	virtual const std::string& getName() const =0;

	void setID();

	virtual uint32_t idRange() =0;
    uint32_t getID() const;
    virtual void addList() =0;
    virtual void removeList() =0;

protected:
	uint32_t id=0;

	friend class Talker;
};

}

#endif // LOTOS2_CREATURE_H
