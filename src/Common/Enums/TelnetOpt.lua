BeginEnumFile("TelnetOpt")
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
EndEnumFile()
