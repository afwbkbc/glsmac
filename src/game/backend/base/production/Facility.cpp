#include "Facility.h"

#include "game/backend/base/FacilityDef.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/base/BaseManager.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Facility::Facility( const FacilityDef* const def )
	: Production( PT_FACILITY )
	, m_def( def ) {}

void Facility::SerializeImpl( types::Buffer& buf ) const {
	buf.WriteString( m_def->m_id );
}

const Production* const Facility::DeserializeImpl( types::Buffer& buf ) {
	const auto facility_id = buf.ReadString();
	const auto* def = g_engine->GetGame()->GetBM()->GetFacilityDef( facility_id );
	if ( !def ) {
		THROW( "failed to deserialize production choice - facility id not found: " + facility_id );
	}
	return new Facility( def );
}

}
}
}
}
