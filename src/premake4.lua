project "profi"
	kind "StaticLib"
	language "C++"
	uuid '7CE179F7-6A98-4744-9B2D-FFFF433A8300'
	
	filesInProj = {
		"*.h",
		"*.cpp",
		"../include/*.h"
	}

	files(filesInProj)

	vpaths {
		['Source Files'] = '*.cpp',
		['Header Files'] = '*.h',
	}
	
	pchheader(path.getabsolute('./precompiled.h'))
	pchsource(path.getabsolute('./precompiled.cpp'))
	
	flags { 'NoRTTI', 'EnableSSE2' }
	
	configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
		 targetdir "../bin"
 
    configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
		 targetdir "../bin"