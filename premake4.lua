solution 'profi_all'
	configurations { "Debug", "Release", "Debug Dynamic", "Release Dynamic" }
	platforms { 'x32', 'x64' }
			
	include './src'
	include './samples/simple'
	include './samples/mt'
	include './tests/profi_tests'
