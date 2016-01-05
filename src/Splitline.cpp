#include "config.h"

#include <algorithm>

#include "Splitline.h"
#include "generated/enums.h"
#include "strings/stringsplit.h"


using namespace lotospp;


Splitline::~Splitline()
{
	reset();
}

void Splitline::set(const Splitline* sl)
{
	reset();
	if (sl->wcnt) {
		wcnt=sl->wcnt;
//		wordptr=new std::string(sl->wordptr);
		word=sl->word;
		}
}

void Splitline::reset()
{
	if (wcnt) {
		word.clear();
		wcnt=0;
//		delete wordptr;
//		wordptr=nullptr;
		}
}

template<typename _CharT>
int Splitline::parse(const _CharT * line)
{
	reset();
	if (!line) {
		return enums::RET_OK;
		}
	std::vector<std::string> lv=strings::StringSplit(line, ',');
	std::copy(lv.begin(), lv.end(), std::back_inserter(word));
	return enums::RET_OK;
}

template<typename _CharT>
void Splitline::addWord(const _CharT str)
{
	if (startQuote) {
		++str;
		if (endQuote) {
			
			}
		}
//	word.push_back();
}

void Splitline::shift()
{
	switch (wcnt) {
		case 0:
			return;
		case 1:
			reset();
			return;
		default:
			word.pop_front();
			wcnt--;
		}
}
