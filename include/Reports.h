// Copyright (c) 2013, Stoyan Nikolov
// All rights reserved.
// This software is governed by a permissive BSD-style license. See LICENSE.
#pragma once

#include <profi_decls.h>

namespace profi {

class PROFI_EXPORT IReport {
public:
	virtual ~IReport() {};
	virtual const void* Data() = 0;
	virtual unsigned Size() = 0;

	virtual void Release() = 0;
};

}