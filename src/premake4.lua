project "profi"
	kind "StaticLib"
	language "C++"
	uuid '7CE179F7-6A98-4744-9B2D-FFFF433A8300'
	
	includedirs { '../include/' }
	includedirs { '../src/' }
	dofile 'third_party.lua'
	
	configuration 'x32'
		defines {
			'PROFI_X32'
		}
		targetdir "../bin32"
		
	configuration 'x64'
		defines {
			'PROFI_X64'
		}
		targetdir "../bin64"
	configuration '*'
	
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
	
	pchheader('precompiled.h')
	pchsource('precompiled.cpp')
	
	flags { 'NoRTTI', 'EnableSSE2' }

	configuration "Debug"
         defines { "DEBUG", "PROFI_BUILD" }
         flags { "Symbols" }
 
    configuration "Release"
         defines { "NDEBUG", "PROFI_BUILD" }
         flags { "Optimize" }

	configuration "Debug Dynamic"
         kind "SharedLib"
		 defines { "DEBUG", "PROFI_BUILD", "PROFI_DYNAMIC_LINK" }
         flags { "Symbols" }
 
    configuration "Release Dynamic"
         kind "SharedLib"
		 defines { "NDEBUG", "PROFI_BUILD", "PROFI_DYNAMIC_LINK" }
         flags { "Optimize" }
