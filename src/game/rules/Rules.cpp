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
	
	buf.WriteInt( m_factions.size() );
	for ( auto& faction : m_factions ) {
		buf.WriteInt( faction.first );
		buf.WriteString( faction.second.Serialize().ToString() );
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
	const size_t factions_count = buf.ReadInt();
	for ( size_t i = 0 ; i < factions_count ; i++ ) {
		const size_t faction_id = buf.ReadInt();
		m_factions[ faction_id ].Unserialize( buf.ReadString() );
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
