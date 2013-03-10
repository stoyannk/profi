#pragma once

#define WIN32_LEAN_AND_MEAD
#define INITGUID
#include <Windows.h>
#include <evntrace.h>
#include <Evntcons.h>
#include <tlhelp32.h>
#include <comutil.h>
#include <wbemidl.h>
#include <wmistr.h>
#include <comdef.h>

#include <memory>
#include <thread>
#include <vector>
#include <algorithm>

#include <iostream>