#include "UnitManager.h"

#include "MoraleSet.h"
#include "Def.h"
#include "StaticDef.h"
#include "Unit.h"

#include "game/backend/Game.h"
#include "game/backend/animation/AnimationManager.h"
#include "game/backend/State.h"
#include "game/backend/Bindings.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/event/DefineMorales.h"
#include "game/backend/event/DefineUnit.h"
#include "game/backend/event/SpawnUnit.h"
#include "game/backend/event/DespawnUnit.h"
#include "game/backend/unit/SpriteRender.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Bool.h"
#include "gse/value/Float.h"
#include "gse/value/Array.h"

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

void UnitManager::SpawnUnit( GSE_CALLABLE, Unit* unit ) {
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
		state->TriggerObject( this, "unit_spawn", ARGS_F( &ctx, gc_space, &si, &ep, &unit, this ) {
			{
				"unit",
				unit->Wrap( GSE_CALL, m_game->GetGCSpace() )
			},
		}; } );
	}
}

void UnitManager::DespawnUnit( GSE_CALLABLE, const size_t unit_id ) {

	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

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
		state->TriggerObject( this, "unit_despawn", ARGS_F( &ctx, gc_space, &si, &ep, &unit ) {
			{
				"unit",
				unit->Wrap( GSE_CALL )
			}
		}; } );
	}

	delete unit;
}

