project 'profi_tests'
	kind 'ConsoleApp'
	language 'C++'
	uuid '0CDF2671-F995-4ECF-A7AB-1677C2AA18BD'

	dofile '../../src/third_party.lua'	
	
	files { '**.h', '**.cpp' }
	
	vpaths {
		['Source Files'] = '*.cpp',
		['Header Files'] = '*.h',
	}
	
	flags { 'NoRTTI', 'EnableSSE2' }

	includedirs { '../../include/', '../../src/', '../gtest-1.6.0/include' }
	libdirs { "../../bin/" }
	
	configuration "Debug"
         defines { "DEBUG;_VARIADIC_MAX=10" }
         flags { "Symbols" }
		 targetdir "../../bin"
		 links { '../../bin/profi', '../../bin/gtestd', '../../bin/gtest_main-mdd' }
 
    configuration "Release"
         defines { "NDEBUG;_VARIADIC_MAX=10" }
         flags { "Optimize" }
		 targetdir "../../bin"
		 links { '../../bin/profi', '../../bin/gtest', '../../bin/gtest_main-md' }