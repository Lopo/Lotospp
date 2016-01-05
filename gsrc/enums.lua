BeginEnumFile("enums")

enum("ReturnValue",
	{"RET_OK", "OK"},
	{"ERR_CANT_OPEN_FILE", "Can't open file"},
	"RET_LASTRET"
	)

enum("UserLevel",
	{"UserLevel_LOGIN", "login"},
	{"UserLevel_NOVICE", "novice"},
	{"UserLevel_USER", "user"},
	{"UserLevel_ADMIN", "admin"}
	)

enum("UserStage",
	{"UserStage_DISCONNECT", "disconnect"},
	{"UserStage_SWAPPED", "swapped"},
	{"UserStage_NEW", "new"},
	{"UserStage_LOGIN_ID", "login"},
	{"UserStage_LOGIN_PWD", "login"},
	{"UserStage_LOGIN_NAME", "login"},
	{"UserStage_LOGIN_NEW_PWD", "login"},
	{"UserStage_LOGIN_REENTER_PWD", "login"},
	{"UserStage_LOGIN_PROMPT", "login"},
	{"UserStage_CMD_LINE", "cmd"},
	{"UserStage_OLD_PWD", "pwd"},
	{"UserStage_NEW_PWD", "pwd"},
	{"UserStage_REENTER_PWD", "pwd"},
	{"UserStage_SUICIDE", "suicide"}
	)

enum("UserFlag",
	"UserFlag_PROMPT",
	"UserFlag_PUIP"
	)

enum("PrintCode",
	{"PrintCode_RS", "RS", "\\033[0m"},
	{"PrintCode_OL", "OL", "\\033[1m"},
	{"PrintCode_UL", "UL", "\\033[4m"},
	{"PrintCode_LI", "LI", "\\033[5m"},
	{"PrintCode_SN", "SN", "\\07"},
	{"PrintCode_RV", "RV", "\\033[7m"},
	{"PrintCode_SC", "SC", "\\033[2J"},
	{"PrintCode_RH", "RH", "\\033[H\\033[J"},
	{"PrintCode_LC", "LC", "\\033[K"},
	{"PrintCode_CU", "CU", "\\033[1A"},
	{"PrintCode_FK", "FK", "\\033[30m"},
	{"PrintCode_FR", "FR", "\\033[31m"},
	{"PrintCode_FG", "FG", "\\033[32m"},
	{"PrintCode_FY", "FY", "\\033[33m"},
	{"PrintCode_FB", "FB", "\\033[34m"},
	{"PrintCode_FM", "FM", "\\033[35m"},
	{"PrintCode_FT", "FT", "\\033[36m"},
	{"PrintCode_FW", "FW", "\\033[37m"},
	{"PrintCode_BK", "BK", "\\033[40m"},
	{"PrintCode_BR", "BR", "\\033[41m"},
	{"PrintCode_BG", "BG", "\\033[42m"},
	{"PrintCode_BY", "BY", "\\033[43m"},
	{"PrintCode_BB", "BB", "\\033[44m"},
	{"PrintCode_BM", "BM", "\\033[45m"},
	{"PrintCode_BT", "BT", "\\033[46m"},
	{"PrintCode_BW", "BW", "\\033[47m"},
	{"PrintCode_IP", "IP"},
	{"PrintCode_NP", "NP"},
	{"PrintCode_PR", "PR"}
	)

enum("TelnetFlag",
	"TelnetFlag_ECHO",
	"TelnetFlag_SGA",
	"TelnetFlag_TERMTYPE",
	"TelnetFlag_TERMSIZE",
	"TelnetFlag_GOT_TELOPT_INFO",
	"TelnetFlag_ANSI_TERM",
	"TelnetFlag_LASTFLAG"
	)

enum("LoginCom",
	{"LoginCom_QUIT", "quit"},
	{"LoginCom_WHO", "who"},
	{"LoginCom_VERSION", "version"}
	)

EndEnumFile()
