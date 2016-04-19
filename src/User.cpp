#include "User.h"

#include <cstdarg>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/algorithm/string.hpp>

#include "define.h"
#include "network/protocol/Telnet.h"
#include "network/OutputMessage.h"
#include "generated/consts.h"
#include "version.h"
#include "strings/stringPrintf.h"
#include "strings/stringSplit.h"
#include "strings/misc.h"
#include "network/Connection.h"
#include "globals.h"
#include "command/Say.h"
#include "command/Quit.h"
#include "IOUser.h"
#include "security/Blowfish.h"
#include "strings/Splitline.h"
#include "Command.h"


using namespace lotospp;


AutoList<User> User::listUser;

#ifdef __ENABLE_SERVER_DIAGNOSTIC__
uint32_t User::userCount=0;
#endif


User::User(const std::string& n /*=""*/, network::Protocol* p/*=nullptr*/)
	: Creature(),
		name(n), client(p)
{
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount++;
#endif

	if (client) {
		client->setUser(this);
		}
}

User::~User()
{
	if (client) {
		client->setUser(nullptr);
		}
	client=nullptr;
	removeList();
#ifdef __ENABLE_SERVER_DIAGNOSTIC__
	userCount--;
#endif
}

void User::addList()
{
	listUser.addList(this);
}

void User::removeList()
{
	listUser.removeList(getID());
}

boost::asio::ip::address User::getAddress() const
{
	if (client) {
		return client->getAddress();
		}
	return boost::asio::ip::address();
}

void User::parseLine()
{
	strings::cleanString(textBuffer[buffnum]);
	com.parse(textBuffer[buffnum]);

	bool skipPrompt=false;
	switch (stage.value()) {
		case enums::UserStage_CMD_LINE:
			runCmdLine();
			skipPrompt=true;
			break;
		default:
			uPrintf("INTERNAL ERROR: Invalid stage in User::parseLine()!\n");
			stage=enums::UserStage_CMD_LINE;
			break;
		}
	if (!skipPrompt) {
		prompt();
		}
// SWAP_BUFF
	buffnum=!buffnum;
}

void User::prompt()
{
	switch (stage.value()) {
		case enums::UserStage_LOGIN_ID:
		case enums::UserStage_LOGIN_NAME:
			uPrintf("Login: ");
			return;
		case enums::UserStage_LOGIN_PWD:
			uPrintf("Enter password: ");
			return;
		case enums::UserStage_LOGIN_NEW_PWD:
			uPrintf("Enter a password: ");
			return;
		case enums::UserStage_LOGIN_REENTER_PWD:
			uPrintf("Re-enter password: ");
			return;
		case enums::UserStage_CMD_LINE:
			uPrintf("%s@%s>\n", name.c_str(), options.get("global.serverName", "").c_str());
			return;
		case enums::UserStage_OLD_PWD:
			uPrintf("Enter old password: ");
			return;
		case enums::UserStage_NEW_PWD:
			uPrintf("Enter new password: ");
			return;
		case enums::UserStage_REENTER_PWD:
			uPrintf("Re-enter new password: ");
			return;
		case enums::UserStage_SUICIDE:
			uPrintf("Enter password: ");
			return;
	}
}

