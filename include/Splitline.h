#ifndef LOTOS2_SPLITLINE_H
#define	LOTOS2_SPLITLINE_H


#include "config.h"

#include <string>
#include <deque>


namespace lotos2 {
	
class Splitline
{
public:
	~Splitline();

	void set(const Splitline* sl);
	void reset();
	template<typename _CharT>
	int parse(const _CharT* line);
	template<typename _CharT>
	void addWord(const _CharT);
	std::string* getWord();
	void shift();

	unsigned int wcnt;
	std::deque<std::string> word;
//	std::string* wordptr=nullptr;

	bool startQuote=false;
	bool endQuote=false;
};
	
} // namespace lotos2

#endif // LOTOS2_SPLITLINE_H
