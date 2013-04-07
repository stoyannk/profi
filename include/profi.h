#pragma once

#define PROFI_ENABLE

#ifdef PROFI_ENABLE

#include <LocalProfileScope.h>

#define PROFI_FUNC profi::LocalProfileScope lps_##__LINE__##(__FUNCTION__);

#else

#define PROFI_FUNC

#endif