#include "UnitManager.h"

#include <algorithm>
#include <memory>
#include <unordered_set>

#include "MoraleSet.h"
#include "Def.h"
#include "StaticDef.h"
#include "Unit.h"

#include "game/backend/Game.h"
#include "game/backend/animation/AnimationManager.h"
#include "game/backend/State.h"
#include "game/backend/Bindings.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/unit/SpriteRender.h"
#include "game/backend/Player.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Bool.h"
#include "gse/value/Float.h"
#include "gse/value/Array.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileManager.h"
#include "game/backend/base/Base.h"
#include "game/backend/base/BaseManager.h"
#include "util/String.h"

namespace game {
namespace backend {
namespace unit {

UnitManager::UnitManager( Game* game )
	: gse::GCWrappable( game->GetGCSpace() )
	, m_game( game ) {
	//
}

UnitManager::~UnitManager() {
	Clear();
}

void UnitManager::Clear() {
	for ( auto& it : m_unprocessed_units ) {
		delete it;
	}
	m_unprocessed_units.clear();

	for ( auto& it : m_units ) {
		delete it.second;
	}
	m_units.clear();

	for ( auto& it : m_unit_moralesets ) {
		delete it.second;
	}
	m_unit_moralesets.clear();

	for ( auto& it : m_unit_defs ) {
		delete it.second;
	}
	m_unit_defs.clear();

	m_unit_updates.clear();
}

void UnitManager::DefineMoraleSet( MoraleSet* moraleset ) {
	Log( "Defining unit moraleset ('" + moraleset->m_id + "')" );

	ASSERT( m_unit_moralesets.find( moraleset->m_id ) == m_unit_moralesets.end(), "Unit moraleset '" + moraleset->m_id + "' already exists" );

	m_unit_moralesets.insert(
		{
			moraleset->m_id,
			moraleset
		}
	);
}

void UnitManager::UndefineMoraleSet( const std::string& id ) {
	Log( "Undefining unit moraleset ('" + id + "')" );

	ASSERT( m_unit_moralesets.find( id ) != m_unit_moralesets.end(), "Unit moraleset '" + id + "' not found" );

	m_unit_moralesets.erase( id );
}

void UnitManager::DefineUnit( Def* def ) {
	Log( "Defining unit ('" + def->m_id + "')" );

	ASSERT( m_unit_defs.find( def->m_id ) == m_unit_defs.end(), "Unit definition '" + def->m_id + "' already exists" );

	m_unit_defs.insert(
		{
			def->m_id,
			def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DEFINE );
	NEW( fr.data.unit_define.serialized_unitdef, std::string, Def::Serialize( def ).ToString() );
	m_game->AddFrontendRequest( fr );
}

void UnitManager::UndefineUnit( const std::string& id ) {
	Log( "Undefining unit ('" + id + "')" );

	ASSERT( m_unit_defs.find( id ) != m_unit_defs.end(), "Unit definition '" + id + "' not found" );

	m_unit_defs.erase( id );

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_UNDEFINE );
	NEW( fr.data.unit_undefine.id, std::string, id );
	m_game->AddFrontendRequest( fr );
}

void UnitManager::SpawnUnit( GSE_CALLABLE, Unit* unit ) {
	unit->m_is_registered = true;
	if ( !m_game->IsRunning() ) {
		m_unprocessed_units.push_back( unit );
		return;
	}

	auto* tile = unit->GetTile();

	Log( "Spawning unit #" + std::to_string( unit->m_id ) + " ( " + unit->m_def->m_id + " ) at " + tile->ToString() );

	ASSERT( m_units.find( unit->m_id ) == m_units.end(), "duplicate unit id" );
	m_units.insert_or_assign( unit->m_id, unit );

	QueueUnitUpdate( unit, UUO_SPAWN );

	auto* state = m_game->GetState();
	if ( state->IsMaster() ) {
		state->TriggerObject( this, "unit_spawn", ARGS_F( &unit, this ) {
			{
				"unit",
				unit->Wrap( GSE_CALL )
			},
		}; } );
	}
}

void UnitManager::DespawnUnit( GSE_CALLABLE, const size_t unit_id ) {

	const auto& it = m_units.find( unit_id );
	if ( it == m_units.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unit id " + std::to_string( unit_id ) + " not found" );
	}

	auto* unit = it->second;
	if ( !GetCargo( unit ).empty() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Transport cargo must be despawned before its carrier" );
	}

	Log( "Despawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->GetTile()->ToString() );

	QueueUnitUpdate( unit, UUO_DESPAWN );

	auto* tile = unit->GetTile();
	ASSERT( tile, "unit tile not set" );
	const auto& tile_it = tile->units.find( unit->m_id );
	ASSERT( tile_it != tile->units.end(), "unit id not found in tile" );
	tile->units.erase( tile_it );

	m_units.erase( it );

	auto* state = m_game->GetState();
	if ( state->IsMaster() ) {
		state->TriggerObject( this, "unit_despawn", ARGS_F( &unit ) {
			{
				"unit",
				unit->Wrap( GSE_CALL )
			}
		}; } );
	}

	delete unit;
}

MoraleSet* UnitManager::GetMoraleSet( const std::string& name ) const {
	const auto& it = m_unit_moralesets.find( name );
	if ( it != m_unit_moralesets.end() ) {
		return it->second;
	}
	return nullptr;
}

Unit* UnitManager::GetUnit( const size_t id ) const {
	const auto& it = m_units.find( id );
	if ( it != m_units.end() ) {
		return it->second;
	}
	return nullptr;
}

Def* UnitManager::GetUnitDef( const std::string& name ) const {
	const auto& it = m_unit_defs.find( name );
	if ( it != m_unit_defs.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

const std::map< size_t, Unit* >& UnitManager::GetUnits() const {
	return m_units;
}

std::vector< Unit* > UnitManager::GetCargo( const Unit* transport ) const {
	std::vector< Unit* > result = {};
	std::unordered_set< size_t > seen = {};
	const auto collect = [ &result, &seen, transport ]( Unit* unit ) {
		if ( unit->m_transport_id == transport->m_id && seen.insert( unit->m_id ).second ) {
			result.push_back( unit );
		}
	};
	for ( const auto& it : m_units ) {
		collect( it.second );
	}
	for ( auto* const unit : m_unprocessed_units ) {
		collect( unit );
	}
	std::sort(
		result.begin(),
		result.end(),
		[]( const Unit* left, const Unit* right ) { return left->m_id < right->m_id; }
	);
	return result;
}

const std::string* UnitManager::ValidateEmbark( const Unit* unit, const Unit* transport ) const {
	if ( !unit || !transport ) {
		return new std::string( "Cargo and transport must exist" );
	}
	if ( unit == transport ) {
		return new std::string( "Unit cannot embark on itself" );
	}
	if ( unit->m_transport_id != 0 ) {
		return new std::string( "Unit is already embarked" );
	}
	if ( transport->m_transport_id != 0 ) {
		return new std::string( "Cannot embark on another unit's cargo" );
	}
	if ( unit->m_owner != transport->m_owner ) {
		return new std::string( "Unit can only embark on a friendly transport" );
	}
	if ( unit->GetTile() != transport->GetTile() ) {
		return new std::string( "Unit and transport must occupy the same tile" );
	}
	if ( unit->m_health <= 0.0f || transport->m_health <= 0.0f ) {
		return new std::string( "Destroyed units cannot embark or carry cargo" );
	}
	if ( unit->m_terraforming != map::tile::TERRAFORMING_NONE ) {
		return new std::string( "Unit must cancel its terraforming order before embarking" );
	}
	ASSERT( unit->m_def->m_type == DT_STATIC, "only static cargo is supported" );
	ASSERT( transport->m_def->m_type == DT_STATIC, "only static transports are supported" );
	const auto* const cargo_def = static_cast< const StaticDef* >( unit->m_def );
	const auto* const transport_def = static_cast< const StaticDef* >( transport->m_def );
	if ( cargo_def->m_movement_type != MT_LAND ) {
		return new std::string( "Only land units can embark on troop transports" );
	}
	if ( cargo_def->m_cargo_capacity > 0 ) {
		return new std::string( "Transports cannot be nested" );
	}
	if ( transport_def->m_movement_type == MT_LAND || transport_def->m_cargo_capacity <= 0 ) {
		return new std::string( "Target unit is not a sea or air transport" );
	}
	if ( GetCargo( transport ).size() >= static_cast< size_t >( transport_def->m_cargo_capacity ) ) {
		return new std::string( "Transport has no remaining cargo capacity" );
	}
	return nullptr;
}

void UnitManager::EmbarkUnit( GSE_CALLABLE, Unit* unit, Unit* transport ) {
	m_game->CheckRW( GSE_CALL );
	const auto* const error = ValidateEmbark( unit, transport );
	if ( error ) {
		const auto message = *error;
		delete error;
		GSE_ERROR( gse::EC.GAME_ERROR, message );
	}
	unit->SetTransportId( transport->m_id );
	RefreshUnit( GSE_CALL, unit );
}

void UnitManager::DisembarkUnit( GSE_CALLABLE, Unit* unit ) {
	m_game->CheckRW( GSE_CALL );
	if ( !unit || unit->m_transport_id == 0 ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unit is not embarked" );
	}
	if ( !GetUnit( unit->m_transport_id ) ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unit transport no longer exists" );
	}
	unit->SetTransportId( 0 );
	RefreshUnit( GSE_CALL, unit );
}

void UnitManager::ProcessUnprocessed( GSE_CALLABLE ) {
	ASSERT( m_game->IsRunning(), "game not running" );
	for ( auto* const it : m_unprocessed_units ) {
		SpawnUnit( GSE_CALL, it );
	}
	m_unprocessed_units.clear();
}

void UnitManager::PushUpdates() {
	if ( m_game->IsRunning() && !m_unit_updates.empty() ) {
		for ( const auto& it : m_unit_updates ) {
			const auto unit_id = it.first;
			const auto& uu = it.second;
			const auto& unit = uu.unit;
			if ( uu.ops & UUO_SPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_SPAWN );
				fr.data.unit_spawn.unit_id = unit->m_id;
				NEW( fr.data.unit_spawn.unitdef_id, std::string, unit->m_def->m_id );
				fr.data.unit_spawn.slot_index = unit->m_owner->GetIndex();
				const auto* tile = unit->GetTile();
				fr.data.unit_spawn.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = unit->GetRenderCoords();
				fr.data.unit_spawn.render_coords = {
					c.x,
					c.y,
					c.z
				};

				fr.data.unit_spawn.movement = unit->m_movement;
				fr.data.unit_spawn.morale = unit->m_morale;
				NEW( fr.data.unit_spawn.morale_string, std::string, unit->GetMoraleString() );
				fr.data.unit_spawn.health = unit->m_health;
				fr.data.unit_spawn.embarked = unit->m_transport_id != 0;
				m_game->AddFrontendRequest( fr );
			}
			if ( uu.ops & UUO_REFRESH ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_UPDATE );
				fr.data.unit_update.unit_id = unit->m_id;
				fr.data.unit_update.movement = unit->m_movement;
				fr.data.unit_update.morale = unit->m_morale;
				NEW( fr.data.unit_update.morale_string, std::string, unit->GetMoraleString() );
				fr.data.unit_update.health = unit->m_health;
				fr.data.unit_update.embarked = unit->m_transport_id != 0;
				const auto* tile = unit->GetTile();
				fr.data.unit_update.tile_coords = {
					tile->coord.x,
					tile->coord.y
				};
				const auto c = unit->GetRenderCoords();
				fr.data.unit_update.render_coords = {
					c.x,
					c.y,
					c.z
				};
				m_game->AddFrontendRequest( fr );
			}
			if ( uu.ops & UUO_DESPAWN ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DESPAWN );
				fr.data.unit_despawn.unit_id = unit_id;
				m_game->AddFrontendRequest( fr );
			}
		}
		m_unit_updates.clear();
		m_game->CheckTurnComplete();
	}
}

WRAPIMPL_BEGIN( UnitManager )
	WRAPIMPL_PROPS
	WRAPIMPL_TRIGGERS
		{
			"define_moraleset",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( arr, 1, Array );

				if ( m_unit_moralesets.find( id ) != m_unit_moralesets.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Moraleset \"" + id + "\" already exists" );
				}

