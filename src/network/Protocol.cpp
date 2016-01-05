#include "config.h"

#if defined WIN32 || defined __WINDOWS__
#	include <winerror.h>
#endif

#include <cassert>
#ifdef __DEBUG_NET_DETAIL__
#	include <iostream>
#endif

#include <boost/bind.hpp>
#include <boost/asio/ip/address.hpp>

#include "network/Protocol.h"
#include "globals.h"
#include "network/OutputMessage.h"
#include "network/Connection.h"
#include "User.h"


using namespace lotospp::network;


void Protocol::onSendMessage(OutputMessage_ptr msg)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Protocol::onSendMessage" << std::endl;
#endif
	if (msg==m_outputBuffer) {
		m_outputBuffer.reset();
		}
}

void Protocol::onRecvMessage(NetworkMessage& msg)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Protocol::onRecvMessage" << std::endl;
#endif
	parsePacket(msg);
}

OutputMessage_ptr Protocol::getOutputBuffer()
{
	if (m_outputBuffer && m_outputBuffer->getMessageLength()<NETWORKMESSAGE_MAXSIZE-4096) {
		return m_outputBuffer;
		}
	if (m_connection) {
		m_outputBuffer=OutputMessagePool::getInstance()->getOutputMessage(this);
		return m_outputBuffer;
		}
	return OutputMessage_ptr();
}

void Protocol::releaseProtocol()
{
	if (m_refCount>0) {
		//Reschedule it and try again.
		g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, boost::bind(&Protocol::releaseProtocol, this)));
		}
	else {
		deleteProtocolTask();
		}
}

void Protocol::deleteProtocolTask()
{
	//dispatcher thread
	assert(m_refCount==0);
	setConnection(Connection_ptr());

	delete this;
}

void Protocol::disconnect()
{
	if (getConnection()) {
		getConnection()->closeConnection();
		}
}

boost::asio::ip::address Protocol::getAddress() const
{
	if (getConnection()) {
		return getConnection()->getAddress();
		}

	return boost::asio::ip::address();
}

void Protocol::setUser(lotospp::User* u)
{
	user=u;
	if (user) {
		user->addRef();
		}
}

void Protocol::write(const std::string& str)
{
	OutputMessage_ptr output=OutputMessagePool::getInstance()->getOutputMessage(this, false);
	output->AddString(str);
//	OutputMessagePool::getInstance()->send(output);
	getConnection()->send(output);
}

void Protocol::parseDebug(NetworkMessage& msg)
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

bool Protocol::logout(bool forced)
{
	if (!user) {
		return false;
		}

	if (Connection_ptr connection=getConnection()) {
		connection->closeConnection();
		}

	return g_talker.removeCreature(user);
}
