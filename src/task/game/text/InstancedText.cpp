#include "InstancedText.h"

#include "InstancedFont.h"
#include "task/game/sprite/InstancedSprite.h"
#include "scene/actor/Instanced.h"

namespace task {
namespace game {
namespace text {

InstancedText::InstancedText( const std::string& text, InstancedFont* font )
	: m_text( text ) {
	// create instance for every letter
	for ( const auto c : text ) {
		m_text_sprites.push_back( font->GetSymbolSprite( c ) );
	}
	m_instance_ids.reserve( m_text_sprites.size() );
}

InstancedText::~InstancedText() {
	Hide();
}

void InstancedText::ShowAt( const types::Vec3& coords ) {
	Hide();
	for ( size_t i = 0 ; i < m_text_sprites.size() ; i++ ) {
		m_instance_ids.push_back( m_text_sprites.at( i )->actor->AddInstance( coords ) );
		break; // TMP
	}
}

void InstancedText::Hide() {
	if ( !m_instance_ids.empty() ) {
		ASSERT_NOLOG( m_instance_ids.size() == m_text_sprites.size(), "instance ids size mismatch" );
		for ( size_t i = 0 ; i < m_instance_ids.size() ; i++ ) {
			m_text_sprites.at( i )->actor->RemoveInstance( m_instance_ids.at( i ) );
			break; // TMP
		}
		m_instance_ids.clear();
	}
}

}
}
}