#include <fstream>

#include "Lotos2/network/ProtocolTelnet.h"
#include "Lotos2/Scheduler.h"
#include "Lotos2/tasks.h"
#include "Lotos2/Talker.h"
#include "Lotos2/User.h"
#include "Lotos2/network/OutputMessage.h"
#include "Lotos2/network/Connection.h"


#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t ProtocolTelnet::protocolTelnetCount=0;
#endif

enum {
	TELNET_ECHO=1,
	TELNET_SGA=3,
	TELNET_TERM=24,
	TELNET_NAWS=31,
	TELNET_SE=240,
	TELNET_SB=250,
	TELNET_WILL,
	TELNET_WONT,
	TELNET_DO,
	TELNET_DONT,
	TELNET_IAC
	};

// Helping templates to add dispatcher tasks

template<class FunctionType>
void ProtocolTelnet::addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType& func)
{
	if (droppable) {
		g_dispatcher.addTask(createTask(delay, func));
		}
	else {
		g_dispatcher.addTask(createTask(func));
		}
}

ProtocolTelnet::ProtocolTelnet(Connection_ptr connection)
	: Protocol(connection)
{
	user=NULL;
	m_debugAssertSent=false;
	m_acceptPackets=false;
	eventConnect=0;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	protocolTelnetCount++;
#endif
}

ProtocolTelnet::~ProtocolTelnet()
{
	user=NULL;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	protocolTelnetCount--;
#endif
}

void ProtocolTelnet::setUser(User* u)
{
	user=u;
}

void ProtocolTelnet::releaseProtocol()
{
	//dispatcher thread
	if (user && user->client==this) {
		user->client=NULL;
		}

	Protocol::releaseProtocol();
}

void ProtocolTelnet::deleteProtocolTask()
{
	//dispatcher thread
	if (user) {
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Deleting ProtocolTelnet - Protocol:" << this << ", User: " << user << std::endl;
#endif

		g_talker.FreeThing(user);
		user=NULL;
		}

	Protocol::deleteProtocolTask();
}

bool ProtocolTelnet::connect(uint32_t userId)
{
	unRef();
	eventConnect=0;
	User* _user=g_talker.getUserByID(userId);
	if (!_user || _user->isRemoved() || _user->client) {
		disconnectClient("You are already logged in.");
		return false;
		}

	user=_user;
	user->addRef();
	user->isConnecting=false;
	user->client=this;
	m_acceptPackets=true;

	return true;
}

void ProtocolTelnet::disconnectClient(const char* message)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	if (output) {
		TRACK_MESSAGE(output);
		output->AddString(message);
		OutputMessagePool::getInstance()->send(output);
		}
	disconnect();
}

void ProtocolTelnet::disconnect()
{
	if (getConnection()) {
		getConnection()->closeConnection();
		}
}

void ProtocolTelnet::parsePacket(NetworkMessage &msg, std::size_t bytes_transferred)
{
	msg.setMessageLength(bytes_transferred);
	if (!user || !m_acceptPackets || msg.getMessageLength()<=0) {
		return;
		}
	// Ignore control code replies
	if (msg.GetAt(0)==TELNET_IAC) {
		return;
		}
}

//********************** Send methods  *******************************

void ProtocolTelnet::sendTextMessage(const std::string& message)
{
	NetworkMessage_ptr msg=getOutputBuffer();
	if (msg) {
		TRACK_MESSAGE(msg);
		AddTextMessage(msg, message);
		}
}

void ProtocolTelnet::AddTextMessage(NetworkMessage_ptr msg, const std::string& message)
{
	msg->AddString(message);
}

void ProtocolTelnet::sendEchoOn()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WONT);
	output->AddByte(TELNET_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void ProtocolTelnet::sendEchoOff()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WILL);
	output->AddByte(TELNET_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

template<typename _CharT>
void ProtocolTelnet::setXtermTitle(const _CharT title)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033]0;");
	output->AddString(title);
	output->AddByte(0x07);
	OutputMessagePool::getInstance()->send(output);
}
