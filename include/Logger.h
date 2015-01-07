#ifndef LOTOS2_LOGGER_H
#define LOTOS2_LOGGER_H


#include "config.h"

#include <stdio.h>

#include <string>


namespace lotos2 {

#ifdef __GNUC__
#define __LOTOS2_PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif
#ifdef _MSC_VER
#define __LOTOS2_PRETTY_FUNCTION__ __FUNCDNAME__
#endif

#define LOG_MESSAGE(channel, type, message) \
	Logger::getInstance()->logMessage(channel, type, message, __LOTOS2_PRETTY_FUNCTION__);

enum eLogType {
	LOGTYPE_INFO,
	LOGTYPE_EVENT,
	LOGTYPE_WARNING,
	LOGTYPE_ERROR
	};

class Logger
{
public:
	Logger();
	~Logger();
	static Logger* getInstance();

	void logMessage(const char* channel, eLogType type, std::string message, const char* func);
private:
	FILE* m_file;
};

} // namespace lotos2

#endif // LOTOS2_LOGGER_H
