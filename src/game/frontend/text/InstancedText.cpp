#include "InstancedText.h"

#include "InstancedFont.h"
#include "game/frontend/sprite/InstancedSprite.h"
#include "scene/actor/Instanced.h"

namespace game {
namespace frontend {
namespace text {

InstancedText::InstancedText( const std::string& text, InstancedFont* font, const types::Color& color, const types::Color& shadow_color )
	: m_text( text ) {
	m_text_sprites = font->GetSymbolSprites( m_text, color, shadow_color );
	m_offsets = font->GetSymbolOffsets( m_text );
	m_instance_ids.reserve( m_text_sprites.size() );
}

InstancedText::~InstancedText() {
	Hide();
}

void InstancedText::ShowAt( const types::Vec3& coords ) {
	Hide();
	for ( size_t i = 0 ; i < m_text_sprites.size() ; i++ ) {
		const auto& offsets = m_offsets.at( i );
		m_instance_ids.push_back(
			m_text_sprites.at( i )->actor->AddInstance(
				{
					coords.x + offsets.x,
					coords.y + offsets.y,
					coords.z
				}
			)
		);
	}
}

void InstancedText::Hide() {
	if ( !m_instance_ids.empty() ) {
		ASSERT_NOLOG( m_instance_ids.size() == m_text_sprites.size(), "instance ids size mismatch" );
		for ( size_t i = 0 ; i < m_instance_ids.size() ; i++ ) {
			m_text_sprites.at( i )->actor->RemoveInstance( m_instance_ids.at( i ) );
		}
		m_instance_ids.clear();
	}
}

}
}
}