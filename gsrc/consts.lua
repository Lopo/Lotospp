BeginEnumFile("consts")

enum("TelnetOpt",
--	"TELOPT_BIN=0", -- Binary transmission RFC 856
	"TELOPT_ECHO=1", -- RFC 857
	"TELOPT_SGA=3", -- suppres go ahead RFC 858
--	"TELOPT_STATUS=5", -- status RFC 859
--	"TELOPT_TM=6", -- timing mark RFC 860
	"TELOPT_TERM=24", -- Terminal type option RFC 1091
	"TELOPT_NAWS=31", -- Negotiate About Window Size option RFC 1073
--	"TELOPT_SPEED=32", -- Terminal speed RFC 1079
--	"TELOPT_FLOW=33", -- Remote flow control RFC 1372
--	"TELOPT_LINE=34", -- Linemode RFC 1184
--	"TELOPT_ENV=36", -- environment variables RFC 1408
--	"TELOPT_NEWENV=39", -- Environment variables RFC 1572
--	"TELOPT_CHARSET=42", -- Charset option
	"TELOPT_EOL=255" -- Extended-Options-List
	)

enum("TelnetCmd",
	"TELCMD_SE=240", -- The end of sub-negotiation options
--	"TELCMD_NOP=241", -- No operation
--	"TELCMD_DM=242", -- Data mark
--	"TELCMD_BRK=243", -- Break
--	"TELCMD_IP=244", -- Suspend
--	"TELCMD_AO=245", -- Abort output
	"TELCMD_AYT=246", -- Are You There
--	"TELCMD_EC=247", -- Erase character
--	"TELCMD_EL=248", -- Erase line
--	"TELCMD_GA=249", -- Go ahead
	"TELCMD_SB=250", -- The start of sub-negotiation options
	"TELCMD_WILL=251", -- Confirm willingness to negotiate
	"TELCMD_WONT=252", -- Confirm unwillingness to negotiate
	"TELCMD_DO=253", -- Indicate willingness to negotiate
	"TELCMD_DONT=254", -- Indicate unwillingness to negotiate
	"TELCMD_IAC=255" -- Marks the start of a negotiation sequence RFC 861
	)

enum("TelnetSub",
	"TELSUB_IS=0",
	"TELSUB_SEND=1",
	"TELSUB_INFO=2"
	)

enum("AsciiChar",
	"ASCII_NUL=0",
	"ASCII_BEL=7",
	"ASCII_BS=8", -- backspace
--	"ASCII_HT=9", -- Horizontal Tab
--	"ASCII_LF=10", -- Line Feed
--	"ASCII_VT=11", -- Vertical Tab
--	"ASCII_FF=12", -- Form Feed
--	"ASCII_CR=13", -- Carriage Return
	"ASCII_ESC=27", -- Escape
	"ASCII_DEL=127" -- Delete
	)


EndEnumFile()
