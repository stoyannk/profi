project 'mt'
	kind 'ConsoleApp'
	language 'C++'
	uuid 'C0D6DE44-BC48-4A0C-BC52-8BE497A4D61A'

	dofile '../../src/third_party.lua'	

	files { '**.h', '**.cpp' }

	pchheader('precompiled.h')
	pchsource('precompiled.cpp')
	
	vpaths {
		['Source Files'] = '*.cpp',
		['Header Files'] = '*.h',
	}
	
	flags { 'NoRTTI', 'EnableSSE2' }

	includedirs { '../../include/' }
	links { '../../bin/profi' }
	libdirs { "../../bin/" }

	targetdir "../../bin"
	
	configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
 
    configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
		 
 	configuration "Debug Dynamic"
         defines { "DEBUG", "PROFI_DYNAMIC_LINK" }
         flags { "Symbols" }
 
    configuration "Release  Dynamic"
         defines { "NDEBUG", "PROFI_DYNAMIC_LINK" }
         flags { "Optimize" }
		 