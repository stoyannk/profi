project 'simple'
	kind 'ConsoleApp'
	language 'C++'
	uuid '45344FDF-9294-4EB1-B6ED-004C7C72C3E8'
					
	files { '**.h', '**.cpp' }

	pchheader(path.getabsolute('./precompiled.h'))
	pchsource(path.getabsolute('./precompiled.cpp'))
	
	vpaths {
		['Source Files'] = '*.cpp',
		['Header Files'] = '*.h',
	}
	
	flags { 'NoRTTI', 'EnableSSE2' }

	includedirs { '../../include/' }
	links { 'profi.lib' }
	libdirs { "../../bin/" }
	
	configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
		 targetdir "../../bin"
 
    configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
		 targetdir "../../bin"