				const uint8_t expected_count = unit::MORALE_MAX - unit::MORALE_MIN + 1;
				if ( arr.size() != expected_count ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Moraleset must have exactly " + std::to_string( expected_count ) + " values (found " + std::to_string( arr.size() ) + ")");
				}
				unit::MoraleSet::morale_values_t values = {};
				for ( const auto& v : arr ) {
					if ( v->type != gse::VT_OBJECT ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Moraleset elements must be objects");
					}
					const auto* obj = (gse::value::Object*)v;
					N_GETPROP( name, obj->value, "name", String );
					values.push_back( unit::Morale{ name } );
				}

				DefineMoraleSet( new unit::MoraleSet( id, values ) );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"undefine_moraleset",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				if ( m_unit_moralesets.find( id ) == m_unit_moralesets.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Moraleset \"" + id + "\" not found" );
				}

				UndefineMoraleSet( id );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"get_moraleset",
			NATIVE_CALL( this ) {

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				const auto& moraleset = m_unit_moralesets.find( id );
				if ( moraleset == m_unit_moralesets.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Moraleset \"" + id + "\" not found" );
				}

				gse::value::array_elements_t result = {};
				for ( const auto& morale : moraleset->second->m_morale_values ) {
					// TODO: refactor with Wrap()
					result.push_back( VALUE( gse::value::String,, morale.m_name ) );
				}

				return VALUE( gse::value::Array,, result );
			} )
		},
		{
			"define_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( unit_def, 1, Object );
				N_GETPROP( name, unit_def, "name", String );
				N_GETPROP( morale, unit_def, "morale", String );
				N_GETPROP( unit_type, unit_def, "type", String );
				N_GETPROP( mineral_cost, unit_def, "mineral_cost", Int );
				N_GETPROP_OPT( std::string, required_technology, unit_def, "required_technology", String, "" );
				N_GETPROP_OPT( bool, is_native, unit_def, "is_native", Bool, morale == "NATIVE" );
				N_GETPROP_OPT( int64_t, offense, unit_def, "offense", Int, 1 );
				N_GETPROP_OPT( int64_t, defense, unit_def, "defense", Int, 1 );
				N_GETPROP_OPT_BOOL( can_found_base, unit_def, "can_found_base" );
				N_GETPROP_OPT_BOOL( can_terraform, unit_def, "can_terraform" );
				N_GETPROP_OPT( std::string, chassis_id, unit_def, "chassis", String, "" );
				N_GETPROP_OPT( std::string, weapon_id, unit_def, "weapon", String, "" );
				N_GETPROP_OPT( std::string, armor_id, unit_def, "armor", String, "" );
				N_GETPROP_OPT( std::string, reactor_id, unit_def, "reactor", String, "" );
				N_GETPROP_OPT( int64_t, reactor_power, unit_def, "reactor_power", Int, 1 );
				N_GETPROP_OPT( int64_t, operational_range, unit_def, "operational_range", Int, 0 );
				N_GETPROP_OPT_BOOL( is_missile, unit_def, "is_missile" );
				N_GETPROP_OPT( int64_t, cargo_capacity, unit_def, "cargo_capacity", Int, 0 );
				N_GETPROP_OPT( bool, buildable, unit_def, "buildable", Bool, true );
				N_GETPROP_OPT(
					gse::value::array_elements_t,
					ability_values,
					unit_def,
					"abilities",
					Array,
					gse::value::array_elements_t()
				);
				std::set< std::string > abilities = {};
				if ( ability_values.size() > unit::StaticDef::MAX_ABILITIES ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Too many unit abilities: " + id );
				}
				for ( size_t i = 0 ; i < ability_values.size() ; i++ ) {
					N_GETELEMENT( ability_id, ability_values, i, String );
					if ( ability_id.empty() || !abilities.insert( ability_id ).second ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unit abilities must be unique, non-empty strings: " + id );
					}
				}
				if ( mineral_cost < 0 || mineral_cost > unit::Def::MAX_MINERAL_COST ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit mineral cost: " + std::to_string( mineral_cost ) );
				}
				if (
					offense < 0 ||
					offense > unit::Def::MAX_COMBAT_STRENGTH ||
					defense <= 0 ||
					defense > unit::Def::MAX_COMBAT_STRENGTH ||
					reactor_power < 1 ||
					reactor_power > 4 ||
					( can_found_base && can_terraform )
				) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit combat or capability values: " + id );
				}

				if ( m_unit_defs.find( id ) != m_unit_defs.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit def '" + id + "' already exists");
				}

				if ( unit_type == "static" ) {
					N_GETPROP( movement_type_str, unit_def, "movement_type", String );
					unit::movement_type_t movement_type;
					if ( movement_type_str == "land" ) {
						movement_type = unit::MT_LAND;
					}
					else if ( movement_type_str == "water" ) {
						movement_type = unit::MT_WATER;
					}
					else if ( movement_type_str == "air" ) {
						movement_type = unit::MT_AIR;
					}
					else if ( movement_type_str == "immovable" ) {
						movement_type = unit::MT_IMMOVABLE;
					}
					else {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid movement type: " + movement_type_str + ". Specify one of: land water air immovable");
					}
					if (
						( can_found_base && movement_type != unit::MT_LAND && movement_type != unit::MT_WATER ) ||
						( can_terraform && movement_type != unit::MT_LAND )
					) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid movement type for founding or terraforming unit: " + id );
					}
					if (
						operational_range < 0 ||
						operational_range > unit::StaticDef::MAX_OPERATIONAL_RANGE ||
						( movement_type != unit::MT_AIR && ( operational_range > 0 || is_missile ) ) ||
						( is_missile && operational_range == 0 ) ||
						cargo_capacity < 0 ||
						cargo_capacity > unit::StaticDef::MAX_CARGO_CAPACITY ||
						( cargo_capacity > 0 && ( movement_type == unit::MT_IMMOVABLE || is_missile ) )
					) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit operational range: " + id );
					}
					N_GETPROP( movement_per_turn, unit_def, "movement_per_turn", Int );
					if ( movement_per_turn < 0 ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit movement per turn: " + id );
					}
					N_GETPROP( render_def, unit_def, "render", Object );
					N_GETPROP( render_type, render_def, "type", String );
					if ( render_type == "sprite" ) {
						N_GETPROP( sprite_file, render_def, "file", String );
						N_GETPROP( sprite_x, render_def, "x", Int );
						N_GETPROP( sprite_y, render_def, "y", Int );
						N_GETPROP( sprite_w, render_def, "w", Int );
						N_GETPROP( sprite_h, render_def, "h", Int );
						N_GETPROP( sprite_cx, render_def, "cx", Int );
						N_GETPROP( sprite_cy, render_def, "cy", Int );
						N_GETPROP_OPT_INT( sprite_morale_based_xshift, render_def, "morale_based_xshift" );
						const auto* moraleset = GetMoraleSet( morale );
						if ( !moraleset ) {
							GSE_ERROR( gse::EC.INVALID_CALL, "Morale type '" + morale + "' is not defined");
						}
						auto* def = new unit::StaticDef(
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
							movement_type,
							movement_per_turn,
							new unit::SpriteRender(
								sprite_file,
								sprite_x,
								sprite_y,
								sprite_w,
								sprite_h,
								sprite_cx,
									sprite_cy,
									sprite_morale_based_xshift
								),
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

						DefineUnit( def );

						return VALUE( gse::value::Undefined );
					}
					else {
						GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported render type: " + render_type );
					}
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported unit type: " + unit_type );
				}
			})
		},
		{
			"undefine_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );

				if ( m_unit_defs.find( id ) == m_unit_defs.end() ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit def '" + id + "' not found" );
				}

				UndefineUnit( id );

				return VALUE( gse::value::Undefined );
			} )
		},
		{
			"get_unit_def",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( id, 0, String );
				auto* const def = GetUnitDef( id );
				if ( !def ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit type '" + id + "' is not defined" );
				}
				return def->Wrap( GSE_CALL );
			} )
		},
		{
			"get_unit_defs",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 0 );
				std::vector< unit::Def* > defs = {};
				defs.reserve( m_unit_defs.size() );
				for ( const auto& it : m_unit_defs ) {
					defs.push_back( it.second );
				}
				std::sort(
					defs.begin(),
					defs.end(),
					[]( const unit::Def* left, const unit::Def* right ) {
						return left->m_id < right->m_id;
					}
				);
				gse::value::array_elements_t result = {};
				result.reserve( defs.size() );
				for ( auto* const def : defs ) {
					result.push_back( def->Wrap( GSE_CALL ) );
				}
				return VALUE( gse::value::Array,, result );
			} )
		},
		{
			"has_unit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( unit_id, 0, Int );
				return VALUE( gse::value::Bool,, m_units.find( unit_id ) != m_units.end() );
			} )
		},
		{
			"get_unit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE( unit_id, 0, Int );
				const auto& it = m_units.find( unit_id );
				if ( it != m_units.end() ) {
					return it->second->Wrap( GSE_CALL );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit id " + std::to_string( unit_id ) + " not found" );
				}
			} )
		},
		{
			"get_units",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS_MAX( 1 );
				bool include_embarked = false;
				if ( !arguments.empty() ) {
					N_GETVALUE( requested_include_embarked, 0, Bool );
					include_embarked = requested_include_embarked;
				}
				gse::value::array_elements_t result = {};
				result.reserve( m_units.size() );
				for ( const auto& it : m_units ) {
					if (
						it.second->m_health > 0.0f &&
						(include_embarked || it.second->m_transport_id == 0)
					) {
						result.push_back( it.second->Wrap( GSE_CALL ) );
					}
				}
				return VALUE( gse::value::Array,, result );
			} )
		},
		{
			"spawn_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				N_GETVALUE( obj, 0, Object );

				N_GETPROP_OPT( size_t, unit_id, obj, "id", Int, 0 );
				N_GETPROP( def_name, obj, "def", String );
				N_GETPROP_UNWRAP( owner, obj, "owner", Player );
				N_GETPROP_UNWRAP( tile, obj, "tile", map::tile::Tile );
				N_GETPROP( morale, obj, "morale", Int );
				N_GETPROP( health, obj, "health", Float );
				N_GETPROP_OPT( std::string, terraforming_name, obj, "terraforming", String, "none" );
				N_GETPROP_OPT( int64_t, terraforming_turns_remaining, obj, "terraforming_turns_remaining", Int, 0 );
				N_GETPROP_OPT( size_t, home_base_id, obj, "home_base_id", Int, 0 );
				N_GETPROP_OPT( int64_t, fuel, obj, "fuel", Int, 0 - 1 );
				N_GETPROP_OPT( size_t, transport_id, obj, "transport_id", Int, 0 );
				if ( home_base_id > 0 && m_game->IsRunning() ) {
					auto* const home_base = m_game->GetBM()->GetBase( home_base_id );
					if ( !home_base ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unit home base does not exist" );
					}
					if ( home_base->m_owner != owner->GetSlot() ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unit home base belongs to another player" );
					}
				}
				const auto terraforming = map::tile::Tile::GetTerraformingFromString( terraforming_name );
				if (
					( terraforming == map::tile::TERRAFORMING_NONE && util::String::GetLowerCase( terraforming_name ) != "none" ) ||
					terraforming_turns_remaining < 0 ||
					terraforming_turns_remaining > unit::Unit::MAX_TERRAFORMING_TURNS
				) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Invalid unit terraforming order" );
				}

				auto* def = GetUnitDef( def_name );
				if ( !def ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit type '" + def_name + "' is not defined" );
				}
				ASSERT( def->m_type == unit::DT_STATIC, "only static defs are supported for now" );
				const auto* staticdef = (unit::StaticDef*)def;
				if ( fuel < -1 ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Unit fuel cannot be negative" );
				}
				if ( fuel < 0 ) {
					fuel = staticdef->m_operational_range;
				}
				if ( fuel > staticdef->m_operational_range ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Unit fuel exceeds its operational range" );
				}
				auto unit = std::make_unique< unit::Unit >(
					GSE_CALL,
					this,
					unit_id ? unit_id : unit::Unit::GetNextId(),
					def,
					owner->GetSlot(),
					tile,
					staticdef->m_movement_per_turn,
					morale,
					health,
					false,
					terraforming,
					static_cast< uint16_t >( terraforming_turns_remaining ),
					home_base_id,
					static_cast< uint16_t >( fuel ),
					0
				);
				if ( transport_id > 0 ) {
					auto* const transport = GetUnit( transport_id );
					const auto* const error = ValidateEmbark( unit.get(), transport );
					if ( error ) {
						const auto message = *error;
						delete error;
						GSE_ERROR( gse::EC.INVALID_CALL, message );
					}
					unit->SetTransportId( transport_id );
				}
				auto* const result = unit.release();
				SpawnUnit( GSE_CALL, result );
				return result->Wrap( GSE_CALL );
			})
		},
		{
			"despawn_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				if ( arguments.at( 0 )->type == gse::VT_INT ) {
					N_GETVALUE( unit_id, 0, Int );
					DespawnUnit( GSE_CALL, unit_id );
				}
				else {
					N_GETVALUE_UNWRAP( unit, 0, Unit );
					DespawnUnit( GSE_CALL, unit->m_id );
				}
				return VALUE( gse::value::Undefined );
			})
		},
	};
