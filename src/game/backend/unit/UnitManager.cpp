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
#include "game/backend/unit/SpriteRender.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/value/Bool.h"
#include "gse/value/Float.h"
#include "gse/value/Array.h"
#include "game/backend/map/tile/TileManager.h"

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
				unit->Wrap( GSE_CALL, m_game->GetGCSpace() )
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
					if ( v->type != gse::Value::T_OBJECT ) {
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
			"define_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );
				N_GETVALUE( unit_def, 1, Object );
				N_GETPROP( name, unit_def, "name", String );
				N_GETPROP( morale, unit_def, "morale", String );
				N_GETPROP( unit_type, unit_def, "type", String );

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
					return it->second->Wrap( GSE_CALL, true );
				}
				else {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit id " + std::to_string( unit_id ) + " not found" );
				}
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
				N_GETPROP_UNWRAP( owner, obj, "owner", slot::Slot );
				N_GETPROP_UNWRAP( tile, obj, "tile", map::tile::Tile );
				N_GETPROP( morale, obj, "morale", Int );
				N_GETPROP( health, obj, "health", Float );

				auto* def = GetUnitDef( def_name );
				if ( !def ) {
					GSE_ERROR( gse::EC.GAME_ERROR, "Unit type '" + def_name + "' is not defined" );
				}
				ASSERT_NOLOG( def->m_type == unit::DT_STATIC, "only static defs are supported for now" );
				const auto* staticdef = (unit::StaticDef*)def;
				auto* unit = new unit::Unit(
					GSE_CALL,
					this,
					unit_id ? unit_id : unit::Unit::GetNextId(),
					def,
					owner,
					tile,
					staticdef->m_movement_per_turn,
					morale,
					health,
					false
				);
				SpawnUnit( GSE_CALL, unit );
				return unit->Wrap( GSE_CALL, true );
			})
		},
		{
			"despawn_unit",
			NATIVE_CALL( this ) {

				m_game->CheckRW( GSE_CALL );

				N_EXPECT_ARGS( 1 );
				if ( arguments.at( 0 )->type == gse::Value::T_INT ) {
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
		[ this, tm, tiles_to_lock, on_complete, unit, dst_tile, ctx, gc_space, ep, si ]( const size_t animation_id ) {
			ASSERT_NOLOG( unit->m_animation_id, "animation id gone" );
			unit->m_animation_id = 0;
			tm->UnlockTiles( m_game->GetSlotNum(), tiles_to_lock );
			gse::ExecutionPointer ep2 = ep;
			unit->SetTile( gc_space, ctx, si, ep2 , dst_tile );
			on_complete();
		}
	);
	m_game->AddFrontendRequest( fr );

	return nullptr; // no error
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
