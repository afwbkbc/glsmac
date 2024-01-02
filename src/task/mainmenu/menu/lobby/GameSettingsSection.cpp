#include "GameSettingsSection.h"

#include <algorithm>

#include "Lobby.h"

namespace task {
namespace mainmenu {
namespace lobby {

GameSettingsSection::GameSettingsSection( Lobby* lobby, ::game::GlobalSettings* game_settings )
	: LobbySection( lobby )
	, m_game_settings( game_settings ) {
	SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
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
	m_element_rows[ RI_MAP_TYPE ].choices->SetMode( Dropdown::DM_MENU );

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
		ASSERT( GetLobby()->GetPlayer()->GetRole() == ::game::Player::PR_HOST, "settings can only be locked for host" );
		Log( "Locking game settings" );
		m_is_locked = true;
		UpdateRows();
	}
}

void GameSettingsSection::Unlock() {
	if ( m_is_locked ) {
		ASSERT( GetLobby()->GetPlayer()->GetRole() == ::game::Player::PR_HOST, "settings can only be unlocked for host" );
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

	const auto& game_rules = m_game_settings->game_rules;

	::ui::object::ChoiceList::choices_t difficulty_levels = {};
	ChoiceList::value_t current_difficulty_level = 0;
	for ( auto& it : game_rules.m_difficulty_levels ) {
		if ( m_game_settings->global_difficulty.m_name == it.second.m_name ) {
			current_difficulty_level = it.first;
		}
		difficulty_levels.push_back(
			{
				it.first,
				it.second.m_name
			}
		);
	}
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

	ChoiceList::value_t game_type = 0;
	switch ( m_game_settings->map.type ) {
		case game::MapSettings::MT_RANDOM:
		case game::MapSettings::MT_CUSTOM: {
			HideRows( m_loadmap_rows );
			ShowRows( m_non_loadmap_rows );
			game_type = 0;
			break;
		}
		case game::MapSettings::MT_MAPFILE: {
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

	UpdateRow(
		RI_PLANET_SIZE, {
			{ ::game::MapSettings::MAP_TINY,     "Tiny Planet" },
			{ ::game::MapSettings::MAP_SMALL,    "Small Planet" },
			{ ::game::MapSettings::MAP_STANDARD, "Standard Planet" },
			{ ::game::MapSettings::MAP_LARGE,    "Large Planet" },
			{ ::game::MapSettings::MAP_HUGE,     "Huge Planet" },
		}, m_game_settings->map.size
	);

	UpdateRow(
		RI_PLANET_OCEAN, {
			{ ::game::MapSettings::MAP_OCEAN_LOW,    "30-50% of Surface" },
			{ ::game::MapSettings::MAP_OCEAN_MEDIUM, "50-70% of Surface" },
			{ ::game::MapSettings::MAP_OCEAN_HIGH,   "70-90% of Surface" },
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
	return m_is_locked || GetLobby()->GetPlayer()->GetRole() != ::game::Player::PR_HOST; // always locked for non-host
}

void GameSettingsSection::CreateRow( const row_id_t row_id, const std::string& label, const size_t label_width, const size_t choices_width ) {
	NEWV( label_el, ::ui::object::Label, "PopupLabel" );
	label_el->SetText( label );
	label_el->SetAlign( UIObject::ALIGN_LEFT );
	label_el->SetLeft( 6 );
	label_el->SetWidth( label_width );
	m_body->AddChild( label_el );
	NEWV( choices_el, ::ui::object::Dropdown, "PopupDropdown" );
	choices_el->SetAlign( UIObject::ALIGN_LEFT );
	choices_el->SetLeft( label_width );
	choices_el->SetWidth( choices_width );
	choices_el->On(
		UIEvent::EV_CHANGE, EH( this, choices_el, row_id ) {
			const auto& game_rules = m_game_settings->game_rules;
			switch ( row_id ) {
				case RI_DIFFICULTY_LEVEL:
					m_game_settings->global_difficulty = game_rules.m_difficulty_levels.at( data->value.change.id );
					break;
				case RI_TIME_CONTROLS:
					break;
				case RI_SPACE_1:
					break;
				case RI_MAP_TYPE:
					if ( data->value.change.id == 1 ) { // "load map"
						//choices_el->SetValue( 0 ); // keep "random map" until map is actually selected
						ShowLoadMap();
					}
					else { // "random map"
						m_game_settings->map.type = game::MapSettings::MT_RANDOM;
						m_game_settings->map.filename = "";
						UpdateRows();
						GetLobby()->UpdateGameSettings();
					}
					break;
				case RI_PLANET_SIZE:
					m_game_settings->map.size = data->value.change.id;
					break;
				case RI_PLANET_OCEAN:
					m_game_settings->map.ocean = data->value.change.id;
					break;
				case RI_PLANET_EROSIVE:
					m_game_settings->map.erosive = data->value.change.id;
					break;
				case RI_PLANET_LIFEFORMS:
					m_game_settings->map.lifeforms = data->value.change.id;
					break;
				case RI_PLANET_CLOUDS:
					m_game_settings->map.clouds = data->value.change.id;
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
	);
	m_body->AddChild( choices_el );
	m_element_rows[ row_id ] = {
		label_el,
		choices_el
	};
}

void GameSettingsSection::UpdateRow( const row_id_t row_id, const ::ui::object::ChoiceList::choices_t& choices, const ChoiceList::value_t default_choice ) {
	const auto& row = m_element_rows.at( row_id );
	if ( !IsLocked() && row_id != RI_MAP_FILE ) {
		row.choices->SetChoices( choices );
		row.choices->SetValue( default_choice );
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
	m_load_map.section->SetAlign( ::ui::object::UIObject::ALIGN_CENTER );
	m_load_map.section->On(
		UIEvent::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == UIEvent::K_ESCAPE ) {
				return m_load_map.button_cancel->Trigger( UIEvent::EV_BUTTON_CLICK, data );
			}
			return false;
		}
	);
	g_engine->GetUI()->AddObject( m_load_map.section );

	NEW( m_load_map.browser, ::ui::object::FileBrowser );
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
		m_load_map.browser->SetDefaultDirectory( util::FS::GetAbsolutePath( ::game::map::s_consts.fs.default_map_directory ) );
	}
	m_load_map.browser->SetFileExtension( ::game::map::s_consts.fs.default_map_extension );
	m_load_map.browser->On(
		UIEvent::EV_SELECT, EH( this ) {
			const auto& path = m_load_map.browser->GetSelectedFile();
			if ( !util::FS::FileExists( path ) ) {
				g_engine->GetUI()->Error(
					"File " + util::FS::GetBaseName( path ) + " does not exist!"
				);
			}
			else {
				ASSERT( util::FS::IsAbsolutePath( path ), "path must be absolute" );
				m_game_settings->map.type = game::MapSettings::MT_MAPFILE;
				m_game_settings->map.filename = path;
				UpdateRows();
				GetLobby()->UpdateGameSettings();
				HideLoadMap();
			}
			return true;
		}
	);
	m_load_map.section->AddChild( m_load_map.browser );
	NEW( m_load_map.button_ok, ::ui::object::Button, "PopupButtonOkCancel" );
	m_load_map.button_ok->SetAlign( ::ui::object::UIObject::ALIGN_BOTTOM | ::ui::object::UIObject::ALIGN_LEFT );
	m_load_map.button_ok->SetBottom( 8 );
	m_load_map.button_ok->SetLeft( 12 );
	m_load_map.button_ok->SetWidth( 206 );
	m_load_map.button_ok->SetLabel( "OK" );
	m_load_map.button_ok->On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
			UIEvent::event_data_t newdata = {};
			newdata.key.code = UIEvent::K_ENTER;
			return m_load_map.browser->Trigger( ui::event::UIEvent::EV_KEY_DOWN, &newdata );
		}
	);

	m_load_map.section->AddChild( m_load_map.button_ok );

	NEW( m_load_map.button_cancel, ::ui::object::Button, "PopupButtonOkCancel" );
	m_load_map.button_cancel->SetAlign( ::ui::object::UIObject::ALIGN_BOTTOM | ::ui::object::UIObject::ALIGN_RIGHT );
	m_load_map.button_cancel->SetBottom( 8 );
	m_load_map.button_cancel->SetRight( 12 );
	m_load_map.button_cancel->SetWidth( 206 );
	m_load_map.button_cancel->SetLabel( "CANCEL" );
	m_load_map.button_cancel->On(
		UIEvent::EV_BUTTON_CLICK, EH( this ) {
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