WRAPIMPL_END_PTR()

UNWRAPIMPL_PTR( UnitManager )

void UnitManager::Serialize( types::Buffer& buf ) const {

	Log( "Serializing " + std::to_string( m_unit_moralesets.size() ) + " unit moralesets" );
	buf.WriteInt( m_unit_moralesets.size() );
	for ( const auto& it : m_unit_moralesets ) {
		buf.WriteString( it.first );
		buf.WriteString( MoraleSet::Serialize( it.second ).ToString() );
	}

	Log( "Serializing " + std::to_string( m_unit_defs.size() ) + " unit defs" );
	buf.WriteInt( m_unit_defs.size() );
	for ( const auto& it : m_unit_defs ) {
		buf.WriteString( it.first );
		buf.WriteString( Def::Serialize( it.second ).ToString() );
	}

	size_t serialized_units = 0;
	for ( const auto& it : m_units ) {
		if ( it.second->m_health > 0.0f ) {
			serialized_units++;
		}
	}
	Log( "Serializing " + std::to_string( serialized_units ) + " units" );
	buf.WriteInt( serialized_units );
	for ( const auto& it : m_units ) {
		if ( it.second->m_health > 0.0f ) {
			buf.WriteString( Unit::Serialize( it.second ).ToString() );
		}
	}
	buf.WriteInt( Unit::GetNextId() );

	Log( "Saved next unit id: " + std::to_string( Unit::GetNextId() ) );
}

