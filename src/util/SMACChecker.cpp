#include <vector>
#include <string>

#ifndef _WIN32

#include <unistd.h>

#endif

#include "SMACChecker.h"

namespace util {

bool SMACChecker::IsSMACDirectory( const std::string& path ) {
	const std::vector< std::string > important_files = {
		"terranx.exe"
	};
	for ( auto& file : important_files ) {
		std::string fullPath = path + "/" + file;
#ifdef _WIN32
		if (GetFileAttributesA(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
#else
		if ( access( fullPath.c_str(), F_OK ) == -1 ) {
#endif
			return false;
		}
	}
	return true;
}

}