void UnitManager::SkipUnitTurn( GSE_CALLABLE, const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Skipping unit turn #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->GetTile()->ToString() );

	unit->m_movement = 0.0f;

	RefreshUnit( GSE_CALL, unit );
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

void UnitManager::ProcessUnprocessed( GSE_CALLABLE ) {
	ASSERT( m_game->IsRunning(), "game not running" );
	for ( auto& it : m_unprocessed_units ) {
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
				m_game->AddFrontendRequest( fr );
			}
			if ( uu.ops & UUO_REFRESH ) {
				auto fr = FrontendRequest( FrontendRequest::FR_UNIT_UPDATE );
				fr.data.unit_update.unit_id = unit->m_id;
				fr.data.unit_update.movement = unit->m_movement;
				fr.data.unit_update.health = unit->m_health;
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
			"define_morales",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( arr, 1, Array );
				const uint8_t expected_count = unit::MORALE_MAX - unit::MORALE_MIN + 1;
				if ( arr.size() != expected_count ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Morale set must have exactly " + std::to_string( expected_count ) + " values (found " + std::to_string( arr.size() ) + ")");
				}
				unit::MoraleSet::morale_values_t values = {};
				for ( const auto& v : arr ) {
					if ( v->type != gse::Value::T_OBJECT ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Morale set elements must be objects");
					}
					const auto* obj = (gse::value::Object*)v;
					N_GETPROP( name, obj->value, "name", String );
					values.push_back( unit::Morale{ name } );
				}
				return m_game->AddEvent( GSE_CALL, new event::DefineMorales( m_game->GetSlotNum(), new unit::MoraleSet( id, values ) ) );
			} )
		},
		{
			"define_unit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( unit_def, 1, Object );
				N_GETPROP( name, unit_def, "name", String );
				N_GETPROP( morale, unit_def, "morale", String );
				N_GETPROP( unit_type, unit_def, "type", String );
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
					N_GETPROP( movement_per_turn, unit_def, "movement_per_turn", Int );
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
							)
						);
						return m_game->AddEvent( GSE_CALL, new event::DefineUnit( m_game->GetSlotNum(), def ) );
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
			"spawn_unit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 5 );
				N_GETVALUE( def_name, 0, String );
				N_GETVALUE_UNWRAP( owner, 1, slot::Slot );
				N_GETVALUE_UNWRAP( tile, 2, map::tile::Tile );
				N_GETVALUE( morale, 3, Int );
				N_GETVALUE( health, 4, Float );
				return m_game->AddEvent( GSE_CALL, new event::SpawnUnit(
					m_game->GetSlotNum(),
					def_name,
					owner->GetIndex(),
					tile->coord.x,
					tile->coord.y,
					GetMorale( GSE_CALL, morale ),
					GetHealth( GSE_CALL, health )
				) );
			})
		},
		{
			"despawn_unit",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 1 );
				N_GETVALUE_UNWRAP( unit, 0, Unit );
				return m_game->AddEvent( GSE_CALL, new event::DespawnUnit( m_game->GetSlotNum(), unit->m_id ) );
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

	Log( "Serializing " + std::to_string( m_units.size() ) + " units" );
	buf.WriteInt( m_units.size() );
	for ( const auto& it : m_units ) {
		buf.WriteString( Unit::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( Unit::GetNextId() );

	Log( "Saved next unit id: " + std::to_string( Unit::GetNextId() ) );
}

void UnitManager::Unserialize( GSE_CALLABLE, types::Buffer& buf ) {
	ASSERT( m_unit_moralesets.empty(), "unit moralesets not empty" );
	ASSERT( m_unit_defs.empty(), "unit defs not empty" );
	ASSERT( m_units.empty(), "units not empty" );
	ASSERT( m_unprocessed_units.empty(), "unprocessed units not empty" );

	size_t sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit moralesets" );
	m_unit_moralesets.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineMoraleSet( MoraleSet::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit defs" );
	m_unit_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineUnit( Def::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " units" );
	if ( !m_game->IsRunning() ) {
		m_unprocessed_units.reserve( sz );
	}
	for ( size_t i = 0 ; i < sz ; i++ ) {
		auto b = types::Buffer( buf.ReadString() );
		SpawnUnit( GSE_CALL, Unit::Unserialize( GSE_CALL, b, this ) );
	}

	Unit::SetNextId( buf.ReadInt() );
	Log( "Restored next unit id: " + std::to_string( Unit::GetNextId() ) );
}

#if defined( DEBUG ) || defined( FASTDEBUG )
const std::string UnitManager::ToString() {
	return "game::UnitManager()";
}
#endif

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
		if ( op & UUO_SPAWN ) {
			// if unit is despawned immediately after spawning - frontend doesn't need to know
			m_unit_updates.erase( it );
			return;
		}
		update.ops = UUO_NONE; // clear other actions if unit was despawned
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

const std::string* UnitManager::MoveUnitValidate( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile ) {
	const auto result = m_game->GetState()->TriggerObject( this, "unit_move_validate", ARGS_F( &ctx, gc_space, &si, &ep, &unit, &dst_tile ) {
		{
			"unit",
			unit->Wrap( GSE_CALL )
		},
		{
			"src_tile",
			unit->GetTile()->Wrap( GSE_CALL )
		},
		{
			"dst_tile",
			dst_tile->Wrap( GSE_CALL )
		},
	}; } );
	switch ( result->type ) {
		case gse::Value::T_NULL:
		case gse::Value::T_UNDEFINED:
			return nullptr; // no errors
		case gse::Value::T_STRING:
			return new std::string( ( (gse::value::String*)result )->value ); // error
		default:
			THROW( "unexpected validation result type: " + result->GetTypeString() );
	}
}

gse::Value* const UnitManager::MoveUnitResolve( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile ) {
	return m_game->GetState()->TriggerObject(this, "unit_move_resolve", ARGS_F( &ctx, gc_space, &si, &ep, &unit, &dst_tile ) {
		{
			"unit",
			unit->Wrap( GSE_CALL )
		},
		{
			"src_tile",
			unit->GetTile()->Wrap( GSE_CALL )
		},
		{
			"dst_tile",
			dst_tile->Wrap( GSE_CALL )
		},
	}; } );
}

