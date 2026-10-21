#include "Facility.h"

#include "game/backend/base/FacilityDef.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/base/BaseManager.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Facility::Facility( const FacilityDef* const def, const std::string& hint )
	: Production( PT_FACILITY, hint )
	, m_def( def ) {}

gse::value::Object* const Facility::ToValue( GSE_CALLABLE, const std::string& id ) const {
	return VALUEEXT( gse::value::Object, GSE_CALL, {
		{ "id", VALUE( gse::value::String, , id ) },
		{ "name", VALUE( gse::value::String, , m_def->m_name ) },
		{ "texture", VALUE( gse::value::String, , m_def->m_render_info.ToTextureString() ) },
		{
			"hint", VALUE(
				gse::value::String, , !m_hint.empty()
					? m_hint
					: m_def->m_description
			)
		},
	} );
}

const uint8_t Facility::GetGroupOrder() const {
	return 5; // TODO: get from scripts
}

void Facility::SerializeImpl( types::Buffer& buf ) const {
	buf.WriteString( m_def->m_id );
}

const Production* const Facility::DeserializeImpl( types::Buffer& buf, const std::string& hint ) {
	const auto facility_id = buf.ReadString();
	const auto* def = g_engine->GetGame()->GetBM()->GetFacilityDef( facility_id );
	if ( !def ) {
		THROW( "failed to deserialize production choice - facility id not found: " + facility_id );
	}
	return new Facility( def, hint );
}

}
}
}
}
