function enum(params, ...)
	local name = ""
	local namespace = nil
	local nameprefix = ""
	local values = {...}

	if type(params) == "string" then
		namespace, name = params:match("(.-)::(.+)")
		if not name then
			namespace = nil
			name = params
		end
	else
		name = params["name"]
		namespace = params["namespace"]
	end
	if namespace then nameprefix = namespace .. "::" end

	-- Automask = values will be generated automatically as a bitmask
	local automask = params["bitmask"]

	-- Pre-process the values some
	-- We split them into a tree structure, with name as a list, and value as an int
	local nvalues = {}
	for _, value in ipairs(values) do
		nval = {}

		function split(s)
			local e, v = s:match("(.-)%s*=%s*(.*)")
			if v then
				if v:match("%d+") then
					automask = false
				end
			else
				e = s
			end
			local n = {}
			n["name"] = {e}
			n["int" ] = v

			return n
		end

		if type(value) == "string" then
			nval = split(value)
		elseif type(value) == "table" then
			nval = split(table.remove(value, 1))
			for e, v in ipairs(value) do
				table.insert(nval["name"], v)
			end
			if value["null"] then
				nval["null"] = true
			end
		else
			error("Value must be table/string")
		end
		table.insert(nvalues, nval)
	end
	values = nvalues

	-- Automask the values, if we should
	local mask_count = 0
	local fullmask = 0
	local allmask = {}
	if automask then
		local mask = 1
		for _, v in ipairs(values) do
			if v["null"] then
				-- Some values are NULL
				v["int"] = 0
			elseif v["all"] then
				-- All values!
				table.insert(allmask, v)
			elseif v["int"] then
				-- Leave it alone, it's a composition
			else
				v["int"] = mask
				fullmask = fullmask + mask
				mask = mask * 2
				mask_count = mask_count + 1
			end
		end
	end

	-- Fill the 'all' values
	for _, v in ipairs(allmask) do
		v["int"] = fullmask
	end

	if namespace then
		header:write("namespace " .. namespace .. " {" .. "\n")
	end

	-- Write the enum declaration to the header
	header:write("\tnamespace enums {\n")
	header:write("\t\tenum " .. name .. " {\n")
	for k, v in ipairs(values) do
		header:write("\t\t\t" .. v["name"][1])
		if v["int"] then
			header:write("=" .. v["int"])
		end
		if k~=#values then
			header:write(",")
		end
		header:write("\n")
	end

	-- End enum
	header:write("\t\t\t}; // enum " .. name .. "\n")
	header:write("\t\t} // namespace enums\n\n")

	-- Create the Enum type
	local enumtype = ""
	local enumbase = ""

	if params["bitmask"] then
		if automask then
			enumbase = "Enum<" .. nameprefix .. "enums::" .. name .. ", " .. mask_count .. ">"
		else
			enumbase = "Enum<" .. nameprefix .. "enums::" .. name .. ", -1>"
		end
		enumtype = "Bit" .. enumbase
	else
		enumbase = "Enum<" .. nameprefix .. "enums::" .. name .. ", " .. nameprefix .. "enums::" .. values[#values]["name"][1] .. "+1>"
		enumtype = enumbase
	end
	header:write("\ttypedef " .. enumtype .. " " .. name .. ";\n")
	header:write("\ttypedef " .. enumbase .. " " .. name .. "__Base;\n\n")

	-- Write the enum definitions (as real types)
	for _, v in ipairs(values) do
		header:write("\tconst " .. name .. " " .. v["name"][1] .. "(enums::" .. v["name"][1] .. ");\n")
	end
	header:write("\n\ttypedef " .. name .. " " .. name .. "Type;\n")

	if namespace then
		header:write("\t} // namespace " .. namespace .. "\n")
	end
	header:write("\n\n")


	-- Write the .implementation declarations
	--implementation:write("template<> bool " .. nameprefix .. name .. "__Base::initialized=false;\n")
	implementation:write("template<> std::string " .. nameprefix .. name .. "__Base::enum_name=\"" .. name .. "\";\n")
	--implementation:write("template<> " .. nameprefix .. name .. "__Base::EnumToString " .. nameprefix .. name .. "__Base::enum_to_string=" .. nameprefix .. name .. "__Base::EnumToString();\n")
	--implementation:write("template<> " .. nameprefix .. name .. "__Base::EnumToValue " .. nameprefix .. name .. "__Base::enum_to_value=" .. nameprefix .. name .. "__Base::EnumToValue();\n")
	--implementation:write("template<> " .. nameprefix .. name .. "__Base::StringToEnum " .. nameprefix .. name .. "__Base::string_to_enum=" .. nameprefix .. name .. "__Base::StringToEnum();\n")
	--implementation:write("template<> " .. nameprefix .. name .. "__Base::StringToEnum " .. nameprefix .. name .. "__Base::lstring_to_enum=" .. nameprefix .. name .. "__Base::StringToEnum();\n")

	-- Init function
	implementation:write("template<> void " .. nameprefix .. name .. "__Base::initialize()\n")
	implementation:write("{\n")
	for _, v in ipairs(values) do
		local first = table.remove(v["name"], 1)
		implementation:write("\t" .. "initAddValue(" .. nameprefix .. "enums::" .. first .. ", ")
		if #v["name"]==0 then
			implementation:write("\"" .. first .. "\"")
		else
			implementation:write("\"" .. v["name"][1] .. "\"")
			local t=type(v["name"][2])
			if t=="string" then
				implementation:write(", \"" .. v["name"][2] .. "\"")
			elseif t=="number" then
				implementation:write(", " .. v["name"][2])
			else
				implementation:write(", \"\"")
			end
		end
		implementation:write(");\n")

	end
	implementation:write("}\n\n")
end

function definition(code)
	header:write(code)
end

function BeginEnumFile(filename)
	-- These are global
	local dir = ""
	if #arg == 0 then
		dir = "."
	else
		dir = arg[1]
	end
	header = {name = dir .. "/" .. filename .. ".h", _ = ""}
	function header:write(s)
		header._ = header._ .. s
	end
	implementation = {name = dir .. "/" .. filename .. ".cpp", _ = ""}
	function implementation:write(s)
		implementation._ = implementation._ .. s
	end

	-- Some warning text
	automated_text = "// This file has been automatically generated by Lua script at " .. os.date("%Y-%m-%d %H:%M:%S") .. "\n"
		.. "// Do not make changes to this file manually, as they will be discarded as soon as the project is recompiled\n"
	header:write(automated_text)
	implementation:write(automated_text)

	-- Include guards
	header:write("#ifndef LOTOSPP_LUAGENERATED_" .. filename:upper() .. "_H\n")
	header:write("#define LOTOSPP_LUAGENERATED_" .. filename:upper() .. "_H\n\n\n")

	-- Includes
	header:write("#include \"config.h\"\n")
	header:write("#include \"Common/enum.h\"\n\n\n")
	header:write("using lotospp::Enum;\n\n")
	header:write("namespace lotospp {\n")
	implementation:write("#include \"" .. filename .. ".h\"\n\n\n")
	implementation:write("namespace lotospp {\n\n")
end

function EndEnumFile()
	header:write("}\n\n")
	header:write("#endif\n")
	implementation:write("}\n")

	local hfile = io.open(header.name, "w+")
	hfile:write(header._)
	hfile:close()
	local cppfile = io.open(implementation.name, "w+")
	cppfile:write(implementation._)
	cppfile:close()
end
