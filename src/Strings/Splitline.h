#ifndef LOTOSPP_STRINGS_SPLITLINE_H
#define	LOTOSPP_STRINGS_SPLITLINE_H

#include "Common/Enums/ReturnValue.h"
#include <string>
#include <deque>


namespace LotosPP::Strings {

/**
 * A splitline object takes a line of text and splits it into words
 */
class Splitline
{
public:
	~Splitline();

	void reset();
	LotosPP::Common::ReturnValue parse(const std::string& inLine);
	/// Shift all words up by one. This is equivalent to the unix shell shift command
	void shift();

	std::string wordptr(const size_t pos) const;

	bool startQuote{false};
	bool endQuote{false};
	std::deque<std::string> word{};

private:
	/// Get an individual word either in or not in quotes
	size_t getWord(size_t& o1);
	/// Add a word into the list
	void addWord(const size_t o1, const size_t o2);

	std::string line;
	std::deque<size_t> wordPtr{};
};
	
	}

#endif
