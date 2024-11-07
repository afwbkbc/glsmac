#include "UnitManager.h"

#include "MoraleSet.h"
#include "Def.h"
#include "Unit.h"

#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/bindings/Bindings.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/event/DespawnUnit.h"

#include "gse/context/Context.h"
#include "gse/callable/Native.h"
#include "gse/type/Bool.h"
#include "gse/type/Float.h"
#include "gse/type/Array.h"

namespace game {
namespace backend {
namespace unit {

UnitManager::UnitManager( Game* game )
	: m_game( game ) {
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

	for ( auto& it : m_unit_moralesets ) {
		delete it.second;
	}
	m_unit_moralesets.clear();

	for ( auto& it : m_units ) {
		delete it.second;
	}
	m_units.clear();

	for ( auto& it : m_unit_defs ) {
		delete it.second;
	}
	m_unit_defs.clear();

	m_unit_updates.clear();
}

void UnitManager::DefineMoraleSet( unit::MoraleSet* moraleset ) {
	Log( "Defining unit moraleset ('" + moraleset->m_id + "')" );

	ASSERT( m_unit_moralesets.find( moraleset->m_id ) == m_unit_moralesets.end(), "Unit moraleset '" + moraleset->m_id + "' already exists" );

	m_unit_moralesets.insert(
		{
			moraleset->m_id,
			moraleset
		}
	);
}

void UnitManager::DefineUnit( unit::Def* def ) {
	Log( "Defining unit ('" + def->m_id + "')" );

	ASSERT( m_unit_defs.find( def->m_id ) == m_unit_defs.end(), "Unit definition '" + def->m_id + "' already exists" );

	m_unit_defs.insert(
		{
			def->m_id,
			def
		}
	);

	auto fr = FrontendRequest( FrontendRequest::FR_UNIT_DEFINE );
	NEW( fr.data.unit_define.serialized_unitdef, std::string, unit::Def::Serialize( def ).ToString() );
	m_game->AddFrontendRequest( fr );
}

void UnitManager::SpawnUnit( Unit* unit ) {
	if ( !m_game->IsRunning() ) {
		m_unprocessed_units.push_back( unit );
		return;
	}

	auto* tile = unit->GetTile();

	Log( "Spawning unit #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + tile->ToString() );

	ASSERT( m_units.find( unit->m_id ) == m_units.end(), "duplicate unit id" );
	m_units.insert_or_assign( unit->m_id, unit );

	QueueUnitUpdate( unit, UUO_SPAWN );

	auto* state = m_game->GetState();
	if ( state->IsMaster() ) {
		state->m_bindings->Call(
			bindings::Bindings::CS_ON_UNIT_SPAWN, {
				{
					"unit",
					unit->Wrap()
				},
			}
		);
	}
}

void UnitManager::DespawnUnit( const size_t unit_id ) {

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
		state->m_bindings->Call(
			bindings::Bindings::CS_ON_UNIT_DESPAWN, {
				{
					"unit",
					unit->Wrap()
				}
			}
		);
	}

	delete unit;
}

void UnitManager::SkipUnitTurn( const size_t unit_id ) {
	const auto& it = m_units.find( unit_id );
	ASSERT( it != m_units.end(), "unit id not found" );
	auto* unit = it->second;

	Log( "Skipping unit turn #" + std::to_string( unit->m_id ) + " (" + unit->m_def->m_id + ") at " + unit->GetTile()->ToString() );

	unit->m_movement = 0.0f;

	RefreshUnit( unit );
}

unit::MoraleSet* UnitManager::GetMoraleSet( const std::string& name ) const {
	const auto& it = m_unit_moralesets.find( name );
	if ( it != m_unit_moralesets.end() ) {
		return it->second;
	}
	return nullptr;
}

unit::Unit* UnitManager::GetUnit( const size_t id ) const {
	const auto& it = m_units.find( id );
	if ( it != m_units.end() ) {
		return it->second;
	}
	return nullptr;
}

unit::Def* UnitManager::GetUnitDef( const std::string& name ) const {
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

void UnitManager::ProcessUnprocessed() {
	ASSERT( m_game->IsRunning(), "game not running" );
	for ( auto& it : m_unprocessed_units ) {
		SpawnUnit( it );
	}
	m_unprocessed_units.clear();
}

void UnitManager::PushUnitUpdates() {
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

WRAPIMPL_BEGIN( UnitManager, CLASS_UNITS )
	WRAPIMPL_PROPS
		{
			"lock_tiles",
				NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( tiles, 0, Array );
				N_PERSIST_CALLABLE( on_complete, 1 );
				map::tile::positions_t tile_positions = {};
				tile_positions.reserve( tiles.size() );
				for ( const auto& tileobj : tiles ) {
					N_UNWRAP( tile, tileobj, map::tile::Tile );
					tile_positions.push_back( tile->coord );
				}
				m_game->SendTileLockRequest( tile_positions, [ this, on_complete, tile_positions, ctx, call_si ]() {
					on_complete->Run( ctx, call_si, {
						VALUE( gse::callable::Native, [ this, tile_positions ](
							gse::context::Context* ctx,
							const gse::si_t& call_si,
							const gse::type::function_arguments_t& arguments
						) -> gse::Value {
							m_game->SendTileUnlockRequest( tile_positions );
							return VALUE( gse::type::Undefined );
						}),
					});
					N_UNPERSIST_CALLABLE( on_complete );
				});
				return VALUE( gse::type::Undefined );
			})

		},
/*		{
			"add",
			NATIVE_CALL( this ) {
				N_EXPECT_ARGS( 2 );
				N_GETVALUE( id, 0, String );

				N_GETVALUE( faction_def, 1, Object );
				N_GETPROP( name, faction_def, "name", String );

				auto* faction = new Faction( id, name );

				N_GETPROP( colors, faction_def, "colors", Object );
				N_GETPROP_UNWRAP( colors_text, colors, "text", types::Color );
				N_GETPROP_UNWRAP( colors_text_shadow, colors, "text_shadow", types::Color );
				N_GETPROP_UNWRAP( colors_border, colors, "border", types::Color );
				faction->m_colors = {
					colors_text,
					colors_text_shadow,
					colors_border
				};

				N_GETPROP_OPT_BOOL( is_naval, faction_def, "is_naval")
				if ( is_naval ) {
					faction->m_flags |= Faction::FF_NAVAL;
				}
				N_GETPROP_OPT_BOOL( is_progenitor, faction_def, "is_progenitor")
				if ( is_progenitor ) {
					faction->m_flags |= Faction::FF_PROGENITOR;
				}

				N_GETPROP( bases_def, faction_def, "bases", Object );
				N_GETPROP( bases_render_def, bases_def, "render", Object );

				N_GETPROP( bases_render_type, bases_render_def, "type", String );
				if ( bases_render_type == "sprite_grid" ) {
					N_GETPROP( file, bases_render_def, "file", String );
					N_GETPROP( grid_x, bases_render_def, "grid_x", Int );
					N_GETPROP( grid_y, bases_render_def, "grid_y", Int );
					N_GETPROP( cell_width, bases_render_def, "cell_width", Int );
					N_GETPROP( cell_height, bases_render_def, "cell_height", Int );
					N_GETPROP_OPT( size_t, cell_cx, bases_render_def, "cell_cx", Int, cell_width / 2 );
					N_GETPROP_OPT( size_t, cell_cy, bases_render_def, "cell_cy", Int, cell_height / 2 );
					N_GETPROP( cell_padding, bases_render_def, "cell_padding", Int );
					N_GETPROP_OPT( float, scale_x, bases_render_def, "scale_x", Float, 1.0f );
					N_GETPROP_OPT( float, scale_y, bases_render_def, "scale_y", Float, 1.0f );
					faction->m_bases_render = {
						file,
						(size_t)grid_x,
						(size_t)grid_y,
						(size_t)cell_width,
						(size_t)cell_height,
						cell_cx,
						cell_cy,
						(size_t)cell_padding,
						scale_x,
						scale_y
					};
				}
				else {
					delete faction;
					GSE_ERROR( gse::EC.GAME_ERROR, "Unsupported bases render type: " + bases_render_type );
				}

				N_GETPROP( base_names, bases_def, "names", Object );
				N_GETPROP( base_names_land, base_names, "land", Array );
				faction->m_base_names.land.reserve( base_names_land.size() );
				for ( size_t i = 0  ; i < base_names_land.size() ; i++ ) {
					N_GETELEMENT( v, base_names_land, i, String );
					faction->m_base_names.land.push_back( v );
				}

				N_GETPROP( base_names_water, base_names, "water", Array );
				faction->m_base_names.water.reserve( base_names_water.size() );
				for ( size_t i = 0  ; i < base_names_water.size() ; i++ ) {
					N_GETELEMENT( v, base_names_water, i, String );
					faction->m_base_names.water.push_back( v );
				}

				Add( faction );
				return faction->Wrap();
			} )
		},*/
	};
WRAPIMPL_END_PTR( UnitManager )

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
		buf.WriteInt( it.first );
		buf.WriteString( Unit::Serialize( it.second ).ToString() );
	}
	buf.WriteInt( Unit::GetNextId() );

