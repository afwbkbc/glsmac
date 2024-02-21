#include "Rules.h"

namespace game {
namespace rules {

void Rules::Initialize() {
	if ( !m_is_initialized ) {
		InitRules();
		m_is_initialized = true;
	}
}

const types::Buffer Rules::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_factions_order.size() );
	for ( auto& id : m_factions_order ) {
		const auto& faction = m_factions.at( id );
		buf.WriteString( id );
		buf.WriteString( faction.Serialize().ToString() );
	}

	buf.WriteInt( m_difficulty_levels.size() );
	for ( auto& difficulty_level : m_difficulty_levels ) {
		buf.WriteInt( difficulty_level.first );
		buf.WriteString( difficulty_level.second.Serialize().ToString() );
	}

	return buf;
}

void Rules::Unserialize( types::Buffer buf ) {

	m_factions.clear();
	m_factions_order.clear();
	const size_t factions_count = buf.ReadInt();
	for ( size_t i = 0 ; i < factions_count ; i++ ) {
		const auto faction_id = buf.ReadString();
		m_factions[ faction_id ].Unserialize( buf.ReadString() );
		m_factions_order.push_back( faction_id );
	}

	m_difficulty_levels.clear();
	const size_t difficulty_levels_count = buf.ReadInt();
	for ( size_t i = 0 ; i < difficulty_levels_count ; i++ ) {
		const size_t difficulty_level_id = buf.ReadInt();
		m_difficulty_levels[ difficulty_level_id ].Unserialize( buf.ReadString() );
	}

	m_is_initialized = true;

}

}
}
