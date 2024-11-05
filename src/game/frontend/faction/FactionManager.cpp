#include "FactionManager.h"

#include "game/frontend/Game.h"
#include "game/backend/Faction.h"
#include "Faction.h"

namespace game {
namespace frontend {
namespace faction {

FactionManager::FactionManager( Game* game )
	: m_game( game ) {

}

void FactionManager::DefineFaction( const backend::Faction* def ) {
	ASSERT( def, "faction is null" );
	ASSERT( m_factions.find( def->m_id ) == m_factions.end(), "faction already defined" );
	NEWV( faction, Faction, def, m_game->GetISM() );
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
