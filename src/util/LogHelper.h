#pragma once

#include <string>

namespace util {

class LogHelper {
public:
	static void Print( const std::string& text );
	static void Println( const std::string& text );
	static void Flush();
};

}
