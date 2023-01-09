#include "SMACChecker.h"

#include <vector>
#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace util {

void SMACChecker::CheckPath( const string& path ) {
	const vector<string> important_files = {
		"terranx.exe"
	};
	for (auto& file : important_files) {
		if ( access( ( path + "/" + file ).c_str(), F_OK ) == -1 ) {
			throw runtime_error("Invalid SMAC directory or SMAC installation corrupted (could not find file: '" + path + "/" + file + "').");
		}
	}
	
}

}
