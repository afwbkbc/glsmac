#include "InstancedTextManager.h"

#include "InstancedFont.h"
#include "InstancedText.h"
#include "types/Font.h"

namespace game {
namespace frontend {
namespace text {

InstancedTextManager::InstancedTextManager( sprite::InstancedSpriteManager* ism )
	: m_ism( ism ) {
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
				new InstancedFont( m_ism, font )
			}
		).first;
	}
	return it->second;
}

InstancedText* InstancedTextManager::CreateInstancedText(
	const std::string& text,
	InstancedFont* font,
	const types::Color& color,
	const types::Color& shadow_color
) {
	Log( "Creating instanced text: '" + text + "', " + font->GetFontName() + ", " + color.ToString() + "-" + shadow_color.ToString() );
	return new InstancedText( text, font, color, shadow_color );
}

}
}
}
