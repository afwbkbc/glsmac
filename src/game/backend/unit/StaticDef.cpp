#include "StaticDef.h"

#include <cmath>
#include <memory>

#include "Render.h"

#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/Float.h"
#include "gse/value/String.h"
#include "gse/value/Array.h"

#include "engine/Engine.h"
#include "game/backend/Game.h"
#include "UnitManager.h"

namespace game {
namespace backend {
namespace unit {

static constexpr int64_t COMPONENT_METADATA_VERSION = 4;

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
	ASSERT( it != s_movement_type_str.end(), "unknown movement type: " + std::to_string( movement_type ) );
	return it->second;
}

StaticDef::StaticDef(
	const std::string& id,
	const MoraleSet* moraleset,
	const std::string& name,
	const int64_t mineral_cost,
	const std::string& required_technology,
	const bool is_native,
	const int64_t offense,
	const int64_t defense,
	const bool can_found_base,
	const bool can_terraform,
	const movement_type_t movement_type,
	const movement_t movement_per_turn,
	const Render* render,
	const std::string& chassis_id,
	const std::string& weapon_id,
	const std::string& armor_id,
	const std::string& reactor_id,
	const int64_t reactor_power,
	const std::set< std::string >& abilities,
	const int64_t operational_range,
	const bool is_missile,
	const int64_t cargo_capacity,
	const bool buildable
)
	: Def(
		id,
		moraleset,
		DT_STATIC,
		name,
		mineral_cost,
		required_technology,
		is_native,
		offense,
		defense,
		can_found_base,
		can_terraform,
		buildable
	)
	, m_movement_type( movement_type )
	, m_movement_per_turn( movement_per_turn )
	, m_render( render )
	, m_chassis_id( chassis_id )
	, m_weapon_id( weapon_id )
	, m_armor_id( armor_id )
	, m_reactor_id( reactor_id )
	, m_reactor_power( reactor_power )
	, m_abilities( abilities )
	, m_operational_range( operational_range )
	, m_is_missile( is_missile )
	, m_cargo_capacity( cargo_capacity ) {
	if (
		m_movement_type < MT_IMMOVABLE ||
		m_movement_type > MT_AIR ||
		!std::isfinite( m_movement_per_turn ) ||
		m_movement_per_turn < 0.0f ||
		!m_render ||
		m_reactor_power < 1 ||
		m_reactor_power > 4 ||
		m_abilities.size() > MAX_ABILITIES ||
		m_operational_range < 0 ||
		m_operational_range > MAX_OPERATIONAL_RANGE ||
		m_cargo_capacity < 0 ||
		m_cargo_capacity > MAX_CARGO_CAPACITY ||
		( m_cargo_capacity > 0 && ( m_movement_type == MT_IMMOVABLE || m_is_missile ) ) ||
		( m_movement_type != MT_AIR && ( m_operational_range > 0 || m_is_missile ) ) ||
		( m_is_missile && m_operational_range == 0 ) ||
		(
			m_can_found_base &&
			m_movement_type != MT_LAND && m_movement_type != MT_WATER
		) ||
		( m_can_terraform && m_movement_type != MT_LAND )
	) {
		THROW( "invalid static unit definition: " + m_id );
	}
	for ( const auto& ability : m_abilities ) {
		if ( ability.empty() ) {
			THROW( "invalid empty unit ability: " + m_id );
		}
	}
}

StaticDef::~StaticDef() {
	delete m_render;
}

const movement_type_t StaticDef::GetMovementType() const {
	return m_movement_type;
}

const bool StaticDef::HasAbility( const std::string& id ) const {
	return m_abilities.find( id ) != m_abilities.end();
}

const bool StaticDef::IsArtillery() const {
	return HasAbility( "HeavyArtillery" ) || m_id == "SporeLauncher";
}

const bool StaticDef::IsPsiAttack() const {
	return m_is_native || m_weapon_id == "PsiAttack";
}

const bool StaticDef::IsPsiDefense() const {
	return m_is_native || m_armor_id == "PsiDefense";
}

const std::string StaticDef::ToString( const std::string& prefix ) const {
	return (std::string)
		TS_OBJ_BEGIN( "StaticDef" ) +
		TS_OBJ_PROP_STR( "id", m_id ) +
		TS_OBJ_PROP_STR( "name", m_name ) +
		TS_OBJ_PROP_NUM( "mineral_cost", m_mineral_cost ) +
		TS_OBJ_PROP_STR( "required_technology", m_required_technology ) +
		TS_OBJ_PROP_STR( "chassis", m_chassis_id ) +
		TS_OBJ_PROP_STR( "weapon", m_weapon_id ) +
		TS_OBJ_PROP_STR( "armor", m_armor_id ) +
		TS_OBJ_PROP_STR( "reactor", m_reactor_id ) +
		TS_OBJ_PROP_NUM( "reactor_power", m_reactor_power ) +
		TS_OBJ_PROP_STR( "movement_type", GetMovementTypeString( m_movement_type ) ) +
		TS_OBJ_PROP_NUM( "movement_per_turn", m_movement_per_turn ) +
		TS_OBJ_PROP_NUM( "operational_range", m_operational_range ) +
		TS_OBJ_PROP_NUM( "cargo_capacity", m_cargo_capacity ) +
		TS_OBJ_PROP( "render", m_render->ToString( TS_PREFIX_NEXT ) ) +
		TS_OBJ_END();
}

void StaticDef::Serialize( types::Buffer& buf, const StaticDef* def ) {
	buf.WriteInt( def->m_movement_type );
	buf.WriteFloat( def->m_movement_per_turn );
	Render::Serialize( buf, def->m_render );
	buf.WriteInt( COMPONENT_METADATA_VERSION );
	buf.WriteString( def->m_chassis_id );
	buf.WriteString( def->m_weapon_id );
	buf.WriteString( def->m_armor_id );
	buf.WriteString( def->m_reactor_id );
	buf.WriteInt( def->m_reactor_power );
	buf.WriteInt( def->m_abilities.size() );
	for ( const auto& ability : def->m_abilities ) {
		buf.WriteString( ability );
	}
	buf.WriteInt( def->m_operational_range );
	buf.WriteBool( def->m_is_missile );
	buf.WriteInt( def->m_cargo_capacity );
	buf.WriteBool( def->m_buildable );
}

StaticDef* StaticDef::Deserialize(
	types::Buffer& buf,
	const std::string& id,
	const std::string& moraleset_name,
	const std::string& name,
	const int64_t mineral_cost,
	const std::string& required_technology,
	const bool is_native,
	const int64_t offense,
	const int64_t defense,
	const bool can_found_base,
	const bool can_terraform
) {
	const auto serialized_movement_type = buf.ReadInt();
	const auto movement_per_turn = buf.ReadFloat();
	if ( serialized_movement_type < MT_IMMOVABLE || serialized_movement_type > MT_AIR ) {
		THROW( "invalid serialized unit movement type" );
	}
	if ( !std::isfinite( movement_per_turn ) || movement_per_turn < 0.0f ) {
		THROW( "invalid serialized unit movement per turn" );
	}
	if (
		( can_found_base && serialized_movement_type != MT_LAND && serialized_movement_type != MT_WATER ) ||
		( can_terraform && serialized_movement_type != MT_LAND )
	) {
		THROW( "invalid serialized movement type for founding or terraforming unit" );
	}
	const auto* moraleset = g_engine->GetGame()->GetUM()->GetMoraleSet( moraleset_name );
	if ( !moraleset ) {
		THROW( "could not find morale set: " + moraleset_name );
	}
	auto render = std::unique_ptr< Render >( Render::Deserialize( buf ) );
	std::string chassis_id = "";
	std::string weapon_id = "";
	std::string armor_id = "";
	std::string reactor_id = "";
	int64_t reactor_power = 1;
	std::set< std::string > abilities = {};
	int64_t operational_range = 0;
	bool is_missile = false;
	int64_t cargo_capacity = 0;
	bool buildable = true;
	if ( buf.GetRemaining() > 0 ) {
		const auto version = buf.ReadInt();
		if ( version < 1 || version > COMPONENT_METADATA_VERSION ) {
			THROW( "unsupported serialized unit component metadata version" );
		}
		chassis_id = buf.ReadString();
		weapon_id = buf.ReadString();
		armor_id = buf.ReadString();
		reactor_id = buf.ReadString();
		reactor_power = buf.ReadInt();
		if ( reactor_power < 1 || reactor_power > 4 ) {
			THROW( "invalid serialized unit reactor power" );
		}
		const auto ability_count = buf.ReadCollectionSize( "unit ability" );
		if ( ability_count > MAX_ABILITIES ) {
			THROW( "too many serialized unit abilities" );
		}
		for ( size_t i = 0 ; i < ability_count ; i++ ) {
			const auto ability = buf.ReadString();
			if ( ability.empty() || !abilities.insert( ability ).second ) {
				THROW( "invalid or duplicate serialized unit ability" );
			}
		}
		if ( version >= 2 ) {
			operational_range = buf.ReadInt();
			is_missile = buf.ReadBool();
			if (
				operational_range < 0 ||
				operational_range > MAX_OPERATIONAL_RANGE ||
				( serialized_movement_type != MT_AIR && ( operational_range > 0 || is_missile ) ) ||
				( is_missile && operational_range == 0 )
			) {
				THROW( "invalid serialized unit operational range" );
			}
		}
		if ( version >= 3 ) {
			cargo_capacity = buf.ReadInt();
			if (
				cargo_capacity < 0 ||
				cargo_capacity > MAX_CARGO_CAPACITY ||
				(
					cargo_capacity > 0 &&
					(serialized_movement_type == MT_IMMOVABLE || is_missile)
				)
			) {
				THROW( "invalid serialized unit cargo capacity" );
			}
		}
		if ( version >= 4 ) {
			buildable = buf.ReadBool();
		}
	}
	return new StaticDef(
		id,
		moraleset,
		name,
		mineral_cost,
		required_technology,
		is_native,
		offense,
		defense,
		can_found_base,
		can_terraform,
		static_cast< movement_type_t >( serialized_movement_type ),
		movement_per_turn,
		render.release(),
		chassis_id,
		weapon_id,
		armor_id,
		reactor_id,
		reactor_power,
		abilities,
		operational_range,
		is_missile,
		cargo_capacity,
		buildable
	);
}

WRAPIMPL_BEGIN( StaticDef )
	gse::value::array_elements_t abilities = {};
	abilities.reserve( m_abilities.size() );
	for ( const auto& ability : m_abilities ) {
		abilities.push_back( VALUE( gse::value::String, , ability ) );
	}
	WRAPIMPL_PROPS
			WRAPIMPL_GET_CUSTOM( "is_immovable", Bool, m_movement_type == MT_IMMOVABLE )
			WRAPIMPL_GET_CUSTOM( "is_land", Bool, m_movement_type == MT_LAND )
			WRAPIMPL_GET_CUSTOM( "is_water", Bool, m_movement_type == MT_WATER )
			WRAPIMPL_GET_CUSTOM( "is_air", Bool, m_movement_type == MT_AIR )
			WRAPIMPL_GET_CUSTOM( "movement_per_turn", Float, m_movement_per_turn )
			WRAPIMPL_GET_CUSTOM( "health_per_turn", Float, HEALTH_PER_TURN )
			WRAPIMPL_GET_CUSTOM( "health_max", Float, HEALTH_MAX )
			WRAPIMPL_GET_CUSTOM( "chassis", String, m_chassis_id )
			WRAPIMPL_GET_CUSTOM( "weapon", String, m_weapon_id )
			WRAPIMPL_GET_CUSTOM( "armor", String, m_armor_id )
			WRAPIMPL_GET_CUSTOM( "reactor", String, m_reactor_id )
			WRAPIMPL_GET_CUSTOM( "reactor_power", Int, m_reactor_power )
			WRAPIMPL_GET_CUSTOM( "abilities", Array, abilities )
			WRAPIMPL_GET_CUSTOM( "operational_range", Int, m_operational_range )
			WRAPIMPL_GET_CUSTOM( "is_missile", Bool, m_is_missile )
			WRAPIMPL_GET_CUSTOM( "cargo_capacity", Int, m_cargo_capacity )
			WRAPIMPL_GET_CUSTOM( "is_artillery", Bool, IsArtillery() )
			WRAPIMPL_GET_CUSTOM( "is_psi_attack", Bool, IsPsiAttack() )
			WRAPIMPL_GET_CUSTOM( "is_psi_defense", Bool, IsPsiDefense() )
		};
	WRAPIMPL_PROPS_EXTEND( Def )
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( StaticDef )

}
}
}
