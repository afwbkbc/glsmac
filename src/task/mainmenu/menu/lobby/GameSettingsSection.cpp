#include "GameSettingsSection.h"

#include <algorithm>

#include "Lobby.h"
#include "ui_legacy/object/Label.h"
#include "game/backend/Player.h"
#include "game/backend/settings/Settings.h"
#include "util/FS.h"
#include "engine/Engine.h"
#include "ui_legacy/object/Button.h"
#include "ui_legacy/UI.h"
#include "ui_legacy/object/FileBrowser.h"
#include "game/backend/map/Consts.h"

namespace task {
namespace mainmenu {
namespace lobby {

GameSettingsSection::GameSettingsSection( Lobby* lobby, game::backend::settings::GlobalSettings* game_settings )
	: LobbySection( lobby )
	, m_game_settings( game_settings ) {
	SetAlign( ui_legacy::ALIGN_LEFT | ui_legacy::ALIGN_TOP );
	SetTitleText( " " ); // to have header created
}

void GameSettingsSection::Create() {
	LobbySection::Create();

	ASSERT( m_element_rows.empty(), "element rows not empty" );
	m_element_rows.resize( RI_MAX );

	CreateRow( RI_DIFFICULTY_LEVEL, "Global Difficulty Level", 162, 120 );
	CreateRow( RI_TIME_CONTROLS, "Time Controls", 108, 90 );
	CreateRow( RI_MAP_TYPE, "Type of Game", 106, 140 );
	CreateRow( RI_PLANET_SIZE, "Planet Size", 106, 140 );
	CreateRow( RI_PLANET_OCEAN, "Ocean Coverage", 124, 164 );
	CreateRow( RI_PLANET_EROSIVE, "Erosive Forces", 124, 110 );
	CreateRow( RI_PLANET_LIFEFORMS, "Native Lifeforms", 124, 110 );
	CreateRow( RI_PLANET_CLOUDS, "Cloud Cover", 124, 110 );

	CreateRow( RI_MAP_FILE, "Map Filename", 106, 170 );

	size_t top = 2;
	for ( auto& element_row : m_element_rows ) {
		if ( element_row.label ) {
			element_row.label->SetTop( top + 2 );
		}
		if ( element_row.choices ) {
			element_row.choices->SetTop( top );
		}
		top += 23;
	}

	// to allow selecting Load Map repeatedly
	m_element_rows[ RI_MAP_TYPE ].choices->SetMode( ui_legacy::object::NumDropdown::DM_MENU );

	// these two are paginated based on "Random Map"/"Load Map" choice
	m_element_rows[ RI_MAP_FILE ].label->SetTop( m_element_rows[ RI_PLANET_SIZE ].label->GetTop() );
	m_element_rows[ RI_MAP_FILE ].choices->SetTop( m_element_rows[ RI_PLANET_SIZE ].choices->GetTop() );
}

void GameSettingsSection::Align() {
	SetWidth( 302 );
	SetHeight( 364 );

	LobbySection::Align();
}

void GameSettingsSection::Destroy() {
	HideLoadMap();
	for ( auto& element_row : m_element_rows ) {
		if ( element_row.label ) {
			m_body->RemoveChild( element_row.label );
		}
		if ( element_row.choices ) {
			m_body->RemoveChild( element_row.choices );
		}
	}

	m_element_rows.clear();

	LobbySection::Destroy();
}

void GameSettingsSection::Lock() {
	if ( !m_is_locked ) {
		ASSERT( GetLobby()->GetPlayer()->GetRole() == game::backend::Player::PR_HOST, "settings can only be locked for host" );
		Log( "Locking game settings" );
		m_is_locked = true;
		UpdateRows();
	}
}

void GameSettingsSection::Unlock() {
	if ( m_is_locked ) {
		ASSERT( GetLobby()->GetPlayer()->GetRole() == game::backend::Player::PR_HOST, "settings can only be unlocked for host" );
		Log( "Unlocking game settings" );
		m_is_locked = false;
		UpdateRows();
	}
}

void GameSettingsSection::UpdateRows() {

	ASSERT( !m_element_rows.empty(), "element rows are empty" );

	std::string title = m_game_settings->game_name;
	std::transform( title.begin(), title.end(), title.begin(), ::toupper );
	SetTitleText( title );

	const auto& rules = m_game_settings->rules;

	::ui_legacy::object::NumChoiceList::choices_t difficulty_levels = {};
	ui_legacy::object::NumChoiceList::value_t current_difficulty_level = 0;
	THROW( "deprecated" );
	/*
	for ( auto& it : rules.m_difficulty_levels ) {
		if ( m_game_settings->difficulty_level.m_name == it.second.m_name ) {
			current_difficulty_level = it.first;
		}
		difficulty_levels.push_back(
			{
				it.first,
				it.second.m_name
			}
		);
	}*/
	UpdateRow(
		RI_DIFFICULTY_LEVEL,
		difficulty_levels,
		current_difficulty_level
	);
	UpdateRow(
		RI_TIME_CONTROLS, {
			{
				0,
				"None"
			},
		}, 0
	);

	ui_legacy::object::NumChoiceList::value_t game_type = 0;
	switch ( m_game_settings->map.type ) {
		case game::backend::settings::MapSettings::MT_RANDOM:
		case game::backend::settings::MapSettings::MT_CUSTOM: {
			HideRows( m_loadmap_rows );
			ShowRows( m_non_loadmap_rows );
			game_type = 0;
			break;
		}
		case game::backend::settings::MapSettings::MT_MAPFILE: {
			HideRows( m_non_loadmap_rows );
			ShowRows( m_loadmap_rows );
			game_type = 1;
			break;
		}
		default:
			THROW( "unknown map type" );
	}

	UpdateRow(
		RI_MAP_TYPE, {
			{ 0, "Random Map" },
			{ 1, "Load Map" }
		}, game_type
	);

	/* deprecated
	UpdateRow(
		RI_PLANET_SIZE, {
			{ 0, "Tiny Planet" },
			{ 1, "Small Planet" },
			{ 2, "Standard Planet" },
			{ 3, "Large Planet" },
			{ 4, "Huge Planet" },
		}, m_game_settings->map.size
	);

	UpdateRow(
		RI_PLANET_OCEAN, {
			{ 0, "30-50% of Surface" },
			{ 1, "50-70% of Surface" },
			{ 2, "70-90% of Surface" },
		}, m_game_settings->map.ocean
	);

	UpdateRow(
		RI_PLANET_EROSIVE, {
			{ 1, "Strong" },
			{ 2, "Average" },
			{ 3, "Weak" },
		}, m_game_settings->map.erosive
	);

	UpdateRow(
		RI_PLANET_LIFEFORMS, {
			{ 1, "Rare" },
			{ 2, "Average" },
			{ 3, "Abundant" },
		}, m_game_settings->map.lifeforms
	);

	UpdateRow(
		RI_PLANET_CLOUDS, {
			{ 1, "Sparse" },
			{ 2, "Average" },
			{ 3, "Dense" },
		}, m_game_settings->map.clouds
	);
*/
	UpdateRow(
		RI_MAP_FILE, {
			{
				0,
				util::FS::GetBaseName( m_game_settings->map.filename )
			},
		}, 0
	);
}

const bool GameSettingsSection::IsLocked() const {
	return m_is_locked || GetLobby()->GetPlayer()->GetRole() != game::backend::Player::PR_HOST; // always locked for non-host
}

void GameSettingsSection::CreateRow( const row_id_t row_id, const std::string& label, const size_t label_width, const size_t choices_width ) {
	NEWV( label_el, ::ui_legacy::object::Label, "PopupLabel" );
	label_el->SetText( label );
	label_el->SetAlign( ui_legacy::ALIGN_LEFT );
	label_el->SetLeft( 6 );
	label_el->SetWidth( label_width );
	m_body->AddChild( label_el );
	NEWV( choices_el, ::ui_legacy::object::NumDropdown, "PopupDropdown" );
	choices_el->SetAlign( ui_legacy::ALIGN_LEFT );
	choices_el->SetLeft( label_width );
	choices_el->SetWidth( choices_width );
	THROW( "DEPRECATED" );
	/*choices_el->On(
		ui_legacy::event::EV_CHANGE, EH( this, choices_el, row_id ) {
			const auto& rules = m_game_settings->rules;
			switch ( row_id ) {
				case RI_DIFFICULTY_LEVEL:
					m_game_settings->difficulty_level = *data->value.change.id_str;
					break;
				case RI_TIME_CONTROLS:
					break;
				case RI_SPACE_1:
					break;
				case RI_MAP_TYPE:
					if ( data->value.change.id_num == 1 ) { // "load map"
						//choices_el->SetValue( 0 ); // keep "random map" until map is actually selected
						ShowLoadMap();
					}
					else { // "random map"
						m_game_settings->map.type = game::backend::settings::MapSettings::MT_RANDOM;
						m_game_settings->map.filename = "";
						UpdateRows();
						GetLobby()->UpdateGameSettings();
					}
					break;
				case RI_PLANET_SIZE:
					switch ( data->value.change.id_num ) {
						case 0: {
							m_game_settings->map.size_x = 68;
							m_game_settings->map.size_y = 34;
							break;
						}
						case 1: {
							m_game_settings->map.size_x = 88;
							m_game_settings->map.size_y = 44;
							break;
						}
						case 2: {
							m_game_settings->map.size_x = 112;
							m_game_settings->map.size_y = 56;
							break;
						}
						case 3: {
							m_game_settings->map.size_x = 140;
							m_game_settings->map.size_y = 70;
							break;
						}
						case 4: {
							m_game_settings->map.size_x = 180;
							m_game_settings->map.size_y = 90;
							break;
						}
					}
					break;
				case RI_PLANET_OCEAN:
					switch ( data->value.change.id_num ) {
						case 0: {
							m_game_settings->map.ocean_coverage = 0.4f;
							break;
						}
						case 1: {
							m_game_settings->map.ocean_coverage = 0.6f;
							break;
						}
						case 2: {
							m_game_settings->map.ocean_coverage = 0.8f;
							break;
						}
					}
					break;
				case RI_PLANET_EROSIVE:
					switch ( data->value.change.id_num ) {
						case 0: {
							m_game_settings->map.erosive_forces = 0.5f;
							break;
						}
						case 1: {
							m_game_settings->map.erosive_forces = 0.75f;
							break;
						}
						case 2: {
							m_game_settings->map.erosive_forces = 1.0f;
							break;
						}
					}
					break;
				case RI_PLANET_LIFEFORMS:
					switch ( data->value.change.id_num ) {
						case 0: {
							m_game_settings->map.native_lifeforms = 0.25f;
							break;
						}
						case 1: {
							m_game_settings->map.native_lifeforms = 0.5f;
							break;
						}
						case 2: {
							m_game_settings->map.native_lifeforms = 0.75f;
							break;
						}
					}
					break;
				case RI_PLANET_CLOUDS:
					switch ( data->value.change.id_num ) {
						case 0: {
							m_game_settings->map.cloud_cover = 0.25f;
							break;
						}
						case 1: {
							m_game_settings->map.cloud_cover = 0.5f;
							break;
						}
						case 2: {
							m_game_settings->map.cloud_cover = 0.75f;
							break;
						}
					}
					break;
				case RI_MAP_FILE:
					// nothing to do yet because it will just open file browser
					break;
				case RI_MAX:
					break;
			}
			GetLobby()->UpdateGameSettings();
			return true;
		}
	);*/
	m_body->AddChild( choices_el );
	m_element_rows[ row_id ] = {
		label_el,
		choices_el
	};
}

void GameSettingsSection::UpdateRow( const row_id_t row_id, const ::ui_legacy::object::NumChoiceList::choices_t& choices, const ::ui_legacy::object::NumChoiceList::value_t default_choice ) {
	const auto& row = m_element_rows.at( row_id );
	if ( !IsLocked() && row_id != RI_MAP_FILE ) {
		row.choices->SetChoices( choices );
		row.choices->SetValueByKey( default_choice );
	}
	else {
		row.choices->SetChoices( {} );
		for ( const auto& it : choices ) {
			if ( it.first == default_choice ) {
				row.choices->SetValue( it.second );
				return;
			}
		}
	}
}

void GameSettingsSection::HideRows( const std::vector< row_id_t > row_ids ) {
	for ( const auto& row_id : row_ids ) {
		m_element_rows[ row_id ].label->Hide();
		m_element_rows[ row_id ].choices->Hide();
	}
}

void GameSettingsSection::ShowRows( const std::vector< row_id_t > row_ids ) {
	for ( const auto& row_id : row_ids ) {
		m_element_rows[ row_id ].label->Show();
		m_element_rows[ row_id ].choices->Show();
	}
}

void GameSettingsSection::ShowLoadMap() {
	ASSERT( !m_load_map.browser, "load map already visible" );

	GetLobby()->LockInput();

	NEW( m_load_map.section, Section, "PopupSection" );
	m_load_map.section->SetTitleText( "LOAD MAP" );
	m_load_map.section->SetHeight( 520 );
	m_load_map.section->SetWidth( 450 );
	m_load_map.section->SetZIndex( 0.7f );
	m_load_map.section->SetAlign( ui_legacy::ALIGN_CENTER );
	m_load_map.section->On(
		ui_legacy::event::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == ui_legacy::event::K_ESCAPE ) {
				return m_load_map.button_cancel->Trigger( ui_legacy::event::EV_BUTTON_CLICK, data );
			}
			return false;
		}
	);
	g_engine->GetUI()->AddObject( m_load_map.section );

