#include "config.h"

#include <cstdint>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/bind.hpp>

#include "network/OutputMessage.h"
#include "globals.h"
#include "network/Connection.h"
#include "network/Protocol.h"
#include "Singleton.h"
#include "system/system.h"


using namespace lotos2::network;


#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t OutputMessagePool::OutputMessagePoolCount=OUTPUT_POOL_SIZE;
#endif

OutputMessage::OutputMessage()
{
	freeMessage();
}

OutputMessage::~OutputMessage()
{
}

char* OutputMessage::getOutputBuffer()
{
	return (char*)&m_MsgBuf[m_outputBufferStart];
}

Protocol* OutputMessage::getProtocol()
{
	return m_protocol;
}

Connection_ptr OutputMessage::getConnection()
{
	return m_connection;
}

uint64_t OutputMessage::getFrame() const
{
	return m_frame;
}

void OutputMessage::freeMessage()
{
	setConnection(Connection_ptr());
	setProtocol(nullptr);
	m_frame=0;
	m_outputBufferStart=0;

	//setState have to be the last one
	setState(OutputMessage::STATE_FREE);
}

void OutputMessage::setProtocol(Protocol* protocol)
{
	m_protocol=protocol;
}

void OutputMessage::setConnection(Connection_ptr connection)
{
	m_connection=connection;
}

void OutputMessage::setState(OutputMessageState state)
{
	m_state=state;
}

OutputMessage::OutputMessageState OutputMessage::getState() const
{
	return m_state;
}

void OutputMessage::setFrame(uint64_t frame)
{
	m_frame=frame;
}

//*********** OutputMessagePool ****************

OutputMessagePool::OutputMessagePool()
{
	for (uint32_t i=0; i<OUTPUT_POOL_SIZE; ++i) {
		OutputMessage* msg=new OutputMessage();
		m_outputMessages.push_back(msg);
#ifdef __TRACK_NETWORK__
		m_allOutputMessages.push_back(msg);
#endif
		}
	m_frameTime=SYS_TIME();
}

OutputMessagePool::~OutputMessagePool()
{
	InternalOutputMessageList::iterator it;
	for (it=m_outputMessages.begin(); it!=m_outputMessages.end(); ++it) {
		delete *it;
		}
	m_outputMessages.clear();
}

void OutputMessagePool::startExecutionFrame()
{
	m_frameTime=SYS_TIME();
	m_isOpen=true;
}

size_t OutputMessagePool::getTotalMessageCount() const
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	return OutputMessagePoolCount;
#else
	return m_allOutputMessages.size();
#endif
}

size_t OutputMessagePool::getAvailableMessageCount() const
{
	return m_outputMessages.size();
}

size_t OutputMessagePool::getAutoMessageCount() const
{
	return m_autoSendOutputMessages.size();
}

OutputMessagePool* OutputMessagePool::getInstance()
{
	static Singleton<OutputMessagePool> instance;
	return instance.get();
}

void OutputMessagePool::send(OutputMessage_ptr msg)
{
	m_outputPoolLock.lock();
	OutputMessage::OutputMessageState state=msg->getState();
	m_outputPoolLock.unlock();

	if (state==OutputMessage::STATE_ALLOCATED_NO_AUTOSEND) {
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Sending message - SINGLE" << std::endl;
#endif
		if (msg->getConnection()) {
			if (!msg->getConnection()->send(msg)) {
				// Send only fails when connection is closing (or in error state)
				// This call will free the message
				msg->getProtocol()->onSendMessage(msg);
				}
			}
		else {
#ifdef __DEBUG_NET__
			std::cout << "Error: [OutputMessagePool::send] NULL connection." << std::endl;
#endif
			}
		}
	else {
#ifdef __DEBUG_NET__
		std::cout << "Warning: [OutputMessagePool::send] State!=STATE_ALLOCATED_NO_AUTOSEND" << std::endl;
#endif
		}
}

