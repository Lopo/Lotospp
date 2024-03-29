#include "Protocol.h"
#include "globals.h"
#include "OutputMessage.h"
#include "Common/User.h"
#include "Common/Scheduler.h"
#ifdef __DEBUG_NET_DETAIL__
#	include <iostream>
#endif
#include <cassert>
#ifdef OS_WIN
#	include <winerror.h>
#endif


using namespace LotosPP::Network;
using namespace std;


void Protocol::onSendMessage(OutputMessage_ptr msg)
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "Protocol::onSendMessage" << endl;
#endif
	if (msg==m_outputBuffer) {
		m_outputBuffer.reset();
		}
}

void Protocol::onRecvMessage(NetworkMessage& msg)
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "Protocol::onRecvMessage" << endl;
#endif
	parsePacket(msg);
}

OutputMessage_ptr Protocol::getOutputBuffer()
{
	if (m_outputBuffer && m_outputBuffer->getMessageLength()<NetworkMessage::MAXSIZE-4096) {
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
		g_scheduler.addEvent(LotosPP::Common::createSchedulerTask(LotosPP::Common::SchedulerTask::SCHEDULER_MINTICKS, boost::bind(&Protocol::releaseProtocol, this)));
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

u_short Protocol::getPort() const
{
	if (getConnection()) {
		return getConnection()->getPort();
		}

	return 0;
}

std::string Protocol::getHostname() const
{
	if (getConnection()) {
		return getConnection()->getHostname();
		}

	return string();
}

void Protocol::setUser(LotosPP::Common::User* u)
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
	size_t pos{msg.getReadPos()},
		dataLength{msg.getMessageLength()};
	if (dataLength) {
		printf("data: ");
		int data{msg.GetByte()};
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

bool Protocol::logout([[maybe_unused]]bool forced)
{
	if (!user) {
		return false;
		}

	if (Connection_ptr connection=getConnection()) {
		connection->closeConnection();
		}

	return g_talker.removeCreature(user);
}
