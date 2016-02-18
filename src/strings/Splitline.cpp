#include "strings/Splitline.h"

#include <algorithm>

#include <string.h>

#include "generated/enums.h"
#include "strings/stringSplit.h"


using namespace lotospp::strings;


Splitline::~Splitline()
{
	reset();
}

void Splitline::reset()
{
	word.clear();
}

int Splitline::parse(const std::string inLine)
{
	size_t o1=0, o2=0;

	reset();

	if (!inLine.length()) { // Empty line
		return enums::RET_OK;
		}

	line.assign(inLine);

	while (true) {
		if (!(o2=getWord(o1))) {
			break;
			}
		addWord(o1, o2);
		o1=o2;
		}
	return enums::RET_OK;
}

size_t Splitline::getWord(size_t& o1)
{
	size_t o2, prev, end=line.length();

	for ( ; o1<end && line[o1]<33; ++o1)
		;
	if (o1==end) {
		return 0;
		}
	startQuote= line[o1]=='"';
	endQuote=false;

	for (o2= prev= o1+startQuote, o2=o1+startQuote; o2<end; ++o2) {
		if (startQuote) {
			if (line[o2]=='"' && line[prev]!='\\') {
				++o2;
				endQuote=true;
				break;
				}
			}
		else {
			if (line[o2]<33) {
				break;
				}
			}
		prev=o2;
		}
	if (o2==o1 || (startQuote && o2==o1+1)) {
		return 0;
		}
	return o2;
}

void Splitline::addWord(const size_t o1, const size_t o2)
{
	wordPtr.push_back(o1); // Set wordptr to point to the start of the word in the actual string
	if (startQuote) { // Remove quotes
		std::string wrd;
		if (endQuote) {
			wrd=line.substr(o1+1, o2-o1-2);
			}
		else {
			wrd=line.substr(1);
			}
		// Go through string and remove any slashes that are escaping a quote if we have words inside quotes
		size_t found=wrd.find("\\\"");
		while (found!=std::string::npos) {
			wrd.erase(found, 1);
			found=wrd.find("\\\"", found+1);
			}
		word.push_back(wrd);
		}
	else {
		word.push_back(line.substr(o1, o2-o1));
		}
}

void Splitline::shift()
{
	switch (word.size()) {
		case 0:
			return;
		case 1:
			reset();
			return;
		default:
			word.pop_front();
		}
}

std::string Splitline::wordptr(const size_t pos) const
{
	return line.substr(wordPtr[pos]);
}
