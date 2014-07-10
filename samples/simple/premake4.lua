project 'simple'
	kind 'ConsoleApp'
	language 'C++'
	uuid '45344FDF-9294-4EB1-B6ED-004C7C72C3E8'

	dofile '../../src/third_party.lua'	
	
	configuration 'x32'
		links { '../../bin32/profi' }
		libdirs { "../../bin32/" }
		targetdir "../../bin32"
		
	configuration 'x64'
		links { '../../bin64/profi' }
		libdirs { "../../bin64/" }
		targetdir "../../bin64"
		
	configuration '*'
	
	files { '**.h', '**.cpp' }

	pchheader('precompiled.h')
	pchsource('precompiled.cpp')
	
	vpaths {
		['Source Files'] = '*.cpp',
		['Header Files'] = '*.h',
	}
	
	flags { 'NoRTTI', 'EnableSSE2' }

	includedirs { '../../include/' }
	
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