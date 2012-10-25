#pragma once

#include <stdexcept>

#pragma warning(disable:4251)
#pragma warning(disable:4275)
//windows defines
#if defined(EXAMPLES_LIB_BUILD)
#define EXAMPLES_LIB_API __declspec(dllexport)
#else
#define EXAMPLES_LIB_API __declspec(dllimport)
#endif