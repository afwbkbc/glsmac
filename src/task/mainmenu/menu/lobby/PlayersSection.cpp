#include "PlayersSection.h"

#include "engine/Engine.h"

namespace task {
namespace mainmenu {
namespace lobby {

PlayersSection::PlayersSection( Lobby* lobby )
	: Section( "PopupSection" )
	, m_lobby( lobby )
{
	
}

void PlayersSection::Create() {
	Section::Create();
	
}

void PlayersSection::Destroy() {
	
	ClearPlayers();
	
	Section::Destroy();
}

void PlayersSection::ClearPlayers() {
	for ( auto& label : m_player_labels ) {
		RemoveChild( label.second );
	}
	m_player_labels.clear();
}

void PlayersSection::SetPlayers( const players_t& players ) {
	
	ClearPlayers();
	
	for ( auto& player : players ) {
		// TODO: styles etc
		NEWV( label, Label );
			label->SetText( player.second.GetName() );
			label->SetFont( g_engine->GetFontLoader()->LoadFont( "arialnb.ttf", 18 ) );
			label->SetTextColor( Color::FromRGB( 192, 192, 192 ) );
			label->SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
			label->SetLeft( 10 );
			label->SetTop( 6 + m_player_labels.size() * 22 );
		AddChild( label );
		m_player_labels[ player.first ] = label;
	}
	
}

}
}
}
