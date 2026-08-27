#include "Def.h"

#include "StaticDef.h"

#include "MoraleSet.h"

#include "gse/value/Object.h"
#include "gse/value/Bool.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"

namespace game {
namespace backend {
namespace unit {

Def::Def(
	const std::string& id,
	const MoraleSet* moraleset,
	const def_type_t type,
	const std::string& name,
	const int64_t mineral_cost,
	const std::string& required_technology,
	const bool is_native,
	const int64_t offense,
	const int64_t defense,
	const bool can_found_base,
	const bool can_terraform,
	const bool buildable
)
	: m_id( id )
	, m_moraleset( moraleset )
	, m_type( type )
	, m_name( name )
	, m_mineral_cost( mineral_cost )
	, m_required_technology( required_technology )
	, m_is_native( is_native )
	, m_offense( offense )
	, m_defense( defense )
	, m_can_found_base( can_found_base )
	, m_can_terraform( can_terraform )
	, m_buildable( buildable ) {
	if (
		m_id.empty() ||
		m_name.empty() ||
		!m_moraleset ||
		m_mineral_cost < 0 ||
		m_mineral_cost > MAX_MINERAL_COST ||
		m_offense < 0 ||
		m_offense > MAX_COMBAT_STRENGTH ||
		m_defense <= 0 ||
		m_defense > MAX_COMBAT_STRENGTH ||
		( m_can_found_base && m_can_terraform )
	) {
		THROW( "invalid unit definition: " + m_id );
	}
}

const types::Buffer Def::Serialize( const Def* def ) {
	types::Buffer buf;
	buf.WriteString( def->m_id );
	buf.WriteString( def->m_moraleset->m_id );
	buf.WriteString( def->m_name );
	buf.WriteInt( def->m_mineral_cost );
	buf.WriteString( def->m_required_technology );
	buf.WriteBool( def->m_is_native );
	buf.WriteInt( def->m_offense );
	buf.WriteInt( def->m_defense );
	buf.WriteBool( def->m_can_found_base );
	buf.WriteBool( def->m_can_terraform );
	buf.WriteInt( def->m_type );
	switch ( def->m_type ) {
		case DT_STATIC: {
			StaticDef::Serialize( buf, (StaticDef*)def );
			break;
		}
		default:
			THROW( "unknown def type on write: " + std::to_string( def->m_type ) );
	}
	return buf;
}

Def* Def::Deserialize( types::Buffer& buf ) {
	const auto id = buf.ReadString();
	const auto moraleset = buf.ReadString();
	const auto name = buf.ReadString();
	const auto mineral_cost = buf.ReadInt();
	const auto required_technology = buf.ReadString();
	const auto is_native = buf.ReadBool();
	const auto offense = buf.ReadInt();
	const auto defense = buf.ReadInt();
	const auto can_found_base = buf.ReadBool();
	const auto can_terraform = buf.ReadBool();
	const auto serialized_type = buf.ReadInt();
	if ( id.empty() || moraleset.empty() || name.empty() ) {
		THROW( "serialized unit definition id, name, or morale set is empty" );
	}
	if ( serialized_type != DT_STATIC ) {
		THROW( "unknown def type on read: " + std::to_string( serialized_type ) );
	}
	if ( mineral_cost < 0 || mineral_cost > MAX_MINERAL_COST ) {
		THROW( "invalid serialized unit mineral cost" );
	}
	if (
		offense < 0 ||
		offense > MAX_COMBAT_STRENGTH ||
		defense <= 0 ||
		defense > MAX_COMBAT_STRENGTH ||
		( can_found_base && can_terraform )
	) {
		THROW( "invalid serialized unit combat or capability values" );
	}
	const auto type = static_cast< def_type_t >( serialized_type );
	switch ( type ) {
		case DT_STATIC:
			return StaticDef::Deserialize(
				buf,
				id,
				moraleset,
				name,
				mineral_cost,
				required_technology,
				is_native,
				offense,
				defense,
				can_found_base,
				can_terraform
			);
		default:
			THROW( "unknown def type on read: " + std::to_string( type ) );
	}
}

WRAPIMPL_BEGIN( Def )
	WRAPIMPL_PROPS
			{
				"id",
				VALUE( gse::value::String, , m_id )
			},
			{
				"name",
				VALUE( gse::value::String, , m_name )
			},
			{
				"production_kind",
				VALUE( gse::value::String, , "unit" )
			},
			{
				"type",
				VALUE( gse::value::String, , "static" ) // TODO
			},
			{
				"mineral_cost",
				VALUE( gse::value::Int, , m_mineral_cost )
			},
			{
				"required_technology",
				VALUE( gse::value::String, , m_required_technology )
			},
			{
				"morale_set",
				VALUE( gse::value::String, , m_moraleset->m_id )
			},
			{
				"is_native",
				VALUE( gse::value::Bool, , m_is_native )
			},
			{
				"offense",
				VALUE( gse::value::Int, , m_offense )
			},
			{
				"defense",
				VALUE( gse::value::Int, , m_defense )
			},
			{
				"can_found_base",
				VALUE( gse::value::Bool, , m_can_found_base )
			},
			{
				"can_terraform",
				VALUE( gse::value::Bool, , m_can_terraform )
			},
			{
				"buildable",
				VALUE( gse::value::Bool, , m_buildable )
			},
		};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( Def )

}
}
}
