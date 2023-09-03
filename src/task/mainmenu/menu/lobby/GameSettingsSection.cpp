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
	for ( auto& it : game_rules.m_difficulty_levels ) {
		difficulty_levels.push_back({ it.first, it.second.m_name });
	}

	UpdateRow( RI_DIFFICULTY_LEVEL,
		difficulty_levels,
		m_game_settings->global_difficulty.m_name
	);
	UpdateRow( RI_TIME_CONTROLS, {
		{ 0, "None" },
	}, "None" );

	UpdateRow( RI_GAME_TYPE, {
		{ 0, "Random Map" },
		{ 1, "Load Map"}
	}, "Random Map" );

	UpdateRow( RI_PLANET_SIZE, {
		{ 1, "Tiny Planet" },
		{ 2, "Small Planet" },
		{ 3, "Standard Planet" },
		{ 4, "Large Planet" },
		{ 5, "Huge Planet" },
	}, "Standard Planet" );

	UpdateRow( RI_PLANET_OCEAN, {
		{ 1, "30-50% of Surface" },
		{ 2, "50-70% of Surface" },
		{ 3, "70-90% of Surface" },
	}, "50-70% of Surface" );

	UpdateRow( RI_PLANET_EROSIVE, {
		{ 1, "Strong" },
		{ 2, "Average" },
		{ 3, "Weak" },
	}, "Average" );

	UpdateRow( RI_PLANET_LIFEFORMS, {
		{ 1, "Rare" },
		{ 2, "Average" },
		{ 3, "Abundant" },
	}, "Average" );

	UpdateRow( RI_PLANET_CLOUDS, {
		{ 1, "Sparse" },
		{ 2, "Average" },
		{ 3, "Dense" },
	}, "Average" );

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
	choices_el->On( UIEvent::EV_CHANGE, EH( this ) {
		const auto& game_rules = m_game_settings->game_rules;
		m_game_settings->global_difficulty = game_rules.m_difficulty_levels.at( data->value.change.id );
		GetLobby()->UpdateGameSettings();
		return true;
	});
	m_body->AddChild( choices_el );
	m_element_rows[ row_id ] = { label_el, choices_el };
}

void GameSettingsSection::UpdateRow( const row_id_t row_id, const ::ui::object::ChoiceList::choices_t& choices, const std::string& default_choice ) {
	const auto& row = m_element_rows.at( row_id );

	if ( GetLobby()->GetPlayer()->GetRole() == ::game::Player::PR_HOST ) {
		row.choices->SetChoices( choices );
	}

	if ( !default_choice.empty() ) {
		row.choices->SetValue( default_choice );
	}
}

}
}
}
