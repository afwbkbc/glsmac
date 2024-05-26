#pragma once

#include <unordered_map>

#include "base/Base.h"

namespace game::rules {
class Faction;
}

namespace task {
namespace game {
namespace faction {

class Faction;

CLASS( FactionManager, base::Base )

	void DefineFaction( const ::game::rules::Faction* def );

	Faction* GetFactionById( const std::string& id ) const;

private:
	std::unordered_map< std::string, Faction* > m_factions = {};

};

}
}
}
