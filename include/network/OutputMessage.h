#ifndef LOTOSPP_NETWORK_OUTPUTMESSAGE_H
#define LOTOSPP_NETWORK_OUTPUTMESSAGE_H


#include "config.h"

#include <cstdint>
#include <list>
#ifdef __TRACK_NETWORK__
#	include <string>
#	include <iostream>
#endif

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include "network/NetworkMessage.h"


namespace lotospp {
	namespace network {
		class Connection;
		typedef boost::shared_ptr<Connection> Connection_ptr;
		class Protocol;

#define OUTPUT_POOL_SIZE 100

class OutputMessage
	: public NetworkMessage,
		boost::noncopyable
{
	friend class OutputMessagePool;

	OutputMessage();

public:
	~OutputMessage();

	char* getOutputBuffer();

	enum OutputMessageState {
		STATE_FREE,
		STATE_ALLOCATED,
		STATE_ALLOCATED_NO_AUTOSEND,
		STATE_WAITING
		};

	Protocol* getProtocol();
	Connection_ptr getConnection();
	uint64_t getFrame() const;

#ifdef __TRACK_NETWORK__
	virtual void Track(std::string file, long line, std::string func)
	{
		if (last_uses.size()>=25) {
			last_uses.pop_front();
			}
		std::ostringstream os;
		os << file << ":" "line " << line << " " << func;
		last_uses.push_back(os.str());
	};

	virtual void clearTrack()
	{
		last_uses.clear();
	};

	void PrintTrace()
	{
		int n=1;
		for (std::list<std::string>::const_reverse_iterator iter=last_uses.rbegin(); iter!=last_uses.rend(); ++iter, ++n) {
			std::cout << "\t" << n << ".\t" << *iter << std::endl;
			}
	};
#endif

protected:
#ifdef __TRACK_NETWORK__
	std::list<std::string> last_uses;
#endif
	void freeMessage();

	void setProtocol(Protocol* protocol);
	void setConnection(Connection_ptr connection);

	void setState(OutputMessageState state);
	OutputMessageState getState() const;

	void setFrame(uint64_t frame);

	Protocol* m_protocol;
	Connection_ptr m_connection;

	uint32_t m_outputBufferStart;
	uint64_t m_frame;

	OutputMessageState m_state;
};

typedef boost::shared_ptr<OutputMessage> OutputMessage_ptr;

class OutputMessagePool
{
public:
	OutputMessagePool();
	~OutputMessagePool();

	static OutputMessagePool* getInstance();

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	static uint32_t OutputMessagePoolCount;
#endif

	void send(OutputMessage_ptr msg);
	void sendAll();
	void stop();
	OutputMessage_ptr getOutputMessage(Protocol* protocol, bool autosend=true);
	void startExecutionFrame();

	size_t getTotalMessageCount() const;
	size_t getAvailableMessageCount() const;
	size_t getAutoMessageCount() const;
	void addToAutoSend(OutputMessage_ptr msg);

protected:
	void configureOutputMessage(OutputMessage_ptr msg, Protocol* protocol, bool autosend);
	void releaseMessage(OutputMessage* msg);
	void internalReleaseMessage(OutputMessage* msg);

	typedef std::list<OutputMessage*> InternalOutputMessageList;
	typedef std::list<OutputMessage_ptr> OutputMessageMessageList;

	InternalOutputMessageList m_outputMessages;
	InternalOutputMessageList m_allOutputMessages;
	OutputMessageMessageList m_autoSendOutputMessages;
	OutputMessageMessageList m_toAddQueue;
	boost::recursive_mutex m_outputPoolLock;
	uint64_t m_frameTime;
	bool m_isOpen;
};

#ifdef __TRACK_NETWORK__
#	define TRACK_MESSAGE(omsg) (omsg)->Track(__FILE__, __LINE__, __FUNCTION__)
//#define TRACK_MESSAGE(omsg) std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << std::endl
#else
#	define TRACK_MESSAGE(omsg)
#endif

	} // namespace network
} // namespace lotospp

#endif // LOTOSPP_NETWORK_OUTPUTMESSAGE_H