void UnitManager::Deserialize( GSE_CALLABLE, types::Buffer& buf ) {
	if (
		!m_unit_moralesets.empty() ||
		!m_unit_defs.empty() ||
		!m_units.empty() ||
		!m_unprocessed_units.empty()
	) {
		THROW( "cannot deserialize units into a non-empty manager" );
	}

	size_t sz = buf.ReadCollectionSize( "unit morale set" );
	Log( "Unserializing " + std::to_string( sz ) + " unit moralesets" );
	m_unit_moralesets.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		auto moraleset = std::unique_ptr< MoraleSet >( MoraleSet::Deserialize( b ) );
		if ( b.GetRemaining() != 0 ) {
			THROW( "unexpected data after serialized unit morale set" );
		}
		if ( name != moraleset->m_id ) {
			THROW( "serialized unit morale set id mismatch" );
		}
		if ( m_unit_moralesets.find( name ) != m_unit_moralesets.end() ) {
			THROW( "duplicate serialized unit morale set: " + name );
		}
		DefineMoraleSet( moraleset.release() );
	}

	sz = buf.ReadCollectionSize( "unit definition" );
	Log( "Unserializing " + std::to_string( sz ) + " unit defs" );
	m_unit_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		auto def = std::unique_ptr< Def >( Def::Deserialize( b ) );
		if ( b.GetRemaining() != 0 ) {
			THROW( "unexpected data after serialized unit definition" );
		}
		if ( name != def->m_id ) {
			THROW( "serialized unit definition id mismatch" );
		}
		if ( m_unit_defs.find( name ) != m_unit_defs.end() ) {
			THROW( "duplicate serialized unit definition: " + name );
		}
		DefineUnit( def.release() );
	}

	sz = buf.ReadCollectionSize( "unit" );
	Log( "Unserializing " + std::to_string( sz ) + " units" );
	if ( !m_game->IsRunning() ) {
		m_unprocessed_units.reserve( sz );
	}
	std::unordered_set< size_t > serialized_unit_ids = {};
	size_t max_unit_id = 0;
	for ( size_t i = 0 ; i < sz ; i++ ) {
		auto b = types::Buffer( buf.ReadString() );
		auto id_buffer = b;
		const auto id = id_buffer.ReadInt< size_t >( "unit id" );
		if ( id == 0 || !serialized_unit_ids.insert( id ).second ) {
			THROW( "invalid or duplicate serialized unit id: " + std::to_string( id ) );
		}
		max_unit_id = std::max( max_unit_id, id );
		auto unit = std::unique_ptr< Unit >( Unit::Deserialize( GSE_CALL, b, this ) );
		SpawnUnit( GSE_CALL, unit.release() );
	}
	ValidateTransports();

	const auto next_unit_id = buf.ReadInt< size_t >( "next unit id" );
	if ( next_unit_id == 0 || next_unit_id <= max_unit_id ) {
		THROW( "invalid serialized next unit id" );
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized unit manager" );
	}
	Unit::SetNextId( next_unit_id );
	Log( "Restored next unit id: " + std::to_string( Unit::GetNextId() ) );
}

