#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <vector>
#include <memory>
#include <functional>
#include <mutex>
#include <sstream>

#include <boost/noncopyable.hpp>
#include <boost/chrono.hpp>

#ifdef _MSC_VER
#define thread_local __declspec(thread)
#endif
