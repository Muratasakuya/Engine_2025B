#pragma once

//============================================================================
//	include
//============================================================================

// windows
#include <Windows.h>
// c++
#include <crtdbg.h>
#include <iostream>
#include <string>
#include <cassert>
#include <sstream>

//============================================================================
//	Assert class
//============================================================================
class Assert {
public:
	//========================================================================
	//	public Methods
	//========================================================================

	Assert() = default;
	~Assert() = default;

	//--------- functions ----------------------------------------------------

	static void DebugAssert(bool condition, const std::string& message, const char* function);

	static void ReleaseAssert(bool condition, const std::string& message, const char* function);

	static void AssertHandler(bool condition, const std::string& message, const char* function);
};

//============================================================================
//	macro definition
//============================================================================
#define ASSERT(condition, message) Assert::AssertHandler(condition, message, __FUNCTION__)