	Log( "Saved next unit id: " + std::to_string( Unit::GetNextId() ) );
}

void UnitManager::Unserialize( types::Buffer& buf ) {
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
		DefineMoraleSet( unit::MoraleSet::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " unit defs" );
	m_unit_defs.reserve( sz );
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto name = buf.ReadString();
		auto b = types::Buffer( buf.ReadString() );
		DefineUnit( unit::Def::Unserialize( b ) );
	}

	sz = buf.ReadInt();
	Log( "Unserializing " + std::to_string( sz ) + " units" );
	if ( !m_game->IsRunning() ) {
		m_unprocessed_units.reserve( sz );
	}
	for ( size_t i = 0 ; i < sz ; i++ ) {
		const auto unit_id = buf.ReadInt();
		auto b = types::Buffer( buf.ReadString() );
		SpawnUnit( Unit::Unserialize( b, this ) );
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

const std::string* UnitManager::MoveUnitValidate( Unit* unit, map::tile::Tile* dst_tile ) {
	const auto result = m_game->GetState()->m_bindings->Trigger( this, "unit_move_validate", {
		{
			"unit",
			unit->Wrap()
		},
		{
			"src_tile",
			unit->GetTile()->Wrap()
		},
		{
			"dst_tile",
			dst_tile->Wrap()
		},
	});
	switch ( result.Get()->type ) {
		case gse::type::Type::T_NULL:
		case gse::type::Type::T_UNDEFINED:
			return nullptr; // no errors
		case gse::type::Type::T_STRING:
			return new std::string( ( (gse::type::String*)result.Get() )->value ); // error
		default:
			THROW( "unexpected validation result type: " + gse::type::Type::GetTypeString( result.Get()->type ) );
	}
}

const gse::Value UnitManager::MoveUnitResolve( Unit* unit, map::tile::Tile* dst_tile ) {
	return m_game->GetState()->m_bindings->Trigger(this, "unit_move_resolve", {
		{
			"unit",
			unit->Wrap()
		},
		{
			"src_tile",
			unit->GetTile()->Wrap()
		},
		{
			"dst_tile",
			dst_tile->Wrap()
		},
	});
}

void UnitManager::MoveUnitApply( Unit* unit, map::tile::Tile* dst_tile, const gse::Value resolutions ) {
	auto* src_tile = unit->GetTile();
	ASSERT( dst_tile, "dst tile not set" );

	if ( src_tile == dst_tile ) {
		return;
	}

	Log( "Moving unit #" + std::to_string( unit->m_id ) + " to " + dst_tile->coord.ToString() );

	ASSERT( src_tile, "src tile not set" );
	ASSERT( src_tile->units.find( unit->m_id ) != src_tile->units.end(), "src tile does not contain this unit" );
	ASSERT( dst_tile->units.find( unit->m_id ) == dst_tile->units.end(), "dst tile already contains this unit" );

	m_game->GetState()->m_bindings->Trigger( this, "unit_move_apply", {
		{
			"unit",
			unit->Wrap( true )
		},
		{
			"src_tile",
			src_tile->Wrap()
		},
		{
			"dst_tile",
			dst_tile->Wrap()
		},
		{
			"resolutions",
			resolutions
		}
	});
}

const std::string* UnitManager::MoveUnitToTile( Unit* unit, map::tile::Tile* dst_tile, const cb_oncomplete& on_complete ) {
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
	fr.data.unit_move.running_animation_id = m_game->AddAnimationCallback(
		[ on_complete, unit, dst_tile ]() {
			unit->SetTile( dst_tile );
			on_complete();
		}
	);
	m_game->AddFrontendRequest( fr );
	return nullptr; // no error
}

const std::string* UnitManager::AttackUnitValidate( Unit* attacker, Unit* defender ) {
	const auto result = m_game->GetState()->m_bindings->Trigger( this, "unit_attack_validate", {
		{
			"attacker",
			attacker->Wrap()
		},
		{
			"defender",
			defender->Wrap()
		},
	});
	switch ( result.Get()->type ) {
		case gse::type::Type::T_NULL:
		case gse::type::Type::T_UNDEFINED:
			return nullptr; // no errors
		case gse::type::Type::T_STRING:
			return new std::string( ( (gse::type::String*)result.Get() )->value ); // error
		default:
			THROW( "unexpected validation result type: " + gse::type::Type::GetTypeString( result.Get()->type ) );
	}
}

const gse::Value UnitManager::AttackUnitResolve( Unit* attacker, Unit* defender ) {
	return m_game->GetState()->m_bindings->Trigger( this, "unit_attack_resolve", {
		{
			"attacker",
			attacker->Wrap()
		},
		{
			"defender",
			defender->Wrap()
		},
	});
}

void UnitManager::AttackUnitApply( Unit* attacker, Unit* defender, const gse::Value resolutions ) {
	auto* state = m_game->GetState();
	state->m_bindings->Trigger( this, "unit_attack_apply",{
		{
			"attacker",
			attacker->Wrap( true )
		},
		{
			"defender",
			defender->Wrap( true )
		},
		{
			"resolutions",
			resolutions
		}
	});
	if ( attacker->m_health <= 0.0f ) {
		if ( state->IsMaster() ) {
			m_game->AddEvent( new event::DespawnUnit( m_game->GetSlotNum(), attacker->m_id ) );
		}
	}
	else {
		RefreshUnit( attacker );
	}
	if ( defender->m_health <= 0.0f ) {
		if ( state->IsMaster() ) {
			m_game->AddEvent( new event::DespawnUnit( m_game->GetSlotNum(), defender->m_id ) );
		}
	}
	else {
		RefreshUnit( defender );
	}
}

void UnitManager::RefreshUnit( const unit::Unit* unit ) {
	if ( unit->m_health <= 0.0f ) {
		if ( m_game->GetState()->IsMaster() ) {
			m_game->AddEvent( new event::DespawnUnit( m_game->GetSlotNum(), unit->m_id ) );
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
