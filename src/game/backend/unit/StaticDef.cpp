#include "StaticDef.h"

#include "Render.h"

#include "gse/type/Bool.h"
#include "gse/type/Int.h"
#include "gse/type/Float.h"

namespace game {
namespace backend {
namespace unit {

// TODO: per-def values?
const health_t StaticDef::HEALTH_MAX = 1.0f;
const health_t StaticDef::HEALTH_PER_TURN = 0.1f;

const std::unordered_map< movement_type_t, std::string > StaticDef::s_movement_type_str = {
	{
		MT_IMMOVABLE,
		"IMMOVABLE"
	},
	{
		MT_LAND,
		"LAND"
	},
	{
		MT_WATER,
		"SEA"
	},
	{
		MT_AIR,
		"AIR"
	}
};
const std::string& StaticDef::GetMovementTypeString( const movement_type_t movement_type ) {
	const auto& it = s_movement_type_str.find( movement_type );
	ASSERT_NOLOG( it != s_movement_type_str.end(), "unknown movement type: " + std::to_string( movement_type ) );
	return it->second;
}

StaticDef::StaticDef(
	const std::string& id,
	const MoraleSet* moraleset,
	const std::string& name,
	const movement_type_t movement_type,
	const movement_t movement_per_turn,
	const Render* render
)
	: Def( id, moraleset, DT_STATIC, name )
	, m_movement_type( movement_type )
	, m_movement_per_turn( movement_per_turn )
	, m_render( render ) {}

StaticDef::~StaticDef() {
	delete m_render;
}

const movement_type_t StaticDef::GetMovementType() const {
	return m_movement_type;
}

const std::string StaticDef::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "StaticDef" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_PROP_STR( "movement_type", GetMovementTypeString( m_movement_type ) ) +
		TS_OBJ_PROP_NUM( "movement_per_turn", m_movement_per_turn ) +
		TS_OBJ_PROP( "render", m_render->ToString( TS_PREFIX_NEXT ) ) +
		TS_OBJ_END();
}

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	buf.WriteInt( def->m_movement_type );
	buf.WriteFloat( def->m_movement_per_turn );
	Render::Serialize( buf, def->m_render );
}

StaticDef* StaticDef::Unserialize( types::Buffer& buf, const std::string& id, const MoraleSet* moraleset, const std::string& name ) {
	const auto movement_type = (movement_type_t)buf.ReadInt();
	const auto movement_per_turn = buf.ReadFloat();
	return new StaticDef( id, moraleset, name, movement_type, movement_per_turn, Render::Unserialize( buf ) );
}

WRAPIMPL_BEGIN( StaticDef, CLASS_UNITDEF )
	WRAPIMPL_PROPS
		WRAPIMPL_GET( "is_immovable", Bool, m_movement_type == MT_IMMOVABLE )
		WRAPIMPL_GET( "is_land", Bool, m_movement_type == MT_LAND )
		WRAPIMPL_GET( "is_water", Bool, m_movement_type == MT_WATER )
		WRAPIMPL_GET( "is_air", Bool, m_movement_type == MT_AIR )
		WRAPIMPL_GET( "movement_per_turn", Float, m_movement_per_turn )
		WRAPIMPL_GET( "health_per_turn", Float, HEALTH_PER_TURN )
		WRAPIMPL_GET( "health_max", Float, HEALTH_MAX )
	};
	WRAPIMPL_PROPS_EXTEND( Def )
WRAPIMPL_END_PTR( StaticDef )

UNWRAPIMPL_PTR( StaticDef )

}
}
}