void UnitManager::ValidateHomeBases() const {
	const auto validate = [ this ]( const Unit* unit ) {
		if ( unit->m_home_base_id == 0 ) {
			return;
		}
		auto* const home_base = m_game->GetBM()->GetBase( unit->m_home_base_id );
		if ( !home_base || home_base->m_owner != unit->m_owner ) {
			THROW( "unit #" + std::to_string( unit->m_id ) + " has an invalid home base" );
		}
	};
	for ( const auto& it : m_units ) {
		validate( it.second );
	}
	for ( const auto* unit : m_unprocessed_units ) {
		validate( unit );
	}
}

void UnitManager::ValidateTransports() const {
	std::unordered_map< size_t, Unit* > units = {};
	for ( const auto& it : m_units ) {
		units.insert( it );
	}
	for ( auto* const unit : m_unprocessed_units ) {
		if ( !units.insert( { unit->m_id, unit } ).second ) {
			THROW( "duplicate unit id while validating transports" );
		}
	}
	std::unordered_map< size_t, size_t > cargo_counts = {};
	for ( const auto& it : units ) {
		auto* const cargo = it.second;
		if ( cargo->m_transport_id == 0 ) {
			continue;
		}
		const auto transport_it = units.find( cargo->m_transport_id );
		if ( transport_it == units.end() ) {
			THROW( "unit #" + std::to_string( cargo->m_id ) + " has a missing transport" );
		}
		auto* const transport = transport_it->second;
		if (
			cargo == transport ||
			cargo->m_owner != transport->m_owner ||
			cargo->GetTile() != transport->GetTile() ||
			transport->m_transport_id != 0 ||
			cargo->m_def->m_type != DT_STATIC ||
			transport->m_def->m_type != DT_STATIC
		) {
			THROW( "unit #" + std::to_string( cargo->m_id ) + " has an invalid transport relationship" );
		}
		const auto* const cargo_def = static_cast< const StaticDef* >( cargo->m_def );
		const auto* const transport_def = static_cast< const StaticDef* >( transport->m_def );
		if (
			cargo_def->m_movement_type != MT_LAND ||
			cargo_def->m_cargo_capacity > 0 ||
			transport_def->m_movement_type == MT_LAND ||
			transport_def->m_cargo_capacity <= 0
		) {
			THROW( "unit #" + std::to_string( cargo->m_id ) + " has incompatible cargo or transport definitions" );
		}
		const auto count = ++cargo_counts[ transport->m_id ];
		if ( count > static_cast< size_t >( transport_def->m_cargo_capacity ) ) {
			THROW( "unit #" + std::to_string( transport->m_id ) + " exceeds its cargo capacity" );
		}
	}
}

