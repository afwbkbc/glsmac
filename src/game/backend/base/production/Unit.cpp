#include "Unit.h"

#include "game/backend/unit/Def.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "game/backend/unit/UnitManager.h"

namespace game {
namespace backend {
namespace base {
namespace production {

Unit::Unit( const std::string& id, const unit::Def* const def )
	: Production( id, PT_UNIT, "?-?-?", "conquer" ) // TODO
	, m_def( def ) {}

gse::value::Object* const Unit::ToValue( GSE_CALLABLE ) const {
	return VALUEEXT( gse::value::Object, GSE_CALL, {
		{ "id", VALUE( gse::value::String, , m_id ) },
		{ "name", VALUE( gse::value::String, , m_def->m_name ) },
		{ "texture", VALUE( gse::value::String, , m_def->GetRenderTextureString() ) },
		{ "hint", VALUE( gse::value::String, , m_hint ) },
		{ "intent", VALUE( gse::value::String, , m_intent ) },
		{ "type", VALUE( gse::value::String, , "unit" ) },
		{ "cost", VALUE( gse::value::Int, , m_def->m_cost ) },
		{ "unit_id", VALUE( gse::value::String, , m_def->m_id ) },
	} );
}

const uint8_t Unit::GetGroupOrder() const {
	return 3; // TODO: get from scripts
}

void Unit::SerializeImpl( types::Buffer& buf ) const {
	buf.WriteString( m_def->m_id );
}

const Production* const Unit::DeserializeImpl( types::Buffer& buf, const std::string& id, const std::string& hint, const std::string& intent ) {
	const auto unit_id = buf.ReadString();
	const auto* def = g_engine->GetGame()->GetUM()->GetUnitDef( unit_id );
	if ( !def ) {
		THROW( "failed to deserialize production choice - unit id not found: " + unit_id );
	}
	return new Unit( id, def );
}

}
}
}
}