	NEW( m_load_map.browser, ::ui_legacy::object::FileBrowser );
	// TODO: determine position from style
	m_load_map.browser->SetTop( 4 );
	m_load_map.browser->SetLeft( 4 );
	m_load_map.browser->SetRight( 4 );
	m_load_map.browser->SetBottom( 36 );
	const auto& filename = m_game_settings->map.filename;
	if ( !filename.empty() ) {
		m_load_map.browser->SetDefaultDirectory( util::FS::GetDirName( filename ) );
		m_load_map.browser->SetDefaultFilename( util::FS::GetBaseName( filename ) );
	}
	else {
		m_load_map.browser->SetDefaultDirectory( util::FS::GetAbsolutePath( game::backend::map::s_consts.fs.default_map_directory ) );
	}
	m_load_map.browser->SetFileExtension( game::backend::map::s_consts.fs.default_map_extension );
	m_load_map.browser->On(
		ui_legacy::event::EV_SELECT, EH( this ) {
			const auto& path = m_load_map.browser->GetSelectedFile();
			if ( !util::FS::FileExists( path ) ) {
				g_engine->GetUI()->Error(
					"File " + util::FS::GetBaseName( path ) + " does not exist!"
				);
			}
			else {
				ASSERT( util::FS::IsAbsolutePath( path ), "path must be absolute" );
				m_game_settings->map.type = game::backend::settings::MapSettings::MT_MAPFILE;
				m_game_settings->map.filename = path;
				UpdateRows();
				GetLobby()->UpdateGameSettings();
				HideLoadMap();
			}
			return true;
		}
	);
	m_load_map.section->AddChild( m_load_map.browser );
	NEW( m_load_map.button_ok, ::ui_legacy::object::Button, "PopupButtonOkCancel" );
	m_load_map.button_ok->SetAlign( ::ui_legacy::ALIGN_BOTTOM | ::ui_legacy::ALIGN_LEFT );
	m_load_map.button_ok->SetBottom( 8 );
	m_load_map.button_ok->SetLeft( 12 );
	m_load_map.button_ok->SetWidth( 206 );
	m_load_map.button_ok->SetLabel( "OK" );
	m_load_map.button_ok->On(
		ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			ui_legacy::event::event_data_t newdata = {};
			newdata.key.code = ui_legacy::event::K_ENTER;
			return m_load_map.browser->Trigger( ui_legacy::event::EV_KEY_DOWN, &newdata );
		}
	);

