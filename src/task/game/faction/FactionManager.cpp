#include "FactionManager.h"

#include "game/rules/Faction.h"
#include "Faction.h"

namespace task {
namespace game {
namespace faction {

void FactionManager::DefineFaction( const ::game::rules::Faction* def ) {
	ASSERT( def, "faction is null" );
	ASSERT( m_factions.find( def->m_id ) == m_factions.end(), "faction already defined" );
	NEWV( faction, Faction, def );
	m_factions.insert(
		{
			def->m_id,
			faction
		}
	);
}

Faction* FactionManager::GetFactionById( const std::string& id ) const {
	ASSERT( m_factions.find( id ) != m_factions.end(), "faction '" + id + "' is not defined" );
	return m_factions.at( id );
}

}
}
}