void User::uRead(network::NetworkMessage msg)
{
	size_t remain;
	uint32_t i, len=msg.getMessageLength();
	std::string input=msg.GetRaw();

	for (i=0; i<len; ++i) { // Loop through input
		if (((unsigned char)input[i])==enums::TELCMD_IAC || bpos) {
			// Deal with telnet commands. If a command was incomplete it will have been stored in the buffer
			remain=len-i;
			if (remain+bpos>buff.max_size()) { // If its too long just dump the rest
				remain=buff.max_size();
				}
			buff.replace(bpos, remain, input.substr(i));
			bpos+=remain;
			if (!parseTelopt() || !bpos) { // If incomplete telopt or nothing left then return
				return;
				}
			len=bpos;
			// Copy remains of buffer back into string, reset i & bpos
			input.assign(buff.substr(0, len));
			i= bpos= 0;
			}
		if (stage==enums::UserStage_NEW) {
			continue;
			}
		switch (input[i]) { // Deal with ordinary data
			case '\r':
				textBuffer[buffnum].erase(tbpos);
				inlinePrompt.clear();
				tbpos=0;
				if (flagsTelnet.isSet(enums::TelnetFlag_ECHO) && flagsTelnet.isSet(enums::TelnetFlag_SGA)) {
					uWrite("\r\n");
					}
				if (level==enums::UserLevel_LOGIN) {
					login(textBuffer[buffnum]);
					if (level==enums::UserLevel_LOGIN) {
						prompt();
						}
					}
				else {
					parseLine();
					}
				break;
			case '\0':
			case '\n':
				break;
			case enums::ASCII_BS: // DEL1
			case enums::ASCII_DEL: // DEL2
				if (tbpos>0) {
					tbpos--;
					textBuffer[buffnum].erase(tbpos);
					uWrite("\b \b");
					}
				break;
			default:
				textBuffer[buffnum].replace(tbpos, 1, 1, input[i]);
				tbpos++;
				switch (stage.value()) {
					case enums::UserStage_LOGIN_PWD:
					case enums::UserStage_LOGIN_NEW_PWD:
					case enums::UserStage_LOGIN_REENTER_PWD:
					case enums::UserStage_OLD_PWD:
					case enums::UserStage_NEW_PWD:
					case enums::UserStage_REENTER_PWD:
					case enums::UserStage_SUICIDE:
						uWrite("*");
						break;
					default:
//						if (flagsTelnet.isSet(enums::TelnetFlag_ECHO) && flagsTelnet.isSet(enums::TelnetFlag_SGA)) {
							uWrite(input.substr(i, 1));
//							}
					}
			}
		}
}

void User::uWrite(const std::string& message)
{
	if (client && message.length()) {
		client->write(message);
		}
}

void User::uPrintf(const char* fmtstr, ...)
{
	std::string str, str2;
	size_t i, str2max=str2.max_size();
	bool pcesc=false;
	va_list args;

	va_start(args, fmtstr);
	str=strings::StringPrintV(fmtstr, args);

	for (i=0; str2.length()<str2max && str[i]; ++i) {
		switch (str[i]) {
			case '\n':
				if (pcesc) {
					str2+='/';
					}
				str2+='\r';
				break;
			case '/':
				if (pcesc) {
					str2+='/';
					}
				else {
					pcesc=true;
					}
				continue;
			default:
				if (pcesc) {
					str2+='/';
					}
			}
		str2+=str[i];
		pcesc=false;
		}
	uWrite(str2);
	va_end(args);
}