	m_load_map.section->AddChild( m_load_map.button_ok );

	NEW( m_load_map.button_cancel, ::ui_legacy::object::Button, "PopupButtonOkCancel" );
	m_load_map.button_cancel->SetAlign( ::ui_legacy::ALIGN_BOTTOM | ::ui_legacy::ALIGN_RIGHT );
	m_load_map.button_cancel->SetBottom( 8 );
	m_load_map.button_cancel->SetRight( 12 );
	m_load_map.button_cancel->SetWidth( 206 );
	m_load_map.button_cancel->SetLabel( "CANCEL" );
	m_load_map.button_cancel->On(
		ui_legacy::event::EV_BUTTON_CLICK, EH( this ) {
			HideLoadMap();
			return true;
		}
	);
	m_load_map.section->AddChild( m_load_map.button_cancel );

}

void GameSettingsSection::HideLoadMap() {
	if ( m_load_map.section ) {
		m_load_map.section->RemoveChild( m_load_map.browser );
		m_load_map.browser = nullptr;
		m_load_map.section->RemoveChild( m_load_map.button_ok );
		m_load_map.button_ok = nullptr;
		m_load_map.section->RemoveChild( m_load_map.button_cancel );
		m_load_map.button_cancel = nullptr;
		g_engine->GetUI()->RemoveObject( m_load_map.section );
		m_load_map.section = nullptr;
		GetLobby()->UnlockInput();
	}
}

}
}
}
