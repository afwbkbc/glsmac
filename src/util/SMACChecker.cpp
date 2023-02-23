#include <vector>
#include <string>
#include <iostream>
#include <fcntl.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include "SMACChecker.h"

using namespace std;

namespace util {

bool SMACChecker::IsSMACDirectory( const string& path ) {
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
			return false;
		}
	}
	return true;
}

}