void UnitManager::QueueUnitUpdate( const Unit* unit, const unit_update_op_t op ) {
	auto it = m_unit_updates.find( unit->m_id );
	if ( it == m_unit_updates.end() ) {
		it = m_unit_updates.insert(
			{
				unit->m_id,
				{
					{},
					unit,
				}
			}
		).first;
	}
	auto& update = it->second;
	if ( op == UUO_DESPAWN ) {
		if ( update.ops & UUO_SPAWN ) {
			// if unit is despawned immediately after spawning - frontend doesn't need to know
			m_unit_updates.erase( it );
			return;
		}
		update.ops = UUO_NONE; // clear other actions if unit was despawned
	}
	if ( op == UUO_SPAWN || op == UUO_REFRESH ) {
		if ( update.ops & UUO_DESPAWN ) {
			// do not despawn if it needs to spawn or refresh, i.e. if event was rolled back
			update.ops = (unit_update_op_t)( (uint8_t)update.ops & ~UUO_DESPAWN);
			if ( op == UUO_SPAWN ) {
				// if there's pending despawn event it means unit was already spawned, nothing to do
				m_unit_updates.erase( it );
				return;
			}
		}
	}
	// add to operations list
	update.ops = (unit_update_op_t)( (uint8_t)update.ops | (uint8_t)op );
}

