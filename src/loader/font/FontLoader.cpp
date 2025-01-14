#include "FontLoader.h"

#include "types/Font.h"
#include "util/FS.h"

namespace loader {
namespace font {

FontLoader::~FontLoader() {
	for ( auto& it : m_fonts ) {
		DELETE( it.second );
	}
	for ( const auto& f : m_builtin_fonts ) {
		DELETE( f.second );
	}
}

types::Font* FontLoader::LoadFont( const resource::resource_t res, const unsigned char size ) {
	const auto& name = GetFilename( res );
	const auto key = name + ":" + std::to_string( size );
	auto it = m_fonts.find( key );
	if ( it == m_fonts.end() ) {
		std::vector< unsigned char > data = {};
		util::FS::ReadFile( data, GetPath( res ) );
		it = m_fonts.insert(
			{
				key,
				LoadFontImpl( GetFilename( res ), data, size )
			}
		).first;
	}
	return it->second;
}

types::Font* FontLoader::GetBuiltinFont( const unsigned char size ) {
	auto it = m_builtin_fonts.find( size );
	if ( it == m_builtin_fonts.end() ) {
		const auto font_key = "<BUILTIN>:" + std::to_string( size );
		it = m_builtin_fonts.insert(
			{
				size,
				LoadFontImpl( "<BUILTIN>", util::FS::GetEmbeddedFile( "resource_font_terminus_TerminusTTF_4_49_3_minified_ttf" ), size )
			}
		).first;
	}
	return it->second;
}

}
}