void OutputMessagePool::sendAll()
{
	boost::recursive_mutex::scoped_lock lockClass(m_outputPoolLock);
	OutputMessageMessageList::iterator it;

	for (it=m_toAddQueue.begin(); it!=m_toAddQueue.end(); ) {
		//drop messages that are older than Connection::read_timeout seconds
		if (SYS_TIME()-(*it)->getFrame()>Connection::read_timeout*1000) {
			(*it)->getProtocol()->onSendMessage(*it);
			it=m_toAddQueue.erase(it);
			continue;
			}

		(*it)->setState(OutputMessage::STATE_ALLOCATED);
		m_autoSendOutputMessages.push_back(*it);
		++it;
		}

	m_toAddQueue.clear();

	for (it=m_autoSendOutputMessages.begin(); it!=m_autoSendOutputMessages.end(); ) {
		OutputMessage_ptr omsg=*it;
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Sending message - ALL" << std::endl;
#endif

		if (omsg->getConnection()) {
			if (!omsg->getConnection()->send(omsg)) {
				// Send only fails when connection is closing (or in error state)
				// This call will free the message
				omsg->getProtocol()->onSendMessage(omsg);
				}
			}
		else {
#ifdef __DEBUG_NET__
			std::cout << "Error: [OutputMessagePool::send] NULL connection." << std::endl;
#endif
			}

		it=m_autoSendOutputMessages.erase(it);
		}
}

void OutputMessagePool::stop()
{
	m_isOpen=false;
}

void OutputMessagePool::releaseMessage(OutputMessage* msg)
{
	g_dispatcher.addTask(createTask(boost::bind(&OutputMessagePool::internalReleaseMessage, this, msg)), true);
}

void OutputMessagePool::internalReleaseMessage(OutputMessage* msg)
{
	if (msg->getProtocol()) {
		msg->getProtocol()->unRef();
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Removing reference to protocol " << msg->getProtocol() << std::endl;
#endif
		}
	else {
		std::cout << "No protocol found." << std::endl;
		}

	if (msg->getConnection()) {
		msg->getConnection()->unRef();
#ifdef __DEBUG_NET_DETAIL__
		std::cout << "Removing reference to connection " << msg->getConnection() << std::endl;
#endif
		}
	else {
		std::cout << "No connection found." << std::endl;
		}

	msg->freeMessage();

#ifdef __TRACK_NETWORK__
	msg->clearTrack();
#endif

	m_outputPoolLock.lock();
	m_outputMessages.push_back(msg);
	m_outputPoolLock.unlock();
}

OutputMessage_ptr OutputMessagePool::getOutputMessage(Protocol* protocol, bool autosend /*= true*/)
{
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "request output message - auto = " << autosend << std::endl;
#endif

	if (!m_isOpen) {
		return OutputMessage_ptr();
		}

	boost::recursive_mutex::scoped_lock lockClass(m_outputPoolLock);

	if (protocol->getConnection()==nullptr) {
		return OutputMessage_ptr();
		}

	if (m_outputMessages.empty()) {
		OutputMessage* msg=new OutputMessage();
		m_outputMessages.push_back(msg);

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	OutputMessagePoolCount++;
#endif

#ifdef __TRACK_NETWORK__
		m_allOutputMessages.push_back(msg);
#endif
		}

	OutputMessage_ptr outputmessage;
	outputmessage.reset(m_outputMessages.back(), boost::bind(&OutputMessagePool::releaseMessage, this, _1));

	m_outputMessages.pop_back();

	configureOutputMessage(outputmessage, protocol, autosend);
	return outputmessage;
}

void OutputMessagePool::configureOutputMessage(OutputMessage_ptr msg, Protocol* protocol, bool autosend)
{
	TRACK_MESSAGE(msg);
	msg->Reset();
	if (autosend) {
		msg->setState(OutputMessage::STATE_ALLOCATED);
		m_autoSendOutputMessages.push_back(msg);
		}
	else {
		msg->setState(OutputMessage::STATE_ALLOCATED_NO_AUTOSEND);
		}

	Connection_ptr connection=protocol->getConnection();
	assert(connection!=nullptr);

	msg->setProtocol(protocol);
	protocol->addRef();
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Adding reference to protocol - " << protocol << std::endl;
#endif
	msg->setConnection(connection);
	connection->addRef();
#ifdef __DEBUG_NET_DETAIL__
	std::cout << "Adding reference to connection - " << connection << std::endl;
#endif
	msg->setFrame(m_frameTime);
}

void OutputMessagePool::addToAutoSend(OutputMessage_ptr msg)
{
	m_outputPoolLock.lock();
	m_toAddQueue.push_back(msg);
	m_outputPoolLock.unlock();
}
