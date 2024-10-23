#include "Pop.h"

#include "PopDef.h"
#include "Base.h"
#include "game/frontend/faction/Faction.h"

namespace game {
namespace frontend {
namespace base {

Pop::Pop( const Base* base, const PopDef* def, const uint8_t variant )
	: m_base( base )
	, m_def( def )
	, m_variant( variant ) {
	//
}

types::texture::Texture* Pop::GetTexture() const {
	return m_def->GetTexture( m_variant, m_base->GetFaction()->m_is_progenitor );
}

}
}
}