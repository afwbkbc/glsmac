#include "InstancedTextManager.h"

#include "InstancedFont.h"
#include "InstancedText.h"
#include "types/Font.h"

namespace task {
namespace game {
namespace text {

InstancedTextManager::InstancedTextManager( scene::Scene* scene )
	: m_scene( scene ) {
	//
}

InstancedTextManager::~InstancedTextManager() {
	for ( const auto& it : m_instanced_texts ) {
		delete it.second;
	}
	for ( const auto& it : m_instanced_fonts ) {
		delete it.second;
	}
}

InstancedFont* InstancedTextManager::GetInstancedFont( const types::Font* font ) {
	auto it = m_instanced_fonts.find( font );
	if ( it == m_instanced_fonts.end() ) {
		Log( "Initializing instanced font: " + font->m_name );
		it = m_instanced_fonts.insert(
			{
				font,
				new InstancedFont( font )
			}
		).first;
	}
	return it->second;
}

InstancedText* InstancedTextManager::CreateInstancedText(
	const std::string& text,
	InstancedFont* font,
	const types::Color& color,
	const z_level_t z_level,
	const float z_index_adjustment
) {

	ASSERT( s_zlevel_map.find( z_level ) != s_zlevel_map.end(), "unknown z level " + std::to_string( z_level ) );
	ASSERT( z_index_adjustment >= -MAX_ZINDEX_ADJUSTMENT && z_index_adjustment <= MAX_ZINDEX_ADJUSTMENT, "z index adjustment too large" );

	Log( "Creating instanced text: '" + text + "', " + font->GetFontName() );

	/*
	NEWV( instanced, scene::actor::Instanced, actor );
	instanced->SetZIndex( s_zlevel_map.at( z_level ) + z_index_adjustment );
	m_scene->AddActor( instanced );
	 */

	return new InstancedText( text, font );
}

}
}
}
