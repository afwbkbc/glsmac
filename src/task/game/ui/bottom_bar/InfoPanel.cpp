#include "InfoPanel.h"

#include "engine/Engine.h"
#include "../../Game.h"

namespace task {
namespace game {
namespace ui {

InfoPanel::InfoPanel( Game* game )
	: BBSection( game, "MiddleAreaInfoPanel" ) {
	m_config.no_outer_border = true;
}

void InfoPanel::Create() {
	BBSection::Create();

	Restart();
}

void InfoPanel::Iterate() {
	BBSection::Iterate();

	while ( m_timer.HasTicked() ) {
		ASSERT( m_line_index <= m_lines.size(), "line index lines overflow" );
		if ( m_line_index == m_lines.size() ) {
			Restart();
		}
		else {
			const auto& line = m_lines.at( m_line_index );
			ASSERT( m_line_position <= line.size(), "line position line overflow" );
			if ( m_line_position == line.size() ) {
				m_line_position = 0;
				m_line_index++;
				if ( m_line_index == m_lines.size() ) {
					m_timer.SetTimeout( s_wait_after_print );
				}
			}
			else {
				while ( m_line_index >= m_labels.size() ) {
					NEWV( label, ::ui::object::Label, "BBMiddleAreaInfoPanelLabel" );
					label->SetLeft( s_padding );
					label->SetTop( s_padding + m_line_index * s_label_height );
					AddChild( label );
					m_labels.push_back( label );
				}
				auto& label = m_labels.at( m_line_index );
				label->SetText( label->GetText() + line[ m_line_position ] );
				m_line_position++;
			}
		}
	}
}

void InfoPanel::Destroy() {
	for ( auto& label : m_labels ) {
		RemoveChild( label );
	}

	BBSection::Destroy();
}

void InfoPanel::Restart() {
	m_timer.Stop();
	m_lines.clear();
	m_line_index = m_line_position = 0;
	for ( auto& label : m_labels ) {
		RemoveChild( label );
	}
	m_labels.clear();

	m_info_type = (info_type_t)m_game->GetRandom()->GetUInt( IT_NONE + 1, IT_MAX - 1 );

	// TODO: multicolor labels

	switch ( m_info_type ) {
		case IT_MISSION_STATS: {
			m_lines = {
				"Mission Year: 2101",
				"",
				"Econ: 50%",
				"Psych: 0%",
				"Labs: 50%"
			};
			break;
		}
		default: {
			THROW( "unknown info type " + std::to_string( m_info_type ) );
		}
	}

	m_timer.SetInterval( s_character_frequency_ms );
}

}
}
}
