// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#if (_MSC_VER >= 1600)
#define PROFI_OVERRIDE override

#else
#define PROFI_OVERRIDE

#endif

#ifdef _MSC_VER
	#ifdef PROFI_BUILD
	#define PROFI_EXPORTED_SYMBOL __declspec(dllexport)
	#else
	#define PROFI_EXPORTED_SYMBOL __declspec(dllimport)
	#endif
#endif

#if defined(PROFI_DYNAMIC_LINK)
#define PROFI_EXPORT PROFI_EXPORTED_SYMBOL
#else
#define PROFI_EXPORT
#endif
