#include <cstdint>
#include <string>
#include <cstdarg>

#include <boost/thread/recursive_mutex.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/algorithm/string.hpp>

#include "define.h"
#include "User.h"
#include "network/protocol/Telnet.h"
#include "network/OutputMessage.h"
#include "generated/consts.h"
#include "generated/enums.h"
#include "version.h"
#include "strings/stringPrintf.h"
#include "strings/stringSplit.h"
#include "misc.h"
#include "network/Connection.h"
#include "globals.h"
#include "command/Say.h"
#include "command/Quit.h"
#include "IOUser.h"
#include "security/crypt_blowfish.h"


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
	prompt();
}

void User::prompt()
{
	uWrite("\n>");
}

void User::uRead(network::NetworkMessage msg)
{
	if (msg[0]==enums::TELNET_IAC) { // ignore control code replies
		return;
		}
//	size_t remain;
//	int32_t i, len=msg.getMessageLength();
	std::string input=msg.GetRaw();

	if (input.back()>=32 || buffpos) {
		if (!getCharclientLine(input)) {
			return;
			}
		}
	else {
		lotospp::terminate(input);
		}
// GOT_LINE
	buff.erase();
	buffpos=0;
	if (level==enums::UserLevel_LOGIN) {
		login(input);
		return;
		}
	word.clear();
	word=strings::wordFind(input);
	if (!word.size()) {
		prompt();
		return;
		}
	if (std::string(".>;:</&![@'*+-,?#").find(input[0])!=std::string::npos) {
		execCommand(input);
		}
	else {
		auto cmd=new command::Say;
		cmd->execute(this, input);
		}
	return;
/*
	for (i=0; i<len; ++i) {
		if (bpos) {
			remain=len-i;
			if (remain+bpos>buff.max_size()) {
				remain=buff.max_size();
				}
			buff.replace(bpos, remain, input.substr(i));
			bpos+=remain;
			if (!bpos) {
				return;
				}
			len=bpos;
			input=buff.substr(0, len);
			i= bpos= 0;
			}
//		if (stage==enums::UserStage_NEW) {
//			continue;
//			}
		switch (input[i]) {
			case '\r':
				*(tbuff+tbpos)='\0';
				inlinePrompt.clear();
				tbpos=0;
				uWrite("\r\n");
				if (level==enums::UserLevel_LOGIN) {
					login(input);
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
			case 8:
			case 127:
				if (tbpos>0) {
					tbpos--;
					*(tbuff+tbpos)='\0';
					uWrite("\b \b");
					}
				break;
			default:
				*(tbuff+tbpos)=input[i];
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
						uWrite(strings::StringPrintf("%c", input[i]));
					}
			}
		}
*/
}

void User::uWrite(const std::string& message)
{
	if (client && message.length()) {
		client->write(message);
		}
}