const morale_t UnitManager::GetMorale( GSE_CALLABLE, const int64_t& morale ) {
	if ( morale < MORALE_MIN || morale > MORALE_MAX ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid morale value: " + std::to_string( morale ) + " (should be between " + std::to_string( MORALE_MIN ) + " and " + std::to_string( MORALE_MAX ) + ", inclusive)" );
	}
	return (morale_t)morale;
}

const health_t UnitManager::GetHealth( GSE_CALLABLE, const float health ) {
	if ( health < Unit::MINIMUM_HEALTH_TO_KEEP || health > StaticDef::HEALTH_MAX ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid health value: " + std::to_string( health ) + " (should be between " + std::to_string( Unit::MINIMUM_HEALTH_TO_KEEP ) + " and " + std::to_string( StaticDef::HEALTH_MAX ) + ", inclusive)" );
	}
	if ( health == 0 ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid health value: " + std::to_string( health ) + " (you can't spawn a dead unit)" );
	}
	return (health_t)health;
}

const std::string* UnitManager::MoveUnitToTile( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile, const cb_oncomplete& on_complete ) {

	auto* tm = m_game->GetTM();
	auto* am = m_game->GetAM();

	if ( unit->m_animation_id ) {
		// stop any previous animation
		am->FinishAnimation( unit->m_animation_id );
	}

	auto* src_tile = unit->GetTile();
	if ( src_tile == dst_tile ) {
		return new std::string( "Unit can't move because it's already on target tile" );
	}

	const std::unordered_set< map::tile::Tile* > tiles_to_lock = { src_tile, dst_tile };

	const auto* locked_tile = tm->FindLockedTile( tiles_to_lock );
	if ( locked_tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Tile " + locked_tile->coord.ToString() + " is locked" );
	}

	m_game->GetTM()->LockTiles( m_game->GetSlotNum(), tiles_to_lock );

	if ( !src_tile->IsLocked() ) {
		return new std::string( "Source tile must be locked before moving unit" );
	}
	if ( !dst_tile->IsLocked() ) {
		return new std::string( "Destination tile must be locked before moving unit" );
	}
	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_MOVE );
	fr.data.unit_move.unit_id = unit->m_id;
	fr.data.unit_move.dst_tile_coords = {
		dst_tile->coord.x,
		dst_tile->coord.y
	};
	fr.data.unit_move.running_animation_id = unit->m_animation_id = am->AddAnimationCallback(
		[ this, tm, tiles_to_lock, on_complete, unit ]( const size_t animation_id ) {
			ASSERT( unit->m_animation_id, "animation id gone" );
			unit->m_animation_id = 0;
			tm->UnlockTiles( m_game->GetSlotNum(), tiles_to_lock );
			on_complete();
		}
	);
	unit->SetTile( GSE_CALL, dst_tile );
	for ( auto* const cargo : GetCargo( unit ) ) {
		cargo->SetTile( GSE_CALL, dst_tile );
		RefreshUnit( GSE_CALL, cargo );
	}
	m_game->AddFrontendRequest( fr );

	return nullptr; // no error
}

