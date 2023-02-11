#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "FS.h"

namespace util {

const bool FS::FileExists( const string& filename ) {
	return std::filesystem::exists(filename);
}

void FS::CreateDirectoryIfNotExists( const string& path ) {
	if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path)) {
		std::filesystem::create_directory(path);
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

