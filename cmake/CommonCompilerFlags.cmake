if (WIN32)
	get_WIN32_WINNT(ver)
	add_definitions(-D_WIN32_WINNT=${ver})
endif ()

include(CheckCXXCompilerFlag)

# Add -Wall if supported by compiler
check_cxx_compiler_flag(-Wall CXX_COMPILER_WALL)
if (CXX_COMPILER_WALL)
	add_compile_options(-Wall)
endif ()

# Add -Wextra if supported by compiler
check_cxx_compiler_flag(-Wextra CXX_COMPILER_WEXTRA)
if (CXX_COMPILER_WEXTRA)
	add_compile_options(-Wextra)
endif ()

# Add -Wextra if supported by compiler
check_cxx_compiler_flag(-Wno-unused-parameter CXX_COMPILER_WNOUNUSEDPARAMETER)
if (CXX_COMPILER_WNOUNUSEDPARAMETER)
	add_compile_options(-Wno-unused-parameter)
endif ()

if (FORCE32)
	check_c_compiler_flag(-m32 C_COMPILER_M32)
	check_cxx_compiler_flag(-m32 CXX_COMPILER_M32)
	if (CXX_COMPILER_M32 AND C_COMPILER_M32)
		add_compile_options(-m32)
	endif ()
endif ()
