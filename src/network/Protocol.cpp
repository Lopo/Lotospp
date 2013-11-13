#if defined WIN32
#include <winerror.h>
#endif

#include "Lotos2/network/Protocol.h"
#include "Lotos2/Scheduler.h"
#include "Lotos2/network/OutputMessage.h"
#include "Lotos2/network/Connection.h"

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
	//dispather thread
	assert(m_refCount==0);
	setConnection(Connection_ptr());

	delete this;
}
