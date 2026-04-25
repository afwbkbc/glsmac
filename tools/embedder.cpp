#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

void Embed( std::ofstream& dst, const std::string& srcdir, const std::string& path ) {

	const auto full_path = (std::filesystem::path)srcdir / path;

	if ( std::filesystem::is_directory( full_path ) ) {
		for ( const auto& entry : std::filesystem::directory_iterator( full_path ) ) {
			const auto relpath = entry.path().string().substr( srcdir.length() + 1 );
			Embed( dst, srcdir, relpath );
		}
	}
	else if ( std::filesystem::is_regular_file( full_path ) ) {

		std::string key = "";
		key.reserve( path.size() );
		for ( const auto& c : path ) {
			switch ( c ) {
				case '\\': {
					key += '/';
					break;
				}
				default:
					key += c;
			}
		}

		dst << "	{ \"" + key + "\", {";

		std::ifstream in( full_path, std::ios::binary );
		if ( !in.is_open() ) {
			std::cout << "Could not open file for reading: " << full_path << std::endl;
			exit( 1 );
		}
		std::stringstream buffer;
		buffer << in.rdbuf();
		in.close();
		const auto& str = buffer.str();

#ifdef DEBUG
		std::cout << "	Embedding " << path << " as " << key << " (" << std::to_string( str.size() ) << " bytes)" << std::endl;
#endif

		bool first = true;
		for ( const auto& c : str ) {
			if ( first ) {
				first = false;
			}
			else {
				dst << ",";
			}
			dst << " " << std::to_string( (unsigned char)c );
		}

		dst << R"( } },
)";

	}
	else {
		std::cout << "	Skipping " << path << " because it's neither file nor directory" << std::endl;
	}
}

int main( int argc, char* argv[] ) {

	if ( argc < 3 ) {
		std::cout << "Usage: " << argv[ 0 ] << " <out_file> <src_dir> [<src_file>] [<src_file>] [...]" << std::endl;
		exit( 1 );
	}

	const std::string path = argv[ 0 ];
	std::string dir = path.substr( 0, path.length() - 8 );
	const std::string sep = dir.substr( dir.length() - 1 );

	const std::filesystem::path out_file = argv[ 1 ];
	const std::filesystem::path out_dir = out_file.parent_path();
	const std::filesystem::path srcdir = argv[ 2 ];

	std::filesystem::create_directory( out_dir );

	std::ofstream out( out_file.string() );
	if ( !out.is_open() ) {
		std::cout << "Could not open file for writing: " << out_file << std::endl;
		exit( 1 );
	}

#ifdef DEBUG
	std::cout << "Creating " << out_file << "..." << std::endl;
#endif

	out << R"(#include <unordered_map>
#include <string>
#include <vector>

static const std::unordered_map< std::string, std::vector< unsigned char > > s_embedded_files = {
)";

	for ( size_t i = 3 ; i < argc ; i++ ) {
		Embed( out, srcdir, argv[ i ] );
	}

	out << R"(};

	const std::unordered_map< std::string, std::vector< unsigned char > >& GetEmbeddedFiles() {
		return s_embedded_files;
	}

)";
	out.close();
	
#ifdef DEBUG
	std::cout << argv[ 1 ] << " created successfully" << std::endl;
#endif

	return 0;
}
