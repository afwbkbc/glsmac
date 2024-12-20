#include "TXTLoader.h"

#include "util/FS.h"
#include "util/String.h"

namespace loader {
namespace txt {

const TXTLoader::txt_data_t& TXTLoader::GetTXTData( const std::string& path ) {
	auto it = m_txt_data.find( path );
	if ( it == m_txt_data.end() ) {
		it = m_txt_data.insert(
			{
				path,
				txt_data_t{ util::FS::ReadFile( path ) }
			}
		).first;
	}
	return it->second;
}

TXTLoader::txt_data_t::txt_data_t( const std::string& source ) {

	// raw lines
	lines = util::String::Split( source, '\n' );

	// sections
	sections_t::iterator section_it = sections.end();
	for ( const auto& line : lines ) {
		if ( !line.empty() && line[ 0 ] == '#' && line.find( ' ' ) == std::string::npos ) {
			if ( line.substr( 0, 4 ) == "#END" ) {
				section_it = sections.end();
			}
			else {
				const auto section_name = line.substr( 1 );
				ASSERT_NOLOG( sections.find( section_name ) == sections.end(), "section '" + section_name + "' already exists" );
				section_it = sections.insert(
					{
						section_name,
						{}
					}
				).first;
			}
		}
		else {
			if ( !line.empty() && section_it != sections.end() ) {
				section_it->second.push_back( line );
			}
		}
	}

}

}
}