const std::string* UnitManager::TeleportUnitToTile( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile ) {
	if ( !unit || !dst_tile ) {
		return new std::string( "Unit and destination tile must exist" );
	}
	if ( unit->m_animation_id ) {
		return new std::string( "Unit cannot teleport while another movement is active" );
	}
	auto* const src_tile = unit->GetTile();
	if ( src_tile == dst_tile ) {
		return new std::string( "Unit is already on the destination tile" );
	}
	if ( src_tile->IsLocked() || dst_tile->IsLocked() ) {
		return new std::string( "Unit cannot teleport through a locked tile" );
	}

	const auto teleport = [ this, dst_tile ]( GSE_CALLABLE, Unit* const target ) {
		auto fr = FrontendRequest( FrontendRequest::FR_UNIT_TELEPORT );
		fr.data.unit_teleport.unit_id = target->m_id;
		fr.data.unit_teleport.dst_tile_coords = {
			dst_tile->coord.x,
			dst_tile->coord.y
		};
		target->SetTile( GSE_CALL, dst_tile );
		m_game->AddFrontendRequest( fr );
		RefreshUnit( GSE_CALL, target );
	};

	teleport( GSE_CALL, unit );
	for ( auto* const cargo : GetCargo( unit ) ) {
		teleport( GSE_CALL, cargo );
	}
	return nullptr;
}

const std::string* UnitManager::AttackUnitValidate( GSE_CALLABLE, Unit* attacker, Unit* defender ) {
	const auto result = m_game->GetState()->TriggerObject( this, "unit_attack_validate", ARGS_F( &attacker, &defender ) {
		{
			"attacker",
			attacker->Wrap( GSE_CALL )
		},
		{
			"defender",
			defender->Wrap( GSE_CALL )
		},
	}; } );
	switch ( result->type ) {
		case gse::VT_NULL:
		case gse::VT_UNDEFINED:
			return nullptr; // no errors
		case gse::VT_STRING:
			return new std::string( ( (gse::value::String*)result )->value ); // error
		default:
			THROW( "unexpected validation result type: " + result->GetTypeString() );
	}
}

gse::Value* const UnitManager::AttackUnitResolve( GSE_CALLABLE, Unit* attacker, Unit* defender ) {
	return m_game->GetState()->TriggerObject( this, "unit_attack_resolve", ARGS_F( &attacker, &defender ) {
		{
			"attacker",
			attacker->Wrap( GSE_CALL )
		},
		{
			"defender",
			defender->Wrap( GSE_CALL )
		},
	}; } );
}

void UnitManager::AttackUnitApply( GSE_CALLABLE, Unit* attacker, Unit* defender, gse::Value* const resolutions ) {
	auto* state = m_game->GetState();
	state->TriggerObject( this, "unit_attack_apply", ARGS_F( &attacker, &defender, &resolutions ) {
		{
			"attacker",
			attacker->Wrap( GSE_CALL )
		},
		{
			"defender",
			defender->Wrap( GSE_CALL )
		},
		{
			"resolutions",
			resolutions
		}
	}; } );
	RefreshUnit( GSE_CALL, attacker );
	RefreshUnit( GSE_CALL, defender );
}

void UnitManager::RefreshUnit( GSE_CALLABLE, const Unit* unit ) {
	QueueUnitUpdate( unit, UUO_REFRESH );
}

map::Map* UnitManager::GetMap() const {
	return m_game->GetMap();
}

slot::Slot* UnitManager::GetSlot( const size_t slot_num ) const {
	return &m_game->GetState()->m_slots->GetSlot( slot_num );
}

}
}
}
