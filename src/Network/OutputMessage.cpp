#include "OutputMessage.h"
#include "Protocol.h"
#include "Connection.h"
#include "globals.h"
#include "Common/Singleton.h"
#include "System/system.h"
#include <iostream>


using namespace LotosPP::Network;
using namespace std;


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
	for (OutputMessage* msg : m_outputMessages) {
		delete msg;
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
	static LotosPP::Common::Singleton<OutputMessagePool> instance;
	return instance.get();
}

void OutputMessagePool::send(OutputMessage_ptr msg)
{
	m_outputPoolLock.lock();
	OutputMessage::OutputMessageState state=msg->getState();
	m_outputPoolLock.unlock();

	if (state==OutputMessage::STATE_ALLOCATED_NO_AUTOSEND) {
#ifdef __DEBUG_NET_DETAIL__
		cout << "Sending message - SINGLE" << endl;
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
			cout << "Error: [OutputMessagePool::send] NULL connection." << endl;
#endif
			}
		}
	else {
#ifdef __DEBUG_NET__
		cout << "Warning: [OutputMessagePool::send] State!=STATE_ALLOCATED_NO_AUTOSEND" << endl;
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
		cout << "Sending message - ALL" << endl;
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
			cout << "Error: [OutputMessagePool::send] NULL connection." << endl;
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
	LotosPP::g_dispatcher.addTask(LotosPP::Common::createTask(boost::bind(&OutputMessagePool::internalReleaseMessage, this, msg)), true);
}

void OutputMessagePool::internalReleaseMessage(OutputMessage* msg)
{
	if (msg->getProtocol()) {
		msg->getProtocol()->unRef();
#ifdef __DEBUG_NET_DETAIL__
		cout << "Removing reference to protocol " << msg->getProtocol() << endl;
#endif
		}
	else {
		cout << "No protocol found." << endl;
		}

	if (msg->getConnection()) {
		msg->getConnection()->unRef();
#ifdef __DEBUG_NET_DETAIL__
		cout << "Removing reference to connection " << msg->getConnection() << endl;
#endif
		}
	else {
		cout << "No connection found." << endl;
		}

	msg->freeMessage();

#ifdef __TRACK_NETWORK__
	msg->clearTrack();
#endif

	m_outputPoolLock.lock();
	m_outputMessages.push_back(msg);
	m_outputPoolLock.unlock();
}

OutputMessage_ptr OutputMessagePool::getOutputMessage(Protocol* protocol, bool autosend/*=true*/)
{
#ifdef __DEBUG_NET_DETAIL__
	cout << "request output message - auto = " << autosend << endl;
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
	outputmessage.reset(m_outputMessages.back(), boost::bind(&OutputMessagePool::releaseMessage, this, boost::placeholders::_1));

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
	cout << "Adding reference to protocol - " << protocol << endl;
#endif
	msg->setConnection(connection);
	connection->addRef();
#ifdef __DEBUG_NET_DETAIL__
	cout << "Adding reference to connection - " << connection << endl;
#endif
	msg->setFrame(m_frameTime);
}

void OutputMessagePool::addToAutoSend(OutputMessage_ptr msg)
{
	m_outputPoolLock.lock();
	m_toAddQueue.push_back(msg);
	m_outputPoolLock.unlock();
}
