#include "config.h"
#include "version.h"

#include <cstdint>
#include <string>
#include <iostream>

#include "network/protocol/Telnet.h"
#include "Task.h"
#include "User.h"
#include "network/OutputMessage.h"
#include "network/Connection.h"
#include "globals.h"


using namespace lotos2::network::protocol;


#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t Telnet::protocolTelnetCount=0;
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
void Telnet::addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType& func)
{
	if (droppable) {
		g_dispatcher.addTask(createTask(delay, func));
		}
	else {
		g_dispatcher.addTask(createTask(func));
		}
}

Telnet::Telnet(lotos2::network::Connection_ptr connection)
	: Protocol(connection)
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	protocolTelnetCount++;
#endif
}

Telnet::~Telnet()
{
	user=nullptr;
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	protocolTelnetCount--;
#endif
}

void Telnet::onConnect()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\n");
	output->AddString(options.get<std::string>("global.serverName"));
	output->AddString("\n");
	output->AddString(LOTOS2_NAME);
	output->AddString(" version ");
	output->AddString(LOTOS2_VERSION_STRING);
	output->AddString("\n");
	output->AddString("\n\nconnection from: ");
	output->AddString(this->getIP().to_string());
	output->AddString("\n");
	output->AddString("login: ");
	OutputMessagePool::getInstance()->send(output);
//	user=new User("", this);
}

void Telnet::onRecvFirstMessage(lotos2::network::NetworkMessage& msg)
{
	parseFirstPacket(msg);
	parsePacket(msg);
}

void Telnet::setUser(lotos2::User* u)
{
	user=u;
}

void Telnet::releaseProtocol()
{
	//dispatcher thread
	if (user && user->client==this) {
		user->client=nullptr;
		}

	Protocol::releaseProtocol();
}

void Telnet::deleteProtocolTask()
{
	//dispatcher thread
	if (user) {
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Deleting Telnet - Protocol:" << this << ", User: " << user << std::endl;
#endif
		g_talker.FreeThing(user);
		user=nullptr;
		}

	Protocol::deleteProtocolTask();
}

bool Telnet::connect(uint32_t userId)
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

void Telnet::disconnectClient(const char* message)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	if (output) {
		TRACK_MESSAGE(output);
		output->AddString(message);
		OutputMessagePool::getInstance()->send(output);
		}
	disconnect();
}

void Telnet::disconnect()
{
	if (getConnection()) {
		getConnection()->closeConnection();
		}
}

//********************** Parse methods *******************************

bool Telnet::parseFirstPacket(lotos2::network::NetworkMessage &msg)
{
    User* _user=new User(this);
    addRef();
    return connect(_user->getID());
}

void Telnet::parsePacket(lotos2::network::NetworkMessage &msg)
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

void Telnet::parseDebug(lotos2::network::NetworkMessage& msg)
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

void Telnet::sendTextMessage(const std::string& message)
{
	NetworkMessage_ptr msg=getOutputBuffer();
	if (msg) {
		TRACK_MESSAGE(msg);
		AddTextMessage(msg, message);
		}
}

void Telnet::AddTextMessage(lotos2::network::NetworkMessage_ptr msg, const std::string& message)
{
	msg->AddString(message);
}

void Telnet::sendEchoOn()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WONT);
	output->AddByte(TELNET_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::sendEchoOff()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WILL);
	output->AddByte(TELNET_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::setXtermTitle(const std::string& title)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033]0;");
	output->AddString(title);
	output->AddByte(0x07);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::sendTermCoords()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_DO);
	output->AddByte(TELNET_NAWS);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::enableLineWrap()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[7h");
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::disableLineWrap()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[7l");
	OutputMessagePool::getInstance()->send(output);
}
/*
void Telnet::f1()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[?25h\033c\033[?7h");
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::f2()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_DO);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::f3()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(TELNET_IAC);
	output->AddByte(TELNET_WILL);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}
*/
