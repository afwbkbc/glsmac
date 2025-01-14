#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

int main( int argc, char* argv[] ) {

	const std::string path = argv[ 0 ];
	std::string dir = path.substr( 0, path.length() - 8 );
	const std::string sep = dir.substr( dir.length() - 1 );

	dir += ".." + sep + ".." + sep;

	const std::string out_file = argv[ 1 ];
	const std::string out_dir = out_file.substr( 0, out_file.rfind( sep ) );

	std::filesystem::create_directories( out_dir );

	if ( argc < 2 ) {
		std::cout << "Usage: " << argv[ 0 ] << " <out> [<file>] [<file>] [...]" << std::endl;
		exit( 1 );
	}

	std::ofstream out( out_file );
	if ( !out.is_open() ) {
		std::cout << "Could not open file for writing: " << out_file << std::endl;
		exit( 1 );
	}
	std::cout << "Creating " << argv[ 1 ] << "..." << std::endl;

	out << R"(#include <unordered_map>
#include <string>
#include <vector>

static const std::unordered_map< std::string, std::vector< unsigned char > > s_embedded_files = {
)";

	for ( size_t i = 2 ; i < argc ; i++ ) {
		const std::string filename = argv[ i ];
		std::string key = "";
		key.reserve( filename.size() );
		bool first = true;
		for ( const auto& c : filename ) {
			switch ( c ) {
				case '.':
				case '/':
				case '\\':
				case '-': {
					key += '_';
					break;
				}
				default:
					key += c;
			}
		}

		out << "	{ \"" + key + "\", {";

		std::ifstream in( dir + filename );
		if ( !in.is_open() ) {
			std::cout << "Could not open file for reading: " << ( dir + filename ) << std::endl;
			exit( 1 );
		}
		std::stringstream buffer;
		buffer << in.rdbuf();
		const auto& str = buffer.str();
		std::cout << "	Embedding " << filename << " as " << key << " (" << std::to_string( str.size() ) << " bytes)" << std::endl;

		first = true;
		for ( const auto& c : str ) {
			if ( first ) {
				first = false;
			}
			else {
				out << ",";
			}
			out << " " << std::to_string( (unsigned char)c );
		}

		out << R"( } },
)";

	}

	out << R"(};

	const std::unordered_map< std::string, std::vector< unsigned char > >& GetEmbeddedFiles() {
		return s_embedded_files;
	}

)";
	out.close();
	std::cout << argv[ 1 ] << " created successfully" << std::endl;

	return 0;
}
