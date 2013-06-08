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