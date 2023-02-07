#include "FS.h"

#include <sys/stat.h>
#include <fstream>
#include <sstream>

namespace util {

const bool FS::FileExists( const string& filename ) {
	struct stat buffer;   
	return ( stat ( filename.c_str(), &buffer ) == 0 ); 
}

void FS::CreateDirectoryIfNotExists( const string& path ) {
	struct stat st = {0};
	if ( stat( path.c_str(), &st ) == -1) {
		mkdir( path.c_str(), 0700);
	}
}

const string FS::ReadFile( const string& filename ) {
	stringstream data;
	ifstream in( filename );
	data << in.rdbuf();
	in.close();
	return data.str();
}

const void FS::WriteFile( const string& filename, const string& data ) {
	ofstream out( filename );
	out << data;
	out.close();
}

}

