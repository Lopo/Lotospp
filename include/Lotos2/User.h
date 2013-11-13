#ifndef LOTOS2_USER_H
#define	LOTOS2_USER_H

#include <list>
#include <map>

#include "Lotos2/templates.h"
#include "Lotos2/Thing.h"
#include "Lotos2/network/ProtocolTelnet.h"

class User
	: public AutoID,
		virtual public Thing
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t userCount;
#endif
	User(ProtocolTelnet* p);
	virtual ~User();

	void setID();
	void setRemoved();

	uint32_t getID() const;
	virtual void onRemoved();

	virtual bool isRemoved() const;

	virtual uint32_t idRange() { return 0x10000000;}
	static AutoList<User> listUser;
	void removeList();
	void addList();

	bool isOffline() const { return getID()==0;}
	void disconnect() { if(client) client->disconnect();}

	void sendTextMessage(const std::string& message) const
	{
		if (client) {
			client->sendTextMessage(message);
			}
	}

protected:
	uint32_t id;
	bool isInternalRemoved;

	ProtocolTelnet* client;

	bool isConnecting;

	friend class Talker;
	friend class ProtocolTelnet;
};

#endif	/* LOTOS2_USER_H */