void User::uPrintf(const std::string& fmtstr, ...)
{
	std::string str, str2;
	size_t i, str2max=str2.max_size();
	bool pcesc=false;
	va_list args;

	va_start(args, fmtstr);
	str=strings::StringPrintV(fmtstr.c_str(), args);

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
			if (inpstr[0]<33) {
				uWrite("login: ");
				return;
				}
			for ( ; it!=loginCom.end(); it++) {
				if (boost::iequals((*it).toString(), inpstr)) {
					break;
					}
				}
			switch ((*it).value()) {
				case enums::LoginCom_QUIT:
					uWrite("\n\n*** Login abandoned ***\n\n");
					throw enums::UserStage_DISCONNECT;
				case enums::LoginCom_WHO:
					uWrite("\ndisabled\n");
					uWrite("login: ");
					return;
				case enums::LoginCom_VERSION:
					uWrite("\nLotos++ ");
					uWrite(LOTOSPP_VERSION_STRING);
					uWrite("\n");
					uWrite("login: ");
					return;
				}
			if (inpstr.length()<MIN_USERNAME_LEN) {
				uWrite("\ntoo short\n\n");
				attempt();
				return;
				}
			if (inpstr.length()>MAX_USERNAME_LEN) {
				uWrite("\ntoo long\n\n");
				attempt();
				return;
				}
			for (unsigned i=0; i<len; ++i) {
				if (!::isalpha(inpstr[i])) {
					uWrite("\nletters only\n\n");
					attempt();
					}
				}
			toLowerCaseString(inpstr);
			inpstr[0]=::toupper(inpstr[0]);
			name=inpstr;
			toLowerCaseString(inpstr);
			// If user has hung on another login clear that session
			for (auto u=listUser.list.begin(); u!=listUser.list.end(); ++u) {
				if (u->second->level==enums::UserLevel_LOGIN && u->second!=this && boost::iequals(u->second->name, name)) {
					u->second->kick();
					}
				}
			if (!IOUser::instance()->load(this, inpstr, true)) {
				uWrite("creating new account\n");
				}
//			else {
//				if (boost::iequals(name, "lopo")) {
//					level=enums::UserLevel_ADMIN;
//					}
//				}
			uWrite("\n\npassword: ");
			client->sendEchoOff();
			level=enums::UserLevel_LOGIN;
			stage=enums::UserStage_LOGIN_PWD;
			return;
		case enums::UserStage_LOGIN_PWD:
			if (len<MIN_PASSWORD_LEN) {
				uWrite("\n\ntoo short\n\n");
				attempt();
				return;
				}
			if (!password || !password->length()) { // if new user
				password=new std::string(security::Bcrypt::crypt(inpstr));
				uWrite("\n\nconfirm: ");
				stage=enums::UserStage_LOGIN_REENTER_PWD;
				}
			else {
				if (!password->compare(security::Bcrypt::crypt(inpstr, password->c_str()))) {
					delete password;
					password=nullptr;
					client->sendEchoOn();
//					cls();
					uWrite("press [ENTER] to login");
					stage=enums::UserStage_LOGIN_PROMPT;
					return;
					}
				uWrite("\n\nwrong password\n\n");
				attempt();
				}
			return;
		case enums::UserStage_LOGIN_REENTER_PWD:
			if (password->compare(security::Bcrypt::crypt(inpstr, password->c_str()))) {
				uWrite("\n\npassword nomatch\n\n");
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
		case enums::UserStage_LOGIN_PROMPT:
			IOUser::instance()->load(this, inpstr);
			delete password;
			password=nullptr;
			uWrite("\n\n");
			stage=enums::UserStage_CMD_LINE;
			uConnect();
			return;
		}
}

void User::uConnect()
{
	for (auto it=listUser.list.begin(); it!=listUser.list.end(); it++) {
		User* u=it->second;
		if (this!=u && !name.compare(u->name)) {
			uWrite("\n\nalready logged in - switching to old session ...\n");
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

bool User::getCharclientLine(std::string& inpstr)
{
	for (size_t l=0; l<inpstr.length(); ++l) {
		/* see if delete entered */
		if (inpstr[l]==8 || inpstr[l]==127) {
			if (buffpos) {
				buffpos--;
				if (checho) {
					uWrite("\b \b");
					}
				}
			continue;
			}
		buff[buffpos]=inpstr[l];
		/* See if end of line */
		if (inpstr[l]<32 || buffpos+2==inpstr.max_size()) {
			terminate(buff);
			inpstr.assign(buff);
			if (checho) {
				uWrite("\n");
				}
			return 1;
			}
		buffpos++;
		}
	if (checho && stage!=enums::UserStage_LOGIN_PWD && stage!=enums::UserStage_LOGIN_REENTER_PWD) {
		uWrite(inpstr);
		}
	return 0;
}

void User::attempt()
{
	attempts++;
	if (attempts==MAX_LOGIN_ATTEMPTS) {
		uWrite("\nmax attempts\n");
		kick();
		removeList();
		return;
		}
	stage=enums::UserStage_LOGIN_NAME;
	delete password;
	uWrite("\nlogin: ");
	client->sendEchoOn();
}

int User::execCommand(std::string inpstr)
{
	std::string comWord;

	comWord= word[0][0]=='.'
		? word[0].substr(1)
		: word[0];
	if (!comWord.length()) {
		uWrite("Unknown command\n");
		return 0;
		}
	removeFirst(inpstr);
	Command* cmd=nullptr;
	if (comWord=="say") {
		cmd=new command::Say;
		}
	else if (comWord=="quit") {
		cmd=new command::Quit;
		}
	if (cmd) {
		cmd->execute(this, inpstr);
		}
	else {
		uWrite("Unknown command\n");
		return 0;
		}
	return 1;
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
