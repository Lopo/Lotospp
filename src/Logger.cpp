#include <ctime>

#include <boost/date_time.hpp>

#include "Lotos2/Logger.h"
#include "Lotos2/Singleton.h"
#include "Lotos2/globals.h"
#include "Lotos2/misc.h"

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

	switch (type) {
		case LOGTYPE_INFO:
			fprintf(m_file, " info: %s\n", message.c_str());
			break;
		case LOGTYPE_EVENT:
			fprintf(m_file, " event: %s\n", message.c_str());
			break;
		case LOGTYPE_WARNING:
			fprintf(m_file, " warning: %s @ %s\n", message.c_str(), func);
			break;
		case LOGTYPE_ERROR:
			fprintf(m_file, " ERROR: %s @ %s\n", message.c_str(), func);
			break;
		default:
			fprintf(m_file, " ???: %s @ %s\n", message.c_str(), func);
			break;
		}

	fflush(m_file);
}