void User::login(std::string inpstr)
{
	LoginCom loginCom;
	auto it=loginCom.begin();
	size_t len=inpstr.length();

	switch (stage.value()) {
		case enums::UserStage_NEW:
		case enums::UserStage_LOGIN_ID:
		case enums::UserStage_LOGIN_NAME:
			for ( ; it!=loginCom.end(); it++) {
				if (boost::iequals((*it).toString(), inpstr)) {
					break;
					}
				}
			switch ((*it).value()) {
				case enums::LoginCom_QUIT:
					uPrintf("\n\n*** Login abandoned ***\n\n");
					throw enums::UserStage_DISCONNECT;
				case enums::LoginCom_WHO:
					{
					uint32_t i=0;
					User* u=nullptr;
					for (auto uit=User::listUser.list.begin(); uit!=User::listUser.list.end(); uit++) {
						u=(*uit).second;
						if (u==this || u->level==enums::UserLevel_LOGIN) {
							continue;
							}
						++i;
						if (!(i%4)) {
							uPrintf("\n");
							}
						uPrintf("%-*s", 19, u->name.c_str());
						}
					if (!i) {
						uPrintf("no1 :(\n");
						}
					}
					return;
				case enums::LoginCom_VERSION:
					uPrintf("\nLotos++ ");
					uPrintf(LOTOSPP_VERSION_STRING);
					uPrintf("\n");
					return;
				}
			if (inpstr.length()<MIN_USERNAME_LEN) {
				uPrintf("\ntoo short\n\n");
				attempt();
				return;
				}
			if (inpstr.length()>MAX_USERNAME_LEN) {
				uPrintf("\ntoo long\n\n");
				attempt();
				return;
				}
			for (unsigned i=0; i<len; ++i) {
				if (!::isalpha(inpstr[i])) {
					uPrintf("\nletters only\n\n");
					attempt();
					}
				}
			strings::toLowerCaseString(inpstr);
			name.assign(inpstr);
			name[0]=::toupper(name[0]);
			// If user has hung on another login clear that session
			for (auto u=listUser.list.begin(); u!=listUser.list.end(); ++u) {
				if (u->second->level==enums::UserLevel_LOGIN && u->second!=this && boost::iequals(u->second->name, name)) {
					u->second->kick();
					}
				}
			if (!IOUser::instance()->load(this, inpstr, true)) {
				uPrintf("creating new account\n");
				}
//			client->sendEchoOff();
			level=enums::UserLevel_LOGIN;
			stage=enums::UserStage_LOGIN_PWD;
			return;
		case enums::UserStage_LOGIN_PWD:
			if (len<MIN_PASSWORD_LEN) {
				uPrintf("\n\ntoo short\n\n");
				attempt();
				return;
				}
			if (!password || !password->length()) { // new user
				password=new std::string(security::Blowfish::crypt(inpstr));
				uPrintf("\n\nconfirm: ");
				stage=enums::UserStage_LOGIN_REENTER_PWD;
				}
			else {
				if (!password->compare(security::Blowfish::crypt(inpstr, password->c_str()))) {
					IOUser::instance()->load(this, name);
					delete password;
					password=nullptr;
//					client->sendEchoOn();
//					cls();
					stage=enums::UserStage_CMD_LINE;
					uConnect();
					return;
					}
				uPrintf("\n\nwrong password\n\n");
				attempt();
				}
			return;
		case enums::UserStage_LOGIN_REENTER_PWD:
			if (password->compare(security::Blowfish::crypt(inpstr, password->c_str()))) {
				uPrintf("\n\npassword nomatch\n\n");
				attempt();
				return;
				}
			client->sendEchoOn();
			level=enums::UserLevel_NOVICE;
			guid=IOUser::instance()->create(this);
			delete password;
			password=nullptr;
//			cls();
			uWrite("press [ENTER] to login");
			stage=enums::UserStage_LOGIN_PROMPT;
			return;
		}
}

void User::uConnect()
{
	for (auto it=listUser.list.begin(); it!=listUser.list.end(); it++) {
		User* u=it->second;
		if (this!=u && !name.compare(u->name)) {
			uPrintf("\n\nalready logged in - switching to old session ...\n");
			uPrintf("old addr: %s", u->getAddress().to_string().c_str());
			u->uPrintf("\nswapping this session to addr %s\n\n", getAddress().to_string().c_str());
			u->disconnect();
			client->setUser(u);
			u->client=client;
			client=nullptr;
			removeList();
			u->prompt();
			throw enums::UserStage_SWAPPED;
			}
		}
	prompt();
}

void User::attempt()
{
	attempts++;
	if (attempts==MAX_LOGIN_ATTEMPTS) {
		uPrintf("\nmax attempts\n");
		kick();
		removeList();
		return;
		}
	stage=enums::UserStage_LOGIN_NAME;
	if (password) {
		delete password;
		password=nullptr;
		}
}

void User::kick()
{
	if (client) {
		client->logout(true);
		}
	else {
		g_talker.removeCreature(this);
		}
}

