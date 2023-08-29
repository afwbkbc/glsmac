#include "GameSettingsSection.h"

#include <algorithm>

#include "Lobby.h"

namespace task {
namespace mainmenu {
namespace lobby {

GameSettingsSection::GameSettingsSection( Lobby* lobby, ::game::GlobalSettings* game_settings )
	: LobbySection( lobby )
	, m_game_settings( game_settings )
{
	std::string title = game_settings->game_name;
	std::transform( title.begin(), title.end(), title.begin(), ::toupper );
	SetTitleText( title ); // to have header created
	SetAlign( UIObject::ALIGN_LEFT | UIObject::ALIGN_TOP );
}

void GameSettingsSection::Create() {
	Section::Create();

	ASSERT( m_element_rows.empty(), "element rows not empty" );
	m_element_rows.resize(RI_MAX );

	const auto& game_rules = GetLobby()->GetSettings().global.game_rules;

	::ui::object::ChoiceList::choices_t difficulty_levels = {};
	for ( auto& difficulty : game_rules.m_difficulty_levels ) {
		difficulty_levels.push_back({ difficulty.first, difficulty.second.m_name });
	}

	// TODO: put actual values

	SetRow(
		RI_DIFFICULTY_LEVEL,
		"Global Difficulty Level", 162,
		{ difficulty_levels },120
	);

	SetRow(
		RI_TIME_CONTROLS,
		"Time Controls", 108,
		{ { 0, "None" } }, 90 // TODO
	);

	SetRow(
		RI_GAME_TYPE,
		"Type of Game", 106,
		{ { 1, "Random Map" }, { 2, "Load Map"} }, 140
	);

	SetRow(
		RI_PLANET_SIZE,
		"Planet Size", 106,
		{
			{ 1, "Tiny Planet" },
			{ 2, "Small Planet" },
			{ 3, "Standard Planet" },
			{ 4, "Large Planet" },
			{ 5, "Huge Planet" },
		}, 140
	);

	SetRow(
		RI_PLANET_OCEAN,
		"Ocean Coverage", 124,
		{
			{ 1, "30-50% of Surface" },
			{ 2, "50-70% of Surface" },
			{ 3, "70-90% of Surface" },
		}, 164
	);

	SetRow(
		RI_PLANET_EROSIVE,
		"Erosive Forces", 124,
		{
			{ 1, "Strong" },
			{ 2, "Average" },
			{ 3, "Weak" },
		}, 110
	);

	SetRow(
		RI_PLANET_LIFEFORMS,
		"Native Lifeforms", 124,
		{
			{ 1, "Rare" },
			{ 2, "Average" },
			{ 3, "Abundant" },
		}, 110
	);

	SetRow(
		RI_PLANET_CLOUDS,
		"Cloud Cover", 124,
		{
			{ 1, "Sparse" },
			{ 2, "Average" },
			{ 3, "Dense" },
		}, 110
	);

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

void GameSettingsSection::SetRow( const row_id_t row_id, const std::string& label, const size_t label_width, const ::ui::object::ChoiceList::choices_t& choices, const size_t choices_width ) {
	NEWV( label_el, ::ui::object::Label, "PopupLabel" )
	label_el->SetText( label );
	label_el->SetAlign( UIObject::ALIGN_LEFT );
	label_el->SetLeft( 6 );
	label_el->SetWidth( label_width );
	m_body->AddChild( label_el );
	NEWV( choices_el, ::ui::object::Dropdown, "PopupDropdown" );
	choices_el->SetChoices( choices );
	choices_el->SetAlign( UIObject::ALIGN_LEFT );
	choices_el->SetLeft( label_width );
	choices_el->SetWidth( choices_width );
	choices_el->On( UIEvent::EV_CHANGE, EH( this ) {
		const auto &value = *( data->value.change.text );
		Log( "VALUE: " + value );
		return true;
	});
	m_body->AddChild( choices_el );
	m_element_rows[ row_id ] = { label_el, choices_el };
}

}
}
}
