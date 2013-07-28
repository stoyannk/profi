profi
=====

Profi is a very light-weight and fast instrumenting profiling library for C++ applications.

Features
=====
 - Instrumenting profiler
 - Small performance impact while profiling
 - Designed for multithreaded code
 - DLL-friendly
 - Multiple timing functions supported
 - Easy eanbling/disabling the whole profiling facility
 - Multiple profiling granularities
 - JSON result exporter
 - HTML result visualizer
 
Motivation
=====
There aren't alot of fast, simple, instrumenting and multithreading-friendly profiling libraries around. 
A primary goal of profi is to be simple, unobtrusive and fast. Profiling code can be added everywhere and there is no need
to 'initialize' a thread like in other libraries. The JSON output and HTML page that renders it make analyzing the data easier but
there is more work to be done in the HTML to make it more user-friendly for very large data sets. 
Multithreaded code is the primary target of profi. DLL-awareness is also very important because all the projects I work on are broken up in many shared libraries 
at least during development.
An excellent library that shares many similarities in it's design is the "High Performance C++ Profiler" (http://code.google.com/p/high-performance-cplusplus-profiler/).
Visual Studio has a good instrumenting profiler that however will blindly profile all functions. For large projects this might mean too many hard to read 
data as well as too much slow-down for the application. Profi will be cross-platform soon.
With profi you can control and profile just some critical parts of the program. If you are careful when controlling the granularity of the profile it can even 
be left running in release or production builds because the slow-down will be negligible.

Implementation
=====
The library keeps the profiling stacks of every thread separately to minimize data sharing across threads and hence avoid the need of data locking.
A custom hash map implementation is used to keep track of the prfiling scopes and their respective timers and counters. 
I use a custom hash map because I don't need to delete any data from it ever and this allows me to make a faster and simpler implementation. 
When a profiling data dump has to be performed, the whole hash map for a scope is copied to minimize locking span. The data is dumped from the copy.
Two timing methods are supported: QueryPerformanceCounter and rdtsc. The library supports CPU flushing when timing. For more details on 
timing methods please refer to my post in the Coherent Labs blog - http://blog.coherent-labs.com/2013/03/timestamps-for-performance-measurements.html.

Requirements
=====
Currently the library runs on Windows only, mostly because I hadn't time to port it to other platforms - something that should be trivial as the everything was written with
portability in mind.
Tested on MSVC 2010 & MSVC 2012. A C++11 compliant compiler and STL library are required.
Premake is required to generate the project files.

Usage
=====
1. Use premake4 (http://industriousone.com/premake) to generate the project and solution files. 
In a command prompt in the main folder of profi run "premake4 vs2010ng"
2. Open the "profi_all.sln" solution. Compile the library.
3. Consult the "simple" and "mt" sample applications for sample usages of the library.
4. "simple" and "mt" dump their data in a file named "output.json" in their respective folders. JSON files created by profi 
can be visualized via the "visualizer.html" page in the "visualizer" folder. By default it reads a file named "output.json" 
in it's working folder. Changing the file it loads is trivial - just edit the HTML file. It works on both Firefox and Chrome 
(for Chrome run it with the --allow-file-access-from-files argument)

The "profi_decls.h" header contains a "Configuration" section where the behavior of the library can be customized. 
To embed the library just include "profi.h" and link with it either dynamically ot static.

Future
=====
I plan to port the library at least to Mac & Linux and keep updating and enhancing it.
Currently the profile can't be stopped or it's granularity changed runtime.
I also hope to have time to expand it one day to be an event-driven library.