/// Parse the telopt stuff by going through the buffer. Returns true if found complete telopt code else returns false
bool User::parseTelopt()
{
	int shift/*, ret*/;
//	std::string path;

	while (bpos && ((unsigned char)buff[0])==enums::TELCMD_IAC) {
		if (bpos<2) {
			return false;
			}
		shift=0;

		switch ((unsigned char)buff[1]) {
			case enums::TELCMD_SB:
				switch (buff[2]) {
					case enums::TELOPT_NAWS:
						if (!(shift=getTermsize())) {
							return false;
							}
						break;
					case enums::TELOPT_TERM:
						if (!(shift=getTermtype())) {
							return false;
							}
						break;
					default:
						uPrintf("WARNING: Your client sent unexpected sub option %d\n", buff[2]);
					}
				break;
			case enums::TELCMD_WILL:
				if (bpos<3) {
					return false;
					}
				switch (buff[2]) {
					case enums::TELOPT_NAWS:
						break;
					case enums::TELOPT_TERM:
						uPrintf("%c%c%c%c%c%c",
								enums::TELCMD_IAC, enums::TELCMD_SB, enums::TELOPT_TERM,
								enums::TELSUB_SEND, enums::TELCMD_IAC, enums::TELCMD_SE);
						break;
					default:
						uPrintf("WARNING: Your client sent unexpected TELNET_WILL\n");
					}
				shift=3;
				break;
			case enums::TELCMD_WONT:
				if (bpos<3) {
					return false;
					}
				switch (buff[2]) {
					case enums::TELOPT_NAWS:
						uPrintf("Unable to get your terminal size, defaulting to %dx%d.\n", termCols, termRows);
						flagsTelnet.set(enums::TelnetFlag_TERMSIZE);
						break;
					case enums::TELOPT_TERM:
						uPrintf("Unable to get your terminal type.\n");
						flagsTelnet.set(enums::TelnetFlag_TERMTYPE);
						termType="<unresolved>";
						break;
					default:
						uPrintf("WARNING: Your client sent unexpected TELNET_WILL\n");
					}
				shift=3;
				break;
			case enums::TELCMD_DO:
				if (bpos<3) {
					return false;
					}
				switch (buff[2]) {
					case enums::TELOPT_SGA:
						flagsTelnet.set(enums::TelnetFlag_SGA);
						break;
					case enums::TELOPT_ECHO:
						flagsTelnet.set(enums::TelnetFlag_ECHO);
						break;
					default:
						uPrintf("WARNING: Your client sent unexpected option %d for TELNET_DO\n", buff[2]);
					}
				shift=3;
				break;
			case enums::TELCMD_DONT:
				if (bpos<3) {
					return false;
					}
				switch (buff[2]) {
					case enums::TELOPT_SGA:
						uPrintf("Your client does not support character mode. This will cause I/O\n         problems with your session.\n");
						flagsTelnet.set(enums::TelnetFlag_SGA);
						break;
					case enums::TELOPT_ECHO:
						uPrintf("WARNING: Your client refused to switch off input echoing.\n");
						// Have to set or check at bottom will never be true
						flagsTelnet.set(enums::TelnetFlag_ECHO);
						break;
					default:
						uPrintf("WARNING: Your client sent unexpected option %d for TELNET_DONT\n", buff[2]);
					}
				shift=3;
				break;
			case enums::TELCMD_AYT:
				uPrintf("[%s : yes]\n", LOTOSPP_NAME);
				shift=2;
				break;
			case enums::TELCMD_IAC:
				uPrintf("%c", (char *)&buff[1]); // Write 255
				// Fall through
			default:
				shift=2;
			}

		bpos-=shift;
		buff.erase(0, shift);
		}

	if (!flagsTelnet.isSet(enums::TelnetFlag_GOT_TELOPT_INFO)
		&& flagsTelnet.isSet(enums::TelnetFlag_ECHO)
		&& flagsTelnet.isSet(enums::TelnetFlag_SGA)
		&& flagsTelnet.isSet(enums::TelnetFlag_TERMTYPE)
		&& flagsTelnet.isSet(enums::TelnetFlag_TERMSIZE)
		) {
		// Got all telopt info so send prelogin screen and print initial prompt 
//		sprintf(path, "%s/%s", ETC_DIR, PRELOGIN_SCREEN);
//		if ((ret=page_file(path, 0))!=enums::RET_OK) {
//			uPrintf("ERROR: cannot page pre-login screen: %s\n\n", err_string[ret]);
//			}
		flagsTelnet.set(enums::TelnetFlag_GOT_TELOPT_INFO);
		stage=enums::UserStage_LOGIN_ID;
		prompt();
		}
	return true;
}

/**
 * Get the terminal size from the telopt sub command
 */
