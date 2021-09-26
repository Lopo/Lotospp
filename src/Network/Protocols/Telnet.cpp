#include "Telnet.h"
#include "Lotospp/buildinfo.h"
#include "Common/Task.h"
#include "Common/User.h"
#include "../OutputMessage.h"
#include "../Connection.h"
#include "Common/Enums/TelnetCmd.h"
#include "Common/Enums/TelnetOpt.h"
#include "globals.h"
#include "Log/Logger.h"
#include <iostream>


using namespace LotosPP::Network::Protocols;
namespace enums=LotosPP::Common::enums;

// Helping templates to add dispatcher tasks

template<typename FunctionType>
void Telnet::addTalkerTaskInternal(bool droppable, uint32_t delay, const FunctionType& func)
{
	if (droppable) {
		g_dispatcher.addTask(createTask(delay, func));
		}
	else {
		g_dispatcher.addTask(createTask(func));
		}
}

Telnet::Telnet(LotosPP::Network::Connection_ptr connection)
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
	boost::asio::ip::address adr=getAddress();

	LOG(LINFO) << "User connection from " << adr;

	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\n")
		->AddString(options.get("global.serverName", ""))
		->AddString("\n")
		->AddString("Lotos++")
		->AddString(" version ")
		->AddString(Lotospp_get_buildinfo()->project_version)
		->AddString("\n")
		->AddString("\n\nconnection from: ")
		->AddString(adr.to_string())
		->AddString("\n")
		->AddByte(enums::TELCMD_IAC)->AddByte(enums::TELCMD_WILL)->AddByte(enums::TELOPT_SGA)
		->AddByte(enums::TELCMD_IAC)->AddByte(enums::TELCMD_WILL)->AddByte(enums::TELOPT_ECHO) // echo off
		->AddByte(enums::TELCMD_IAC)->AddByte(enums::TELCMD_DO)->AddByte(enums::TELOPT_TERM)
		->AddByte(enums::TELCMD_IAC)->AddByte(enums::TELCMD_DO)->AddByte(enums::TELOPT_NAWS)
		;
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::onRecvFirstMessage(LotosPP::Network::NetworkMessage& msg)
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

bool Telnet::connect([[maybe_unused]]uint32_t userId)
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
	if (OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false); output) {
		TRACK_MESSAGE(output);
		output->AddString(message);
		OutputMessagePool::getInstance()->send(output);
		}
	disconnect();
}

//********************** Parse methods *******************************

bool Telnet::parseFirstPacket([[maybe_unused]]LotosPP::Network::NetworkMessage& msg)
{
    LotosPP::Common::User* _user=new LotosPP::Common::User("", this);
	_user->setID();
	_user->stage=LotosPP::Common::enums::UserStage_LOGIN_ID;
	_user->addList();
    addRef();
    return connect(_user->getID());
}

void Telnet::parsePacket(LotosPP::Network::NetworkMessage& msg)
{
	if (!user || !m_acceptPackets || msg.getMessageLength()<=0) {
		return;
		}
	try {
		user->uRead(msg);
		}
	catch (enums::UserStage stg) {
		switch (stg) {
			case enums::UserStage_DISCONNECT:
				user->disconnect();
				[[fallthrough]];
			case enums::UserStage_SWAPPED:
				delete user;
				user=nullptr;
				return;
			default:
				LOG(LERROR) << "INTERNAL ERROR: Caught unexpected user_stage " << stg << "in parsePacket()";
			}
		}
}

//********************** Send methods *******************************

void Telnet::sendEchoOn()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELCMD_IAC)
		->AddByte(enums::TELCMD_WONT)
		->AddByte(enums::TELOPT_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::sendEchoOff()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELCMD_IAC)
		->AddByte(enums::TELCMD_WILL)
		->AddByte(enums::TELOPT_ECHO);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::setXtermTitle(const std::string& title)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033]0;")
		->AddString(title)
		->AddByte(0x07);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::sendTermCoords()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELCMD_IAC)
		->AddByte(enums::TELCMD_DO)
		->AddByte(enums::TELOPT_NAWS);
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
void Telnet::f1() // statline==CHARMODE
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString("\033[?25h\033c\033[?7h"); // Shows the cursor + Reset terminal to initial state + Autowrap on
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::f2()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELCMD_IAC)
		->AddByte(enums::TELCMD_DO)
		->AddByte(TELOPT_NEWENV);
	OutputMessagePool::getInstance()->send(output);
}

void Telnet::f3()
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddByte(enums::TELCMD_IAC)
		->AddByte(enums::TELCMD_WILL)
		->AddByte(TELOPT_NEWENV);
	OutputMessagePool::getInstance()->send(output);
}
*/
