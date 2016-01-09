#include "network/NetworkMessage.h"

#include <cstring>


using namespace lotospp::network;


NetworkMessage::NetworkMessage()
{
	Reset();
}

NetworkMessage::~NetworkMessage()
{
}

uint8_t NetworkMessage::GetByte()
{
	return m_MsgBuf[m_ReadPos++];
}

uint16_t NetworkMessage::GetU16()
{
	uint16_t v=*(uint16_t*)(m_MsgBuf+m_ReadPos);
	m_ReadPos+=2;
	return v;
}

uint32_t NetworkMessage::GetU32()
{
	uint32_t v=*(uint32_t*)(m_MsgBuf+m_ReadPos);
	m_ReadPos+=4;
	return v;
}

uint32_t NetworkMessage::PeekU32() const
{
	uint32_t v=*(uint32_t*)(m_MsgBuf+m_ReadPos);
	return v;
}

uint64_t NetworkMessage::GetU64() const
{
	uint64_t v=*(uint64_t*)(m_MsgBuf+m_ReadPos);
	return v;
}

std::string NetworkMessage::GetString()
{
	std::string raw=GetRaw();
	std::string::size_type p;
	if ((p=raw.find('\0'))==raw.npos) {
		return raw;
		}
	return std::string(raw, p);
}

std::string NetworkMessage::GetRaw()
{
	if (m_MsgSize>=NETWORKMESSAGE_MAXSIZE) {
		return std::string();
		}

	uint16_t stringlen=m_MsgSize-m_ReadPos;
	char* v=(char*)(m_MsgBuf+m_ReadPos);
	m_ReadPos+=stringlen;
	return std::string(v, stringlen);
}

NetworkMessage* NetworkMessage::SkipBytes(int count)
{
	m_ReadPos+=count;
	return this;
}

NetworkMessage* NetworkMessage::AddByte(uint8_t value)
{
	if (!canAdd(1)) {
		return this;
		}
	m_MsgBuf[m_ReadPos++]=value;
	m_MsgSize++;
	return this;
}

NetworkMessage* NetworkMessage::AddU16(uint16_t value)
{
	if (!canAdd(2)) {
		return this;
		}
	*(uint16_t*)(m_MsgBuf+m_ReadPos)=value;
	m_ReadPos+=2;
	m_MsgSize+=2;
	return this;
}

NetworkMessage* NetworkMessage::AddU32(uint32_t value)
{
	if (!canAdd(4)) {
		return this;
		}
	*(uint32_t*)(m_MsgBuf+m_ReadPos)=value;
	m_ReadPos+=4;
	m_MsgSize+=4;
	return this;
}

NetworkMessage* NetworkMessage::AddU64(uint64_t value)
{
	if (!canAdd(8)) {
		return this;
		}
	*(uint64_t*)(m_MsgBuf+m_ReadPos)=value;
	m_ReadPos+=8;
	m_MsgSize+=8;
	return this;
}

NetworkMessage* NetworkMessage::AddString(const char* value)
{
	uint32_t stringlen=(uint32_t)strlen(value);
	if (!canAdd(stringlen+1) || stringlen>8192) {
		return this;
		}

	strcpy((char*)(m_MsgBuf+m_ReadPos), value);
	m_ReadPos+=stringlen;
	m_MsgSize+=stringlen;
	AddByte('\0');
	return this;
}

NetworkMessage* NetworkMessage::AddBytes(const char* bytes, uint32_t size)
{
	if (!canAdd(size) || size>8192) {
		return this;
		}

	memcpy(m_MsgBuf+m_ReadPos, bytes, size);
	m_ReadPos+=size;
	m_MsgSize+=size;
	return this;
}

NetworkMessage* NetworkMessage::AddString(const std::string &value)
{
	AddString(value.c_str());
	return this;
}

int32_t NetworkMessage::getMessageLength() const
{
	return m_MsgSize;
}

void NetworkMessage::setMessageLength(int32_t newSize)
{
	m_MsgSize=newSize;
}

int32_t NetworkMessage::getReadPos() const
{
	return m_ReadPos;
}

void NetworkMessage::setReadPos(int32_t pos)
{
	m_ReadPos=pos;
}

char* NetworkMessage::getBuffer()
{
	return (char*)&m_MsgBuf[0];
}

void NetworkMessage::Reset()
{
	m_MsgSize=0;
	m_ReadPos=0;
}

bool NetworkMessage::canAdd(uint32_t size) const
{
	return size+m_ReadPos<max_body_length;
}

uint8_t NetworkMessage::GetAt(uint32_t pos)
{
	return m_MsgBuf[pos];
}
