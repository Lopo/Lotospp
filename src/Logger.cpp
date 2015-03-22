#include "config.h"

#include <cstdio>
#include <ctime>
#include <string>

#include <boost/date_time.hpp>

#include "Logger.h"
#include "Singleton.h"
#include "globals.h"
#include "misc.h"


using namespace lotos2;


Logger::Logger()
{
	if (!(m_file=fopen(options.get("global.logFile", "").c_str(), "a"))) {
		m_file=stdout;
		}
}

Logger::~Logger()
{
	if (m_file) {
		fclose(m_file);
		}
}

Logger* Logger::getInstance()
{
	static Singleton<Logger> instance;
	return instance.get();
}

void Logger::logMessage(const char* channel, eLogType type, std::string message, const char* func)
{
	//write timestamp of the event
	char buffer[32];
	time_t tmp=time(NULL);
	formatTime(tmp, buffer);
	fputs(buffer, m_file);
	//write channel generating the message
	if (channel) {
		fprintf(m_file, " [%s] ", channel);
		}

	//write message type
	const char* type_str;
	switch (type) {
		case LOGTYPE_INFO:
			type_str="info";
			break;
		case LOGTYPE_EVENT:
			type_str="event";
			break;
		case LOGTYPE_WARNING:
			type_str="warning";
			break;
		case LOGTYPE_ERROR:
			type_str="ERROR";
			break;
		default:
			type_str="???";
			break;
		}
	fprintf(m_file, " %s:", type_str);
	//write the message
	fprintf(m_file, " %s\n", message.c_str());

	fflush(m_file);
}
