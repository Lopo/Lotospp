#ifndef LOTOS2_USER_H
#define	LOTOS2_USER_H

#include <list>
#include <map>

#include "AutoList.h"
#include "Thing.h"
#include "network/ProtocolTelnet.h"

class User
	: virtual public Thing
{
public:
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t userCount;
#endif
	User(ProtocolTelnet* p);
	virtual ~User();

	void setRemoved();

	uint32_t getID() const;
	virtual void onRemoved();

	virtual bool isRemoved() const;

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
	friend class IOUser;
};

#endif	/* LOTOS2_USER_H */
