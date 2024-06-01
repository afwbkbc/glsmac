#include "Faction.h"

#include "game/rules/Faction.h"

namespace task {
namespace game {
namespace faction {

Faction::Faction( const ::game::rules::Faction* def )
	: m_border_color( def->m_colors.border )
	, m_is_progenitor( def->m_flags & ::game::rules::Faction::FF_PROGENITOR ) {
	//
}

}
}
}
