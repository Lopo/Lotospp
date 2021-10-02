#include "stringSplit.h"
#include <boost/algorithm/string.hpp>


namespace LotosPP::Strings {

std::vector<std::string>& StringSplit(const std::string& s, const std::string& delim, std::vector<std::string>& elems)
{
	boost::split(elems, s, boost::is_any_of(delim));
	return elems;
}

std::vector<std::string> StringSplit(const std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	StringSplit(s, delim, elems);
	return elems;
}
/*
std::vector<std::string> wordFind(const std::string& inpstr)
{
	std::vector<std::string> ret{};
	std::string word;
	size_t rmax{ret.max_size()}, wmax{word.max_size()};

	std::string::const_iterator it=inpstr.cbegin();
	do {
		word.erase();
		while (*it<33) {
			if (!*it++) {
				return ret;
				}
			}
		while (*it>32 && word.size()<wmax-1) {
			word+=*it++;
			}
		ret.push_back(word);
		} while (ret.size()<rmax);
	return ret;
}
*/
	}
