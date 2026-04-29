#pragma once

#include <string>

namespace util {

class LogHelper {
public:
	static void Init();
	static void Print( const std::string& text );
	static void Println( const std::string& text, const bool nomutex = false );
	static void Flush();
};

}
