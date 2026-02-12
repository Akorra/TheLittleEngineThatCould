#pragma once

#include <cassert>
#include <iostream>

namespace TLETC
{
// Debug only assertions
#ifdef NDEBUG
    #define TLETC_ASSERT(condition, message) ((void)0)
#else
    #define TLETC_ASSERT(condition, message) \
        do { \
            if (!(condition)) { \
                std::cerr << "Assertion failed: " << message << "\n" \
                          << "File: " << __FILE__ << "\n" \
                          << "Line: " << __LINE__ << "\n" \
                assert(condition); \
            } \
        } while
#endif

// Always-on verification (even in release)
#define TLETC_VERIFY(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "Verification failed: " << message << "\n" \
                      << "File: " << __FILE__ << "\n" \
                      << "Line: " << __LINE__ << "\n"; \
            std::abort(); \
        } \
    } while(0)

} // namespace TLETC
