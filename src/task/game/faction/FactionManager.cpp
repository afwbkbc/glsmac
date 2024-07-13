#include "FactionManager.h"

#include "task/game/Game.h"
#include "game/backend/rules/Faction.h"
#include "Faction.h"

namespace task {
namespace game {
namespace faction {

FactionManager::FactionManager( Game* game )
	: m_game( game ) {

}

void FactionManager::DefineFaction( const ::game::backend::rules::Faction* def ) {
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
