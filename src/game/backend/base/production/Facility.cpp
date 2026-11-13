#include "Facility.h"

#include "game/backend/base/FacilityDef.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/base/BaseManager.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Facility::Facility( const std::string& id, const FacilityDef* const def, const std::string& hint, const std::string& intent )
	: Production( id, PT_FACILITY, hint, intent )
	, m_def( def ) {}

gse::value::Object* const Facility::ToValue( GSE_CALLABLE ) const {
	return VALUEEXT( gse::value::Object, GSE_CALL, {
		{ "id", VALUE( gse::value::String, , m_id ) },
		{ "name", VALUE( gse::value::String, , m_def->m_name ) },
		{ "texture", VALUE( gse::value::String, , m_def->m_render_info.ToTextureString() ) },
		{
			"hint", VALUE(
				gse::value::String, , !m_hint.empty()
					? m_hint
					: m_def->m_description
			)
		},
		{
			"intent", VALUE(
				gse::value::String, , !m_intent.empty()
					? m_intent
					: m_def->m_intent
			)
		},
		{ "type", VALUE( gse::value::String, , "facility" ) },
		{ "cost", VALUE( gse::value::Int, , m_def->m_cost ) },
		{ "facility_id", VALUE( gse::value::String, , m_def->m_id ) },
	} );
}

const uint8_t Facility::GetGroupOrder() const {
	return 5; // TODO: get from scripts
}

void Facility::SerializeImpl( types::Buffer& buf ) const {
	buf.WriteString( m_def->m_id );
}

const Production* const Facility::DeserializeImpl( types::Buffer& buf, const std::string& id, const std::string& hint, const std::string& intent ) {
	const auto facility_id = buf.ReadString();
	const auto* def = g_engine->GetGame()->GetBM()->GetFacilityDef( facility_id );
	if ( !def ) {
		THROW( "failed to deserialize production choice - facility id not found: " + facility_id );
	}
	return new Facility( id, def, hint, intent );
}

}
}
}
}
