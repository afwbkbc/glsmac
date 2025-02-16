#include <cmath>
#include <algorithm>

#include "MainMenu.h"

#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "scheduler/Scheduler.h"
#include "loader/texture/TextureLoader.h"
#include "task/game/Game.h"
#include "game/backend/Player.h"
#include "game/backend/slot/Slots.h"
#include "menu/lobby/Lobby.h"
#include "menu/Main.h"
#include "menu/Error.h"
#include "game/backend/State.h"
#include "util/random/Random.h"
#include "ui_legacy/Theme.h"
#include "ui_legacy/UI.h"
#include "ui_legacy/object/Surface.h"
#include "ui_legacy/object/Label.h"
#include "ui_legacy/object/SoundEffect.h"

#include "version.h"

namespace task {
namespace mainmenu {

void MainMenu::Start() {
	ASSERT( !m_state, "mainmenu state already set" );
	NEW( m_state, ::game::backend::State, nullptr );

	NEW( m_random, util::random::Random );

	auto* ui = g_engine->GetUI();

	NEW( m_theme, Theme );
	ui->AddTheme( m_theme );

	// background
	NEW( m_background, ui_legacy::object::Surface, "MainMenuBackground" );
	ui->AddObject( m_background );

	// preview image for 'customize map'
	NEW( m_customize_map_preview, ui_legacy::object::Surface, "MainMenuCustomizeMapPreview" );
	m_customize_map_preview->Hide();
	ui->AddObject( m_customize_map_preview );
	SetCustomizeMapPreview( "S2L2C2" ); // average preview by default

	NEW( m_customize_map_moons, ui_legacy::object::Surface, "MainMenuCustomizeMapMoons" );
	m_customize_map_moons->Hide();
	ui->AddObject( m_customize_map_moons );

	ResizeCustomizeMapPreview();

	NEW( m_glsmac_logo, ui_legacy::object::Label, "MainMenuGLSMACLogo" );
	m_glsmac_logo->SetText( GLSMAC_VERSION_FULL );
	ui->AddObject( m_glsmac_logo );

	m_mouse_handler = ui->AddGlobalEventHandler(
		ui_legacy::event::EV_MOUSE_DOWN, EH( this ) {
			// rightclick = back
			if ( data->mouse.button == ui_legacy::event::M_RIGHT && m_menu_object ) {
				if ( !m_menu_history.empty() ) { // don't exit game on right-click
					return m_menu_object->MaybeClose(); // only sliding menus will close on right click
				}
			}
			return false;
		}, ui_legacy::UI::GH_BEFORE
	);

	m_key_handler = ui->AddGlobalEventHandler(
		ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			// escape = back
			if ( !data->key.modifiers && data->key.code == ui_legacy::event::K_ESCAPE && m_menu_object ) {
				return m_menu_object->MaybeClose(); // popups have their own escape handler
			}
			return false;
		}, ui_legacy::UI::GH_BEFORE
	);

	NEW( m_music, ui_legacy::object::SoundEffect, "MainMenuMusic" );
	g_engine->GetUI()->AddObject( m_music );

	g_engine->GetGraphics()->AddOnWindowResizeHandler(
		this, RH( this ) {
			if ( m_menu_object ) {
				m_menu_object->Align();
			}
			ResizeCustomizeMapPreview();
		}
	);

