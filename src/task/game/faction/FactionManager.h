#pragma once

#include <unordered_map>

#include "base/Base.h"

namespace game::rules {
class Faction;
}

namespace task {
namespace game {

class Game;

namespace faction {

class Faction;

CLASS( FactionManager, ::base::Base )

	FactionManager( Game* game );

	void DefineFaction( const ::game::rules::Faction* def );

	Faction* GetFactionById( const std::string& id ) const;

private:

	Game* m_game = nullptr;

	std::unordered_map< std::string, Faction* > m_factions = {};

};

}
}
}
