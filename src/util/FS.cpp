#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "FS.h"

using namespace std;

namespace util {

const bool FS::Exists( const string& path ) {
	return std::filesystem::exists( path );
}

const bool FS::FileExists( const string& path ) {
	return Exists( path ) && std::filesystem::is_regular_file( path );
}

const bool FS::DirectoryExists( const string& path ) {
	return Exists( path ) && std::filesystem::is_directory( path );
}

void FS::CreateDirectoryIfNotExists( const string& path ) {
	if ( !DirectoryExists( path ) ) {
		std::filesystem::create_directory( path );
	}
}

const string FS::ReadFile( const string& path ) {
	ASSERT_NOLOG( FileExists( path ), "file \"" + path + "\" does not exist or is not a file" );
	stringstream data;
	ifstream in( path );
	data << in.rdbuf();
	in.close();
	return data.str();
}

const void FS::WriteFile( const string& path, const string& data ) {
	ofstream out( path );
	out << data;
	out.close();
}

}

