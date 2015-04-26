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
#include "generated/consts.h"
#include "globals.h"


using namespace lotos2::network::protocol;


#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t Telnet::protocolTelnetCount=0;
#endif

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
	output->AddString(options.get("global.serverName", ""));
	output->AddString("\n");
	output->AddString(LOTOS2_NAME);
	output->AddString(" version ");
	output->AddString(LOTOS2_VERSION_STRING);
	output->AddString("\n");
	output->AddString("\n\nconnection from: ");
	output->AddString(getAddress().to_string());
	output->AddString("\n");
	output->AddString("login: ");
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::onRecvFirstMessage(lotos2::network::NetworkMessage& msg)
{
	parseFirstPacket(msg);
	parsePacket(msg);
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
	eventConnect=0;
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

//********************** Parse methods *******************************

bool Telnet::parseFirstPacket(lotos2::network::NetworkMessage &msg)
{
    User* _user=new User("", this);
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
	if (msg.GetByte()==enums::TELNET_IAC) {
		return;
		}
	msg.setReadPos(0);
}

void Telnet::parseDebug(lotos2::network::NetworkMessage& msg)
{
	int32_t pos=msg.getReadPos(),
		dataLength=msg.getMessageLength();
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
	msg.setReadPos(pos);
}

//********************** Send methods *******************************

void Telnet::sendEchoOn()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELNET_IAC);
	output->AddByte(enums::TELNET_WONT);
	output->AddByte(enums::TELNET_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::sendEchoOff()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELNET_IAC);
	output->AddByte(enums::TELNET_WILL);
	output->AddByte(enums::TELNET_ECHO);
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
	output->AddByte(enums::TELNET_IAC);
	output->AddByte(enums::TELNET_DO);
	output->AddByte(enums::TELNET_NAWS);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::enableLineWrap()
{
	write("\033[7h");
}

void Telnet::disableLineWrap()
{
	write("\033[7l");
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
	output->AddByte(enums::TELNET_IAC);
	output->AddByte(enums::TELNET_DO);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::f3()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELNET_IAC);
	output->AddByte(enums::TELNET_WILL);
	output->AddByte(TELOPT_NEW_ENVIRON);
	OutputMessagePool::getInstance()->send(output);
}
*/
