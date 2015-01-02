#include "config.h"

#include <fstream>

#include "network/ProtocolTelnet.h"
#include "Scheduler.h"
#include "tasks.h"
#include "Talker.h"
#include "User.h"
#include "network/OutputMessage.h"
#include "network/Connection.h"

#include "globals.h"


using namespace lotos2;
using lotos2::network::ProtocolTelnet;


#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t ProtocolTelnet::protocolTelnetCount=0;
#endif

enum {
	TELNET_ECHO=1,
	TELNET_SGA=3,
	TELNET_BEEP=7,
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
enum {
	ANSI_RESET,
	ANSI_BOLD,
	ANSI_UNDERLINE=4,
	ANSI_BLINK,
	ANSI_REVERSE=7,
	ANSI_FG_BLACK=30,
	ANSI_FG_RED,
	ANSI_FG_GREEN,
	ANSI_FG_YELLOW,
	ANSI_FG_BLUE,
	ANSI_FG_MAGENTA,
	ANSI_FG_CYAN,
	ANSI_FG_WHITE,
	ANSI_BG_BLACK=40,
	ANSI_BG_RED,
	ANSI_BG_GREEN,
	ANSI_BG_YELLOW,
	ANSI_BG_BLUE,
	ANSI_BG_MAGENTA,
	ANSI_BG_CYAN,
	ANSI_BG_WHITE
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

void ProtocolTelnet::onConnect()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("login from: ");
	output->AddString(this->getIP().to_string());
	output->AddString("\n");
	OutputMessagePool::getInstance()->send(output);
//	user=new User("", this);
}

void ProtocolTelnet::onRecvFirstMessage(NetworkMessage& msg)
{
	parseFirstPacket(msg);
	parsePacket(msg);
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
	eventConnect=0;/*
	User* _user=g_talker.getUserByID(userId);
	if (!_user || _user->client) {
		disconnectClient("You are already logged in.");
		return false;
		}

	user=_user;*/
	user->addRef();
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

//********************** Parse methods *******************************

bool ProtocolTelnet::parseFirstPacket(NetworkMessage &msg)
{
    User* _user=new User("", this);
    addRef();
    return connect(_user->getID());
}

void ProtocolTelnet::parsePacket(NetworkMessage &msg)
{
	uint8_t b;
	int32_t pos;

	if (!user || !m_acceptPackets || msg.getMessageLength()<=0) {
		return;
		}

	while ((pos=msg.getReadPos())<=msg.getMessageLength()) {
		b=msg.GetByte();
		switch (b) {
			case '\r':
				msg.setReadPos(0);
                user->parseLine(msg.GetString().substr(0, pos));
                break;
			case '\0':
			case '\n':
				break;
			}
		}

	// Ignore control code replies
	if (msg.GetByte()==TELNET_IAC) {
		return;
		}
	msg.setReadPos(0);
}

void ProtocolTelnet::parseDebug(NetworkMessage& msg)
{
	int dataLength=msg.getMessageLength()-1;
	if (dataLength!=0) {
		printf("data: ");
		int data=msg.GetByte();
		while (dataLength>0) {
			printf("%d ", data);
			if (--dataLength>0) {
				data=msg.GetByte();
				}
			}
		printf("\n");
		}
}

//********************** Send methods *******************************

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

void ProtocolTelnet::sendTermCoords()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_DO);
	output->AddByte(TELNET_NAWS);
	OutputMessagePool::getInstance()->send(output);
}

void ProtocolTelnet::enableLineWrap()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[7h");
	OutputMessagePool::getInstance()->send(output);
}

void ProtocolTelnet::disableLineWrap()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[7l");
	OutputMessagePool::getInstance()->send(output);
}
/*
void ProtocolTelnet::f1()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[?25h\033c\033[?7h");
	OutputMessagePool::getInstance()->send(output);
}

void ProtocolTelnet::f2()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_DO);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}

void ProtocolTelnet::f3()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WILL);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}
*/
