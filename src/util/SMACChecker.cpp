#include <vector>
#include <string>
#include <iostream>
#include <fcntl.h>
#ifdef __linux__
#include <unistd.h>
#endif

#include "SMACChecker.h"

using namespace std;

namespace util {

void SMACChecker::CheckPath( const string& path ) {
	const vector<string> important_files = {
		"terranx.exe"
	};
	for (auto& file : important_files) {
		string fullPath = path + "/" + file;
#ifdef _WIN32
		if (GetFileAttributesA(fullPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
#else
		if (access(fullPath.c_str(), F_OK) == -1) {
#endif
			throw runtime_error("Invalid SMAC directory or SMAC installation corrupted (could not find file: '" + fullPath + "').");
		}
		//if ( access( ( path + "/" + file ).c_str(), F_OK ) == -1 ) {
		//	throw runtime_error("Invalid SMAC directory or SMAC installation corrupted (could not find file: '" + path + "/" + file + "').");
		//}
	}
	
}

}
