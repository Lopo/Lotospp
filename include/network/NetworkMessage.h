#ifndef LOTOS2_NETWORK_NETWORKMESSAGE_H
#define LOTOS2_NETWORK_NETWORKMESSAGE_H


#include "config.h"

#include <cstdint>
#include <string>

#include <boost/shared_ptr.hpp>

#include "network/ProtocolConst.h"


namespace lotos2 {
	namespace network {

class NetworkMessage
{
public:
	enum { max_body_length=NETWORKMESSAGE_MAXSIZE};

	// constructor/destructor
	NetworkMessage();
	virtual ~NetworkMessage();

	// simply read functions for incoming message
	uint8_t GetByte();
	uint16_t GetU16();
	uint32_t GetU32();
	uint32_t PeekU32() const;
	uint64_t GetU64() const;
	std::string GetString();
	std::string GetRaw();
	uint8_t GetAt(uint32_t pos);
	uint8_t operator[](uint32_t pos) { return GetAt(pos);};

	// skips count unknown/unused bytes in an incoming message
	void SkipBytes(int count);

	// simply write functions for outgoing message
	void AddByte(uint8_t value);
	void AddU16(uint16_t value);
	void AddU32(uint32_t value);
	void AddU64(uint64_t value);
	void AddBytes(const char* bytes, uint32_t size);

	void AddString(const std::string &value);
	void AddString(const char* value);

	int32_t getMessageLength() const;
	void setMessageLength(int32_t newSize);
	int32_t getReadPos() const;
	void setReadPos(int32_t pos);

	char* getBuffer();

#ifdef __TRACK_NETWORK__
	virtual void Track(std::string file, long line, std::string func) {};
	virtual void clearTrack() {};
#endif

protected:
	void Reset();
	bool canAdd(uint32_t size) const;

	int32_t m_MsgSize;
	int32_t m_ReadPos;

	uint8_t m_MsgBuf[NETWORKMESSAGE_MAXSIZE];
};

typedef boost::shared_ptr<NetworkMessage> NetworkMessage_ptr;

	} // namespace network
} // namespace lotos2

#endif // LOTOS2_NETWORK_NETWORKMESSAGE_H
