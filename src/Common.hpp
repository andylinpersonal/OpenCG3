/**
*   define some common symbols
*/

#pragma once
#ifndef OPENCG3_COMMON_HEADER
#define OPENCG3_COMMON_HEADER

#include <mutex>
#define AUTOLOCK(MutexType, to_lock)                                          \
{   lock_guard<MutexType> lock(to_lock);

#define AUTOUNLOCK                                                          \
}

#endif // !OPENCG3_COMMON_HEADER