void UnitManager::MoveUnitApply( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile, gse::Value* const resolutions ) {
	auto* src_tile = unit->GetTile();
	ASSERT( dst_tile, "dst tile not set" );

	if ( src_tile == dst_tile ) {
		return;
	}

	Log( "Moving unit #" + std::to_string( unit->m_id ) + " to " + dst_tile->coord.ToString() );

	ASSERT( src_tile, "src tile not set" );
	ASSERT( src_tile->units.find( unit->m_id ) != src_tile->units.end(), "src tile does not contain this unit" );
	ASSERT( dst_tile->units.find( unit->m_id ) == dst_tile->units.end(), "dst tile already contains this unit" );

	m_game->GetState()->TriggerObject( this, "unit_move_apply", ARGS_F( &ctx, gc_space, &si, &ep, &unit, &src_tile, &dst_tile, &resolutions ) {
		{
			"unit",
			unit->Wrap( GSE_CALL, true )
		},
		{
			"src_tile",
			src_tile->Wrap( GSE_CALL )
		},
		{
			"dst_tile",
			dst_tile->Wrap( GSE_CALL )
		},
		{
			"resolutions",
			resolutions
		}
	}; } );
}

const std::string* UnitManager::MoveUnitToTile( GSE_CALLABLE, Unit* unit, map::tile::Tile* dst_tile, const cb_oncomplete& on_complete ) {
	const auto* src_tile = unit->GetTile();
	if ( src_tile == dst_tile ) {
		return new std::string( "Unit can't move because it's already on target tile" );
	}
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
	fr.data.unit_move.running_animation_id = m_game->GetAM()->AddAnimationCallback(
		[ on_complete, unit, dst_tile, ctx, gc_space, ep, si]() {
			gse::ExecutionPointer ep2 = ep;
			unit->SetTile( gc_space, ctx, si, ep2 , dst_tile );
			on_complete();
		}
	);
	m_game->AddFrontendRequest( fr );
	return nullptr; // no error
}

const std::string* UnitManager::AttackUnitValidate( GSE_CALLABLE, Unit* attacker, Unit* defender ) {
	const auto result = m_game->GetState()->TriggerObject( this, "unit_attack_validate", ARGS_F( &ctx, gc_space, &si, &ep, &attacker, &defender ) {
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
		case gse::Value::T_NULL:
		case gse::Value::T_UNDEFINED:
			return nullptr; // no errors
		case gse::Value::T_STRING:
			return new std::string( ( (gse::value::String*)result )->value ); // error
		default:
			THROW( "unexpected validation result type: " + result->GetTypeString() );
	}
}

gse::Value* const UnitManager::AttackUnitResolve( GSE_CALLABLE, Unit* attacker, Unit* defender ) {
	return m_game->GetState()->TriggerObject( this, "unit_attack_resolve", ARGS_F( &ctx, gc_space, &si, &ep, &attacker, &defender ) {
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
	state->TriggerObject( this, "unit_attack_apply", ARGS_F( &ctx, gc_space, &si, &ep, &attacker, &defender, &resolutions ) {
		{
			"attacker",
			attacker->Wrap( GSE_CALL, true )
		},
		{
			"defender",
			defender->Wrap( GSE_CALL, true )
		},
		{
			"resolutions",
			resolutions
		}
	}; } );
	if ( attacker->m_health <= 0.0f ) {
		if ( state->IsMaster() ) {
			m_game->AddEvent( GSE_CALL, new event::DespawnUnit( m_game->GetSlotNum(), attacker->m_id ) );
		}
	}
	else {
		RefreshUnit( GSE_CALL, attacker );
	}
	if ( defender->m_health <= 0.0f ) {
		if ( state->IsMaster() ) {
			m_game->AddEvent( GSE_CALL, new event::DespawnUnit( m_game->GetSlotNum(), defender->m_id ) );
		}
	}
	else {
		RefreshUnit( GSE_CALL, defender );
	}
}

void UnitManager::RefreshUnit( GSE_CALLABLE, const Unit* unit ) {
	if ( unit->m_health <= 0.0f ) {
		if ( m_game->GetState()->IsMaster() ) {
			m_game->AddEvent( GSE_CALL, new event::DespawnUnit( m_game->GetSlotNum(), unit->m_id ) );
		}
	}
	else {
		QueueUnitUpdate( unit, UUO_REFRESH );
	}
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