int User::getTermsize()
{
	unsigned int pos1, pos2, def;

	if (bpos<9) {
		return 0;
		}

	uint16_t dCols=termCols, dRows=termRows;
	termCols= termRows= 0;

	// If one of the sizes involves 255 (TELNET_IAC) then 255 gets sent twice as per telnet spec.
	// Numeric order is high byte , low byte.
	pos1=3+(((unsigned char)buff[3])==enums::TELCMD_IAC);
	pos2=pos1+1+(((unsigned char)buff[pos1+1])==enums::TELCMD_IAC);
	if (pos2>bpos) {
		return 0;
		}
	termCols=(((uint16_t)buff[pos1])<<8)+(uint8_t)buff[pos2];

	pos1=pos2+1+(((unsigned char)buff[pos2+1])==enums::TELCMD_IAC);
	pos2=pos1+1+(((unsigned char)buff[pos1+1])==enums::TELCMD_IAC);
	termRows=((uint16_t)buff[pos1]<<8)+(uint8_t)buff[pos2];

	// Make sure we have a full command sequence. If not then use default.
	if (pos2>bpos-3) {
		return 0;
		}
	if (((unsigned char)buff[pos2+1])!=enums::TELCMD_IAC || ((unsigned char)buff[pos2+2])!=enums::TELCMD_SE) {
		uPrintf("WARNING: Your client sent a corrupt terminal size option!\n");
		termCols= termRows= 0;
		}

	// Will be zero if corrupt above or some dumb terminals will cause zeros
	def=(!termCols || !termRows);
	if (!termCols) {
		termCols=dCols;
		}
	if (!termRows) {
		termRows=dRows;
		}

	if (level==enums::UserLevel_LOGIN) {
		if (def) {
			uPrintf("Your terminal has been set to a default size of %dx%d\n", termCols, termRows);
			}
		else {
			uPrintf("Terminal size: %dx%d\n", termCols, termRows);
			}
		flagsTelnet.set(enums::TelnetFlag_TERMSIZE);
		}
	else {
		// We can receive this even when user is connected as telnet will send it when user resizes xterm
		uPrintf("INFO: Terminal size now: %dx%d\n", termCols, termRows);
		}
	return pos2+3;
}

/**
 * Get the terminal type from the telopt sub command. This code doesn't take into account the possibility of an IAC
 * followed by an SE in the terminal name as its unlikely to happen and won't cause a crash even if it does,
 * it'll just mess up the user session with some rubbish following after the prompt.
 */
int User::getTermtype()
{
	if (bpos<6) {
		return 0;
		}
	size_t sh;
	if (((unsigned char)buff[3])!=enums::TELSUB_IS) {
		uPrintf("WARNING: Your client sent a corrupt terminal type option!\n");
		termType.assign("<unresolved>");
		sh=4;
		}
	else {
		size_t i;
		for (i=4; i<bpos; ++i) {
			if (((unsigned char)buff[i])==enums::TELCMD_IAC
				&& i<bpos-1
				&& ((unsigned char)buff[i+1])==enums::TELCMD_SE
				) {
				break;
				}
			}
		if (i==bpos) {
			return 0;
			}

		// Term type could be null if user did 'export TERM=""' on command line.
		if (i==4) {
			termType.assign("<unresolved>");
			}
		else {
			buff.erase(i);
			termType.assign(buff.substr(4));
			}
		sh=i+2;
		}
	// Unset ansi flag in case we've had more than a type option sent before. It shouldn't happen but...
	flagsTelnet.set(enums::TelnetFlag_TERMTYPE);
	flagsTelnet.unset(enums::TelnetFlag_ANSI_TERM);

	// Set colour flag if user has compatable terminal. 
	std::string ansiTerms=options.get("global.ansiTerms", "");
	if (ansiTerms!="") {
		for (std::string term: strings::StringSplit(ansiTerms, ",")) {
			if (boost::iequals(termType, term)) {
				flagsTelnet.set(enums::TelnetFlag_ANSI_TERM);
				break;
				}
			}
		}
	uPrintf("Terminal type: %s\n", termType.c_str());

	if (!flagsTelnet.isSet(enums::TelnetFlag_ANSI_TERM)) {
		uPrintf("WARNING: Your terminal is not recognised as ANSI code compatable.\n");
		}

	return sh;
}

void User::runCmdLine()
{
	bool dot=false;

	// If nothing entered just return. Word[0] could be zero length if user entered "" on the command line
	if (com.word.empty() || !com.word[0].length()) {
		return;
		}
	if (com.word[0][0]=='.') { // Check for dot command
		if (com.word[0].length()==1) {
			uPrintf("Missing command.\n");
			prompt();
			return;
			}
		dot=true;
		}

	std::string w=com.word[0].substr(dot);
	if (boost::iequals(w, "say")) {
		Command* cmd=new command::Say;
		cmd->execute(this);
		prompt();
		}
	else if (boost::iequals(w, "quit")) {
		Command* cmd=new command::Quit;
		cmd->execute(this);
		}
	else {
		uPrintf("Unknown command.\n");
		}
}
