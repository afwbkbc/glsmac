#include "Popup.h"

#include "engine/Engine.h"
#include "loader/sound/SoundLoader.h"
#include "ui/UI.h"
#include "SoundEffect.h"

namespace ui {
namespace object {

Popup::Popup( const std::string& class_name )
	: UIContainer( class_name ) {
	ASSERT( !class_name.empty(), "popup class name can't be empty" );

	SetAlign( ALIGN_CENTER );
}

void Popup::Create() {
	UIContainer::Create();

	// TODO: fix blending
	// TODO: maybe not needed?
	/* NEW( m_background_locker, ui::object::Surface, "PopupBackground" );
	g_engine->GetUI()->AddObject( m_background_locker ); */

	auto* soundloader = g_engine->GetSoundLoader();

	NEW( m_sounds.open, SoundEffect );
	m_sounds.open->SetSound( soundloader->LoadSound( resource::WAV_MENU_UP ) );
	m_sounds.open->SetVolume( 0.4f );
	AddChild( m_sounds.open );
	NEW( m_sounds.close, SoundEffect );
	m_sounds.close->SetSound( soundloader->LoadSound( resource::WAV_MENU_DOWN ) );
	m_sounds.open->SetVolume( 0.6f );
	AddChild( m_sounds.close );

	PlayOpenSound();
}

void Popup::Destroy() {

	PlayCloseSound();

	RemoveChild( m_sounds.open );
	RemoveChild( m_sounds.close );

	// TODO: fix blending
	// TODO: maybe not needed?
	//g_engine->GetUI()->RemoveObject( m_background_locker );

	OnClose();

	UIContainer::Destroy();
}

void Popup::Open() {
	ASSERT( !m_is_opened, "popup already opened" );
	m_is_opened = true;

	OnOpen();

	g_engine->GetUI()->OpenPopup( this );
}

void Popup::Close() {
	ASSERT( m_is_opened, "popup already closed" );
	g_engine->GetUI()->ClosePopup( this );
}

bool Popup::MaybeClose() {
	ASSERT( m_is_opened, "maybeclose but popup not opened" );
	m_is_opened = false;
	return true;
}

void Popup::PlayOpenSound() {
	if ( !m_sounds_played.open ) {
		m_sounds_played.open = true;
		m_sounds.open->Play();
	}
}

void Popup::PlayCloseSound() {
	if ( !m_sounds_played.close ) {
		m_sounds_played.close = true;
		m_sounds.close->Play();
	}
}

}
}