	NEWV( menu, Main, this );
	ShowMenu( menu );
}

void MainMenu::Iterate() {
	if ( m_goback ) {
		m_goback = false;
		if ( !m_menu_history.empty() ) {
			if ( !m_customize_map_preview_history.empty() ) {
				m_customize_map_preview_filename = m_customize_map_preview_history.back();
				m_customize_map_preview_history.pop_back();
				SetCustomizeMapPreview( m_customize_map_preview_filename );
			}
			m_menu_object->Hide();
			DELETE( m_menu_object );
			m_menu_object = m_menu_history.back();
			m_menu_history.pop_back();
			m_menu_object->Show();
			m_menu_object->SetChoice( m_choice_history.back() );
			m_choice_history.pop_back();
		}
		else {
			g_engine->ShutDown();
		}
	}
	else if ( m_menu_next ) {
		if ( m_menu_object ) {
			m_customize_map_preview_history.push_back( m_customize_map_preview_filename );
			m_choice_history.push_back( m_menu_object->GetChoice() );
			m_menu_object->Hide();
			m_menu_history.push_back( m_menu_object );
		}
		m_menu_object = m_menu_next;
		m_menu_next = nullptr;
		m_menu_object->Show();
	}
	else {
		if ( m_menu_object ) {
			m_menu_object->Iterate();
		}
		if ( !m_show_error_on_start.empty() ) {
			MenuError( m_show_error_on_start );
			m_show_error_on_start.clear();
		}
	}
}

void MainMenu::Stop() {

	g_engine->GetGraphics()->RemoveOnWindowResizeHandler( this );

	if ( m_menu_object ) {
		m_menu_object->Hide();
		DELETE( m_menu_object );
	}

	for ( auto& it : m_menu_history ) {
		DELETE( it );
	}
	m_menu_history.clear();

	if ( m_menu_next ) {
		DELETE( m_menu_next );
		m_menu_next = nullptr;
	}

	if ( m_state ) {
		DELETE( m_state );
		m_state = nullptr;
	}

	g_engine->GetUI()->RemoveObject( m_music );

	auto* ui = g_engine->GetUI();

	ui->RemoveGlobalEventHandler( m_mouse_handler );
	ui->RemoveGlobalEventHandler( m_key_handler );

	ui->RemoveObject( m_background );
	ui->RemoveObject( m_customize_map_preview );
	ui->RemoveObject( m_customize_map_moons );

	ui->RemoveObject( m_glsmac_logo );

	ui->RemoveTheme( m_theme );
	DELETE( m_theme );

	DELETE( m_random );
}

void MainMenu::GoBack() {
	ASSERT( !m_goback, "goback already set" );
	m_goback = true;
}

void MainMenu::ShowMenu( MenuObject* menu_object ) {
	ASSERT( !m_menu_next, "next menu already set" );
	m_menu_next = menu_object;
}

void MainMenu::MenuError( const std::string& error_text ) {
	NEWV( menu, Error, this, error_text );
	ShowMenu( menu );
}

void MainMenu::InitSinglePlayer() {
	m_state->m_slots->Resize( 7 ); // TODO: make dynamic?
	const auto& rules = m_state->m_settings.global.game_rules;
	m_state->m_settings.local.player_name = "Player";
	NEWV( player, ::game::backend::Player,
		m_state->m_settings.local.player_name,
		::game::backend::Player::PR_SINGLE,
		{},
		rules.GetDefaultDifficultyLevel() // TODO: make configurable
	);
	m_state->AddPlayer( player );
	size_t slot_num = 0; // player always has slot 0
	m_state->AddCIDSlot( 0, slot_num ); // for consistency
	auto& slot = m_state->m_slots->GetSlot( slot_num );
	slot.SetPlayer( player, 0, "" );
	slot.SetPlayerFlag( ::game::backend::slot::PF_READY );
}

void MainMenu::StartGame() {
	// real state belongs to game task now
	// save it as backup, then make temporary shallow copy (no connection, players etc)
	//   just for the sake of passing settings to previous menu
	auto* real_state = m_state;
	NEW( m_state, ::game::backend::State, nullptr );
	m_state->m_settings = real_state->m_settings;
	NEWV( task, task::game::Game, real_state, UH( this ) {
		g_engine->GetScheduler()->RemoveTask( this );
	}, UH( this, real_state ) {
		m_menu_object->MaybeClose();
		//m_state->Reset();
	} );
	g_engine->GetScheduler()->AddTask( task );
}

void MainMenu::ShowErrorOnStart( const std::string& error ) {
	ASSERT( m_show_error_on_start.empty(), "m_show_error_on_start already set" );
	m_show_error_on_start = error;
}

void MainMenu::SetCustomizeMapPreview( const std::string& preview_filename ) {
	//Log( "Set customize map preview to " + preview_filename );
	m_customize_map_preview_filename = preview_filename;
	m_customize_map_preview->SetTexture( g_engine->GetTextureLoader()->LoadTexture( GetPreviewResource( preview_filename ) ) );
	m_customize_map_preview->SetStretchTexture( true );
	m_customize_map_preview->Show();
}

const std::string& MainMenu::GetMapPreviewFilename() const {
	return m_customize_map_preview_filename;
}

void MainMenu::SetCustomizeMapMoons( const uint8_t moons_count ) {
	if ( moons_count > 0 ) {
		Log( "Set customize map moons to " + std::to_string( moons_count ) );
		m_customize_map_moons->SetTexture( g_engine->GetTextureLoader()->LoadTexture( GetMoonsResource( moons_count ) ) );
		m_customize_map_moons->SetStretchTexture( true );
		m_customize_map_moons->Show();
	}
	else {
		Log( "Hiding customize map moons" );
		m_customize_map_moons->Hide();
	}
}

util::random::Random* MainMenu::GetRandom() {
	return m_random;
}

void MainMenu::ResizeCustomizeMapPreview() {
	const auto* g = g_engine->GetGraphics();
	const auto w = g->GetViewportWidth();
	const auto h = g->GetViewportHeight();
	if ( m_customize_map_preview ) {
		m_customize_map_preview->SetWidth( floor( 416.0f / 1024.0f * w ) );
		m_customize_map_preview->SetHeight( floor( h ) );
	}
	if ( m_customize_map_moons ) {
		m_customize_map_moons->SetWidth( floor( 450.0f / 1024.0f * w ) );
		m_customize_map_moons->SetHeight( floor( 450.0f / 768.0f * h ) );
	}
}

const resource::resource_t MainMenu::GetPreviewResource( const std::string& preview_filename ) const {
	ASSERT( preview_filename.length() == 6 && preview_filename[ 0 ] == 'S' && preview_filename[ 2 ] == 'L' && preview_filename[ 4 ] == 'C', "unexpected preview filename format: " + preview_filename );
#define xSLC( _s, _l, _c ) \
    if ( preview_filename[1] == (_s+'0') && preview_filename[3] == (_l+'0') && preview_filename[5] == (_c+'0') ) return resource::PCX_S##_s##L##_l##C##_c;
#define xSL( _s, _l ) \
    xSLC( _s, _l, 1 ) \
    xSLC( _s, _l, 2 ) \
    xSLC( _s, _l, 3 )
#define xS( _s ) \
    xSL( _s, 1 ) \
    xSL( _s, 2 ) \
    xSL( _s, 3 )
	xS( 1 )
	xS( 2 )
	xS( 3 )
#undef xS
#undef xSL
#undef xSLC
	THROW( "could not match filename to resource: " + preview_filename );
}

const resource::resource_t MainMenu::GetMoonsResource( const uint8_t moons_count ) const {
	ASSERT( moons_count < 4, "unexpected moons count: " + std::to_string( moons_count ) );
	switch ( moons_count ) {
#define xM( _m ) \
        case _m: return resource::PCX_MOON##_m;
		xM( 1 )
		xM( 2 )
		xM( 3 )
#undef xM
		default:
			THROW( "could not match moons to resource: " + std::to_string( moons_count ) );
	}
}

}
}
