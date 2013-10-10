#include <stdarg.h>
#include <stdio.h>
#include <pthread.h>

#include <string>

#include <boost/algorithm/string.hpp>

#include "config.h"
#include "Lotos2/globals.h"

using std::string;
using std::cout;

/*** Returns true if string has whitespace ***/
int has_whitespace(const char *str)
{
	for (; *str; ++str) {
		if (*str<33) {
			return 1;
			}
		}
	return 0;
}

/* Write out some info with the time prepended. We don't keep an open file descriptor for the log file
 * since if its accidentaly deleted then unix won't recreate the file via an open descriptor to the old file.
 * Opening and closing it (though inefficient) avoids this issue. ***/
void log(const char *fmtstr, ...)
{
	int arr_size=3000;
	char str[arr_size];
	char tstr[20];
	va_list args;
	FILE *fp;

	va_start(args, fmtstr);
	vsnprintf(str, arr_size-20, fmtstr, args);
	va_end(args);

	strftime(tstr, sizeof (tstr), "%Y%m%dT%H%M%S ", &serverTimeTms);

	// If printing to stdout ...
	string logFile(options.get("global.logFile", ""));
	if (logFile=="") {
		cout << tstr << str;
		if (str[strlen(str)-1]!='\n')
			cout << std::endl;
		return;
		}

	/* Use a mutex here so different threads don't write over each others data in the file.
	 * I've no idea if this would actually happen , I guess it depends how write() is implemented by the OS,
	 * but I'm not taking any chances. */
	pthread_mutex_lock(&log_mutex);

	// If this doesn't work where do we print out the error?? We can't , so just return. 
	if (!(fp = fopen(logFile.c_str(), "a"))) {
		pthread_mutex_unlock(&log_mutex);
		return;
		}

	if (str[strlen(str)-1]=='\n') {
		fprintf(fp, "%s%s", tstr, str);
		}
	else {
		fprintf(fp, "%s%s\n", tstr, str);
		}
	fclose(fp);

	pthread_mutex_unlock(&log_mutex);
}
