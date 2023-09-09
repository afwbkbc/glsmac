#include "GameSettingsSection.h"

#include <algorithm>

#include "Lobby.h"

namespace task {
namespace mainmenu {
namespace lobby {

GameSettingsSection::GameSettingsSection( Lobby* lobby, ::game::GlobalSettings *game_settings )
	: LobbySection( lobby )
	, m_game_settings( game_settings )
{
	SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
}

void GameSettingsSection::Create() {
	Section::Create();

	ASSERT( m_element_rows.empty(), "element rows not empty" );
	m_element_rows.resize(RI_MAX );

	CreateRow( RI_DIFFICULTY_LEVEL,"Global Difficulty Level", 162, 120 );
	CreateRow( RI_TIME_CONTROLS, "Time Controls", 108, 90 );
	CreateRow( RI_GAME_TYPE, "Type of Game", 106, 140 );
	CreateRow( RI_PLANET_SIZE, "Planet Size", 106, 140 );
	CreateRow( RI_PLANET_OCEAN, "Ocean Coverage", 124, 164 );
	CreateRow( RI_PLANET_EROSIVE, "Erosive Forces", 124, 110 );
	CreateRow( RI_PLANET_LIFEFORMS, "Native Lifeforms", 124, 110 );
	CreateRow( RI_PLANET_CLOUDS, "Cloud Cover", 124, 110 );

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
}

void GameSettingsSection::Align() {
	SetWidth( 302 );
	SetHeight( 364 );

	Section::Align();
}

void GameSettingsSection::Destroy() {
	for ( auto& element_row : m_element_rows ) {
		if ( element_row.label ) {
			m_body->RemoveChild( element_row.label );
		}
		if ( element_row.choices ) {
			m_body->RemoveChild( element_row.choices );
		}
	}

	m_element_rows.clear();

	Section::Destroy();
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
		difficulty_levels.push_back({ it.first, it.second.m_name });
	}
	UpdateRow( RI_DIFFICULTY_LEVEL,
		difficulty_levels,
		current_difficulty_level
	);
	UpdateRow( RI_TIME_CONTROLS, {
		{ 0, "None" },
	}, 0 );

	UpdateRow( RI_GAME_TYPE, {
		{ 0, "Random Map" },
		{ 1, "Load Map"}
	}, 0 );

	UpdateRow( RI_PLANET_SIZE, {
		{ ::game::MapSettings::MAP_TINY, "Tiny Planet" },
		{ ::game::MapSettings::MAP_SMALL, "Small Planet" },
		{ ::game::MapSettings::MAP_STANDARD, "Standard Planet" },
		{ ::game::MapSettings::MAP_LARGE, "Large Planet" },
		{ ::game::MapSettings::MAP_HUGE, "Huge Planet" },
	}, m_game_settings->map.size );

	UpdateRow( RI_PLANET_OCEAN, {
		{ ::game::MapSettings::MAP_OCEAN_LOW, "30-50% of Surface" },
		{ ::game::MapSettings::MAP_OCEAN_MEDIUM, "50-70% of Surface" },
		{ ::game::MapSettings::MAP_OCEAN_HIGH, "70-90% of Surface" },
	}, m_game_settings->map.ocean );

	UpdateRow( RI_PLANET_EROSIVE, {
		{ 1, "Strong" },
		{ 2, "Average" },
		{ 3, "Weak" },
	}, m_game_settings->map.erosive );

	UpdateRow( RI_PLANET_LIFEFORMS, {
		{ 1, "Rare" },
		{ 2, "Average" },
		{ 3, "Abundant" },
	}, m_game_settings->map.lifeforms );

	UpdateRow( RI_PLANET_CLOUDS, {
		{ 1, "Sparse" },
		{ 2, "Average" },
		{ 3, "Dense" },
	}, m_game_settings->map.clouds );

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
	choices_el->On( UIEvent::EV_CHANGE, EH( this, row_id ) {
		const auto& game_rules = m_game_settings->game_rules;
		switch ( row_id ) {
			case RI_DIFFICULTY_LEVEL:
				m_game_settings->global_difficulty = game_rules.m_difficulty_levels.at( data->value.change.id );
				break;
			case RI_TIME_CONTROLS:
				break;
			case RI_SPACE_1:
				break;
			case RI_GAME_TYPE:
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
			case RI_MAX:
				break;
		}
		GetLobby()->UpdateGameSettings();
		return true;
	});
	m_body->AddChild( choices_el );
	m_element_rows[ row_id ] = { label_el, choices_el };
}

void GameSettingsSection::UpdateRow( const row_id_t row_id, const ::ui::object::ChoiceList::choices_t& choices, const ChoiceList::value_t default_choice ) {
	const auto& row = m_element_rows.at( row_id );
	if ( GetLobby()->GetPlayer()->GetRole() == ::game::Player::PR_HOST ) {
		row.choices->SetChoices( choices );
		row.choices->SetValue( default_choice );
	}
	else {
		for ( const auto& it : choices ) {
			if ( it.first == default_choice ) {
				row.choices->SetValue( it.second );
				return;
			}
		}
	}
}

}
}
}
