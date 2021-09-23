BeginEnumFile("TelnetCmd")
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
EndEnumFile()
