#include "Base.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <unordered_set>

#include "gse/context/Context.h"
#include "gse/value/Object.h"
#include "gse/value/Int.h"
#include "gse/value/String.h"
#include "gse/value/Undefined.h"
#include "gse/value/Array.h"
#include "gse/value/Bool.h"
#include "gse/callable/Native.h"
#include "game/backend/Game.h"
#include "game/backend/State.h"
#include "game/backend/Player.h"
#include "game/backend/slot/Slot.h"
#include "game/backend/slot/Slots.h"
#include "game/backend/map/Map.h"
#include "game/backend/map/tile/Tile.h"
#include "game/backend/faction/Faction.h"
#include "game/backend/base/BaseManager.h"
#include "FacilityDef.h"
#include "Pop.h"
#include "PopDef.h"
#include "game/backend/Random.h"
#include "game/backend/resource/ResourceManager.h"
#include "game/backend/unit/Def.h"
#include "game/backend/unit/UnitManager.h"

namespace game {
namespace backend {
namespace base {

static constexpr size_t MAX_SERIALIZED_POPS = 1024;

static size_t next_id = 1;
const size_t Base::GetNextId() {
	return next_id;
}
const void Base::SetNextId( const size_t id ) {
	next_id = id;
}

Base::Base(
	Game* game,
	const size_t id,
	slot::Slot* owner,
	faction::Faction* faction,
	map::tile::Tile* tile,
	const std::string& name,
	const pops_t& pops,
	const size_t next_pop_id,
	const production_queue_t& production_queue,
	const int64_t accumulated_minerals,
	const facilities_t& facilities
)
	: MapObject( game->GetMap(), tile )
	, m_game( game )
	, m_id( id )
	, m_owner( owner )
	, m_faction( faction )
	, m_name( name )
	, m_pops( pops )
	, m_production_queue( production_queue )
	, m_accumulated_minerals( accumulated_minerals )
	, m_facilities( facilities )
	, m_next_pop_id( next_pop_id ) {
	if ( m_accumulated_minerals < 0 || m_accumulated_minerals > MAX_ACCUMULATED_MINERALS ) {
		THROW( "invalid base accumulated mineral count" );
	}
	std::string validation_error;
	if ( !ValidateFacilities( m_facilities, validation_error ) ) {
		THROW( validation_error );
	}
	if ( !ValidateProductionQueue( m_production_queue, validation_error ) ) {
		THROW( validation_error );
	}
	if ( next_id <= id ) {
		next_id = id + 1;
	}
	ASSERT( !tile->base, "tile already has base" );
	tile->base = this;
	m_tile = tile;
	for ( auto& it : m_pops ) {
		it.second.SetBase( this );
	}
}

const  Game* const Base::GetGame() const {
	return m_game;
}

Pop* const Base::AddPop( const Pop& pop ) {
	ASSERT( m_pops.find( pop.m_id ) == m_pops.end(), "pop already exists" );
	m_pops.insert_or_assign( pop.m_id, pop );
	m_game->GetBM()->RefreshBase( this );
	TriggerUpdate();
	return &m_pops.at( pop.m_id );
}

void Base::SetOwner( GSE_CALLABLE, Player* owner ) {
	if ( !owner || !owner->GetSlot() ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "base owner has no player slot" );
	}
	auto* const owner_slot = owner->GetSlot();
	auto* const slots = m_game->GetState()->m_slots;
	if (
		owner_slot->GetIndex() >= slots->GetCount() ||
		&slots->GetSlot( owner_slot->GetIndex() ) != owner_slot ||
		owner_slot->GetState() != slot::Slot::SS_PLAYER ||
		owner_slot->GetPlayer() != owner ||
		!owner->GetFaction()
	) {
		GSE_ERROR( gse::EC.INVALID_CALL, "base owner is not an active player in this game" );
	}
	if ( m_owner != owner_slot ) {
		m_owner = owner_slot;
		m_game->GetBM()->RefreshBase( this );
		TriggerUpdate();
	}
}

void Base::RemovePop( GSE_CALLABLE, const size_t pop_id ) {
	const auto it = m_pops.find( pop_id );
	if ( it == m_pops.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population does not belong to this base" );
	}
	if ( it->second.m_worked_tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population must stop working before it can be removed" );
	}
	m_pops.erase( it );
	m_game->GetBM()->RefreshBase( this );
	TriggerUpdate();
}

void Base::ChangePopType( GSE_CALLABLE, const size_t pop_id, const std::string& def_id ) {
	ASSERT( m_pops.find( pop_id ) != m_pops.end(), "pop id " + std::to_string( pop_id ) + " not found" );
	auto& pop = m_pops.at( pop_id );
	const auto& defs = m_game->GetBM()->GetBasePopDefs();
	const auto& it = defs.find( def_id );
	if ( it == defs.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "Unknown pop type: " + def_id );
	}
	if ( pop.m_def != it->second ) {
		pop.m_def = it->second;
		TriggerUpdate();
	}
}

void Base::WorkPopTile( GSE_CALLABLE, Pop* const pop, map::tile::Tile* const tile ) {
	if ( !pop || pop->m_base != this ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population does not belong to this base" );
	}
	if ( !tile ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "worked tile is null" );
	}
	if ( pop->m_worked_tile && pop->m_worked_tile != tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population already works a tile" );
	}
	if ( m_worked_tiles.find( tile ) != m_worked_tiles.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "tile is already worked by this base" );
	}
	tile->SetWorkingPop( GSE_CALL, pop );
	pop->SetWorkedTile( GSE_CALL, tile );
	m_worked_tiles.insert( tile );
	TriggerUpdate();
}

void Base::UnworkPopTile( GSE_CALLABLE, Pop* const pop, map::tile::Tile* const tile ) {
	if ( !pop || pop->m_base != this ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population does not belong to this base" );
	}
	if ( !tile || pop->m_worked_tile != tile ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "population does not work this tile" );
	}
	const auto it = m_worked_tiles.find( tile );
	if ( it == m_worked_tiles.end() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "tile is not worked by this base" );
	}
	tile->UnsetWorkingPop( GSE_CALL, pop );
	pop->UnsetWorkedTile( GSE_CALL, tile );
	m_worked_tiles.erase( it );
	TriggerUpdate();
}

const Base::production_t* Base::GetProduction() const {
	return m_production_queue.empty()
		? nullptr
		: &m_production_queue.front();
}

gse::Wrappable* Base::GetProductionDef( const production_t& production ) const {
	switch ( production.kind ) {
		case PK_UNIT:
			return m_game->GetUM()->GetUnitDef( production.id );
		case PK_FACILITY:
		case PK_PROJECT:
			return m_game->GetBM()->GetFacilityDef( production.id );
	}
	return nullptr;
}

bool Base::CanProduceUnit( const unit::Def* def ) const {
	if ( !def || !def->m_buildable || def->m_mineral_cost <= 0 ) {
		return false;
	}
	const auto* const owner = m_owner ? m_owner->GetPlayer() : nullptr;
	if (
		!def->m_required_technology.empty() &&
		( !owner || !owner->HasTechnology( def->m_required_technology ) )
	) {
		return false;
	}
	switch ( def->GetMovementType() ) {
		case unit::MT_LAND:
			return !m_tile->is_water_tile;
		case unit::MT_WATER: {
			if ( m_tile->is_water_tile ) {
				return true;
			}
			for ( const auto* const neighbour : m_tile->neighbours ) {
				if ( neighbour->is_water_tile ) {
					return true;
				}
			}
			return false;
		}
		case unit::MT_AIR:
			return true;
		case unit::MT_IMMOVABLE:
			return false;
	}
	return false;
}

bool Base::CanProduce( const production_t& production ) const {
	if ( production.id.empty() ) {
		return false;
	}
	switch ( production.kind ) {
		case PK_UNIT:
			return CanProduceUnit( m_game->GetUM()->GetUnitDef( production.id ) );
		case PK_FACILITY:
		case PK_PROJECT: {
			auto* const def = m_game->GetBM()->GetFacilityDef( production.id );
			const auto* const owner = m_owner ? m_owner->GetPlayer() : nullptr;
			const bool is_conversion = def && def->m_mineral_to_energy_divisor > 0;
			const bool is_orbital = def && (
				!def->m_orbital_resource.empty() || def->m_orbital_defense
			);
			const auto* const space_elevator = is_orbital
				? m_game->GetBM()->GetProjectBase( "TheSpaceElevator" )
				: nullptr;
			const bool has_orbital_access =
				is_orbital && space_elevator && space_elevator->m_owner == m_owner;
			return
				def &&
				def->m_is_project == ( production.kind == PK_PROJECT ) &&
				( def->m_mineral_cost > 0 || is_conversion ) &&
				( is_conversion || is_orbital || !HasFacility( production.id ) ) &&
				( !def->m_is_project || !m_game->GetBM()->GetProjectBase( production.id ) ) &&
				(
					def->m_required_facility.empty() ||
					HasFacility( def->m_required_facility ) ||
					has_orbital_access
				) &&
				(
					def->m_required_project.empty() ||
					m_game->GetBM()->GetProjectBase( def->m_required_project )
				) &&
				(
					def->m_required_technology.empty() ||
					( owner && owner->HasTechnology( def->m_required_technology ) )
				);
		}
	}
	return false;
}

bool Base::CanQueueProduction( const production_t& production ) const {
	if ( production.kind == PK_FACILITY ) {
		const auto* const def = m_game->GetBM()->GetFacilityDef( production.id );
		if ( def && def->m_mineral_to_energy_divisor > 0 ) {
			return false;
		}
	}
	production_queue_t queue = m_production_queue;
	queue.push_back( production );
	std::string error;
	return ValidateProductionQueue( queue, error );
}

bool Base::CanSetProduction( const production_t& production ) const {
	production_queue_t queue = m_production_queue;
	if ( queue.empty() ) {
		queue.push_back( production );
	}
	else {
		queue.front() = production;
	}
	std::string error;
	return ValidateProductionQueue( queue, error );
}

void Base::SetProduction( GSE_CALLABLE, const production_t& production ) {
	production_queue_t queue = m_production_queue;
	if ( queue.empty() ) {
		queue.push_back( production );
	}
	else {
		queue.front() = production;
	}
	SetProductionQueue( GSE_CALL, queue );
}

void Base::EnqueueProduction( GSE_CALLABLE, const production_t& production ) {
	production_queue_t queue = m_production_queue;
	queue.push_back( production );
	SetProductionQueue( GSE_CALL, queue );
}

void Base::RemoveProduction( GSE_CALLABLE, const size_t index ) {
	if ( index >= m_production_queue.size() ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Production queue index is out of bounds" );
	}
	m_production_queue.erase( m_production_queue.begin() + index );
	TriggerUpdate();
}

void Base::SetProductionQueue( GSE_CALLABLE, const production_queue_t& production_queue ) {
	std::string error;
	if ( !ValidateProductionQueue( production_queue, error ) ) {
		GSE_ERROR( gse::EC.INVALID_CALL, error );
	}
	if ( m_production_queue != production_queue ) {
		m_production_queue = production_queue;
		TriggerUpdate();
	}
}

void Base::ClearProduction() {
	if ( !m_production_queue.empty() ) {
		m_production_queue.clear();
		TriggerUpdate();
	}
}

bool Base::HasFacility( const std::string& id ) const {
	return m_facilities.find( id ) != m_facilities.end();
}

void Base::AddFacility( GSE_CALLABLE, const std::string& id ) {
	const auto* const def = m_game->GetBM()->GetFacilityDef( id );
	if ( !def ) {
		GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown base facility: " + id );
	}
	if (
		def->m_mineral_to_energy_divisor > 0 ||
		!def->m_orbital_resource.empty() ||
		def->m_orbital_defense
	) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Repeatable production cannot be added as a base facility: " + id );
	}
	if ( def->m_is_project && m_game->GetBM()->GetProjectBase( id ) ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Secret project is already complete: " + id );
	}
	if ( !m_facilities.insert( id ).second ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Base already has facility: " + id );
	}
	TriggerUpdate();
}

void Base::RemoveFacility( GSE_CALLABLE, const std::string& id ) {
	if ( m_facilities.erase( id ) == 0 ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Base does not have facility: " + id );
	}
	TriggerUpdate();
}

bool Base::ParseProductionKind( const std::string& value, production_kind_t& result ) {
	if ( value == "unit" ) {
		result = PK_UNIT;
		return true;
	}
	if ( value == "facility" ) {
		result = PK_FACILITY;
		return true;
	}
	if ( value == "project" ) {
		result = PK_PROJECT;
		return true;
	}
	return false;
}

const std::string Base::GetProductionKindString( const production_kind_t kind ) {
	switch ( kind ) {
		case PK_UNIT:
			return "unit";
		case PK_FACILITY:
			return "facility";
		case PK_PROJECT:
			return "project";
	}
	return "unknown";
}

void Base::SetAccumulatedMinerals( GSE_CALLABLE, const int64_t minerals ) {
	if ( minerals < 0 || minerals > MAX_ACCUMULATED_MINERALS ) {
		GSE_ERROR( gse::EC.INVALID_CALL, "Invalid accumulated mineral count: " + std::to_string( minerals ) );
	}
	if ( m_accumulated_minerals != minerals ) {
		m_accumulated_minerals = minerals;
		TriggerUpdate();
	}
}

const types::Buffer Base::Serialize( const Base* base ) {
	types::Buffer buf;
	std::unordered_set< map::tile::Tile* > pop_worked_tiles = {};
	for ( const auto& it : base->m_pops ) {
		const auto& pop = it.second;
		if ( pop.m_worked_tile ) {
			if (
				pop.GetWorkedTileLink() != pop.m_worked_tile ||
				pop.m_worked_tile->GetWorkingPop() != &pop
			) {
				THROW( "base population worker links do not match its assignment" );
			}
			if ( !pop_worked_tiles.insert( pop.m_worked_tile ).second ) {
				THROW( "multiple populations work the same tile" );
			}
		}
		else if ( pop.HasWorkedTileLink() ) {
			THROW( "unassigned base population has a worked tile link" );
		}
	}
	if ( pop_worked_tiles != base->m_worked_tiles ) {
		THROW( "base worked tiles do not match population assignments" );
	}
	buf.WriteInt( base->m_id );
	buf.WriteInt( base->m_owner->GetIndex() );
	buf.WriteString( base->m_faction->m_id );
	buf.WriteInt( base->m_tile->coord.x );
	buf.WriteInt( base->m_tile->coord.y );
	buf.WriteString( base->m_name );
	buf.WriteInt( base->m_pops.size() );
	for ( const auto& it : base->m_pops ) {
		buf.WriteInt( it.first );
		it.second.Serialize( buf );
	}
	buf.WriteInt( base->m_next_pop_id );
	auto* const accumulated_nutrients = const_cast< Base* >( base )->CustomGet( "accumulated_nutrients" );
	buf.WriteBool( accumulated_nutrients != nullptr );
	if ( accumulated_nutrients ) {
		if ( accumulated_nutrients->type != gse::VT_INT ) {
			THROW( "base accumulated nutrients must be an integer" );
		}
		buf.WriteInt( ( (gse::value::Int*)accumulated_nutrients )->value );
	}
	if ( base->m_accumulated_minerals < 0 || base->m_accumulated_minerals > MAX_ACCUMULATED_MINERALS ) {
		THROW( "invalid base accumulated mineral count" );
	}
	std::string validation_error;
	if ( !base->ValidateProductionQueue( base->m_production_queue, validation_error ) ) {
		THROW( validation_error );
	}
	if ( !base->ValidateFacilities( base->m_facilities, validation_error ) ) {
		THROW( validation_error );
	}
	buf.WriteInt( base->m_production_queue.size() );
	for ( const auto& production : base->m_production_queue ) {
		buf.WriteInt( production.kind );
		buf.WriteString( production.id );
	}
	buf.WriteInt( base->m_accumulated_minerals );
	std::vector< std::string > facility_ids( base->m_facilities.begin(), base->m_facilities.end() );
	std::sort( facility_ids.begin(), facility_ids.end() );
	buf.WriteInt( facility_ids.size() );
	for ( const auto& id : facility_ids ) {
		buf.WriteString( id );
	}
	auto* const network_node_artifact_linked =
		const_cast< Base* >( base )->CustomGet( "network_node_artifact_linked" );
	if ( network_node_artifact_linked && network_node_artifact_linked->type != gse::VT_BOOL ) {
		THROW( "base Network Node artifact state must be a boolean" );
	}
	buf.WriteBool(
		network_node_artifact_linked &&
		( (gse::value::Bool*)network_node_artifact_linked )->value
	);
	auto get_economic_victory_value = [base]( const std::string& key ) {
		auto* const value = const_cast< Base* >( base )->CustomGet( key );
		if ( value && value->type != gse::VT_INT ) {
			THROW( "base economic victory state must contain integers" );
		}
		return value ? ( (gse::value::Int*)value )->value : int64_t{ 0 };
	};
	const auto economic_victory_turn = get_economic_victory_value( "economic_victory_turn" );
	const auto economic_victory_cost = get_economic_victory_value( "economic_victory_cost" );
	if (
		economic_victory_turn < 0 || economic_victory_cost < 0 ||
		economic_victory_cost > Player::MAX_ENERGY_CREDITS ||
		( economic_victory_turn == 0 ) != ( economic_victory_cost == 0 )
	) {
		THROW( "invalid base economic victory state" );
	}
	buf.WriteInt( economic_victory_turn );
	buf.WriteInt( economic_victory_cost );
	return buf;
}

Base* Base::Deserialize( GSE_CALLABLE, types::Buffer& buf, Game* game ) {
	if ( !game ) {
		THROW( "cannot deserialize base without a game" );
	}
	const auto id = buf.ReadInt< size_t >( "base id" );
	if ( id == 0 ) {
		THROW( "serialized base id is zero" );
	}
	const auto slot_num = buf.ReadInt< size_t >( "base owner slot" );
	auto* const slots = game->GetState()->m_slots;
	if ( slot_num >= slots->GetCount() ) {
		THROW( "serialized base owner slot is out of bounds" );
	}
	auto* slot = &slots->GetSlot( slot_num );
	if ( slot->GetState() != slot::Slot::SS_PLAYER || !slot->GetPlayer() ) {
		THROW( "serialized base owner slot has no player" );
	}
	const auto faction_id = buf.ReadString();
	auto* faction = game->GetFaction( faction_id );
	auto* const owner_faction = slot->GetPlayer()->GetFaction();
	if ( !faction ) {
		THROW( "serialized base faction does not exist: " + faction_id );
	}
	if ( !owner_faction ) {
		THROW( "serialized base owner has no faction" );
	}
	const auto pos_x = buf.ReadInt< size_t >( "base tile x" );
	const auto pos_y = buf.ReadInt< size_t >( "base tile y" );
	if (
		pos_x >= game->GetMap()->GetWidth() ||
		pos_y >= game->GetMap()->GetHeight() ||
		pos_x % 2 != pos_y % 2
	) {
		THROW( "invalid serialized base tile" );
	}
	auto* tile = game->GetMap()->GetTile( pos_x, pos_y );
	if ( tile->base ) {
		THROW( "serialized base tile already has a base" );
	}
	const auto name = buf.ReadString();
	pops_t pops = {};
	const auto pops_count = buf.ReadCollectionSize( "base population" );
	if ( pops_count > MAX_SERIALIZED_POPS ) {
		THROW( "invalid serialized base population count: " + std::to_string( pops_count ) );
	}
	size_t max_pop_id = 0;
	std::unordered_set< map::tile::Tile* > worked_tiles = {};
	for ( size_t i = 0 ; i < pops_count ; i++ ) {
		const auto pop_id = buf.ReadInt< size_t >( "base population id" );
		if ( pop_id == 0 ) {
			THROW( "serialized base population id is zero" );
		}
		Pop pop = {};
		pop.Deserialize( buf, game );
		if ( pop.m_id != pop_id ) {
			THROW( "serialized base population id mismatch" );
		}
		if ( !pops.emplace( pop.m_id, pop ).second ) {
			THROW( "duplicate serialized base population id: " + std::to_string( pop.m_id ) );
		}
		const auto& renders = ( faction->m_flags & faction::Faction::FF_PROGENITOR )
			? pop.m_def->m_renders_progenitor
			: pop.m_def->m_renders_human;
		if ( pop.m_variant >= renders.size() ) {
			THROW( "serialized base population variant is unavailable" );
		}
		if ( pop.m_worked_tile ) {
			if (
				!worked_tiles.insert( pop.m_worked_tile ).second ||
				pop.m_worked_tile->HasWorkingPopLink() ||
				pop.HasWorkedTileLink()
			) {
				THROW( "invalid serialized base population worked tile" );
			}
		}
		max_pop_id = std::max( max_pop_id, pop_id );
	}
	const auto next_pop_id = buf.ReadInt< size_t >( "next base population id" );
	if ( next_pop_id == 0 || next_pop_id <= max_pop_id ) {
		THROW( "invalid serialized next base population id" );
	}
	const bool has_accumulated_nutrients = buf.ReadBool();
	const auto accumulated_nutrients = has_accumulated_nutrients ? buf.ReadInt() : 0;
	production_queue_t production_queue = {};
	const auto production_count = buf.ReadCollectionSize( "base production queue" );
	if ( production_count > MAX_PRODUCTION_QUEUE_SIZE ) {
		THROW( "invalid serialized base production queue size" );
	}
	production_queue.reserve( production_count );
	for ( size_t i = 0 ; i < production_count ; i++ ) {
		const auto serialized_kind = buf.ReadInt();
		if ( serialized_kind < PK_UNIT || serialized_kind > PK_PROJECT ) {
			THROW( "invalid serialized base production kind" );
		}
		const auto production_id = buf.ReadString();
		if ( production_id.empty() ) {
			THROW( "serialized base production id is empty" );
		}
		production_queue.push_back( {
			static_cast< production_kind_t >( serialized_kind ),
			production_id,
		} );
	}
	const auto accumulated_minerals = buf.ReadInt< int64_t >( "base accumulated minerals" );
	if ( accumulated_minerals < 0 || accumulated_minerals > MAX_ACCUMULATED_MINERALS ) {
		THROW( "invalid serialized base accumulated mineral count" );
	}
	facilities_t facilities = {};
	const auto facility_count = buf.ReadCollectionSize( "base built facility" );
	if ( facility_count > game->GetBM()->GetFacilityDefs().size() ) {
		THROW( "invalid serialized base facility count" );
	}
	for ( size_t i = 0 ; i < facility_count ; i++ ) {
		const auto facility_id = buf.ReadString();
		if ( facility_id.empty() || !facilities.insert( facility_id ).second ) {
			THROW( "invalid or duplicate serialized base facility: " + facility_id );
		}
	}
	const bool network_node_artifact_linked = buf.GetRemaining() > 0
		? buf.ReadBool()
		: false;
	int64_t economic_victory_turn = 0;
	int64_t economic_victory_cost = 0;
	if ( buf.GetRemaining() > 0 ) {
		economic_victory_turn = buf.ReadInt< int64_t >( "economic victory turn" );
		economic_victory_cost = buf.ReadInt< int64_t >( "economic victory cost" );
		if (
			economic_victory_turn < 0 || economic_victory_cost < 0 ||
			economic_victory_cost > Player::MAX_ENERGY_CREDITS ||
			( economic_victory_turn == 0 ) != ( economic_victory_cost == 0 )
		) {
			THROW( "invalid serialized base economic victory state" );
		}
	}
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized base" );
	}
	auto base = std::make_unique< Base >(
		game,
		id,
		slot,
		faction,
		tile,
		name,
		pops,
		next_pop_id,
		production_queue,
		accumulated_minerals,
		facilities
	);
	if ( has_accumulated_nutrients ) {
		base->CustomSet(
			"accumulated_nutrients",
			VALUE( gse::value::Int, , accumulated_nutrients )
		);
	}
	if ( network_node_artifact_linked ) {
		base->CustomSet(
			"network_node_artifact_linked",
			VALUE( gse::value::Bool, , true )
		);
	}
	if ( economic_victory_turn > 0 ) {
		base->CustomSet(
			"economic_victory_turn",
			VALUE( gse::value::Int, , economic_victory_turn )
		);
		base->CustomSet(
			"economic_victory_cost",
			VALUE( gse::value::Int, , economic_victory_cost )
		);
	}
	base->RestoreWorkedTiles( GSE_CALL );
	return base.release();
}

WRAPIMPL_SERIALIZE( Base )
	buf->WriteInt( obj->m_id );
}

WRAPIMPL_DESERIALIZE( Base )
	const auto id = buf->ReadInt< size_t >( "base reference id" );
	const auto& base = game->GetBM()->GetBase( id );
	if ( !base ) {
		THROW( "base id not found: " + std::to_string( id ) );
	}
	return base->Wrap( GSE_CALL );
}

WRAPIMPL_DYNAMIC_GETTERS( Base )
	WRAPIMPL_GET_CUSTOM( "id", Int, m_id )
	WRAPIMPL_GET_CUSTOM( "name", String, m_name )
	WRAPIMPL_LINK( "get_owner", m_owner )
	WRAPIMPL_LINK( "get_tile", m_tile )
	WRAPIMPL_CUSTOM_SETTERS
	{
		"get_production",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			const auto* const production = GetProduction();
			auto* const def = production
				? GetProductionDef( *production )
				: nullptr;
			return def
				? def->Wrap( GSE_CALL )
				: VALUE( gse::value::Undefined );
		} )
	},
	{
		"get_production_queue",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			gse::value::array_elements_t result = {};
			result.reserve( m_production_queue.size() );
			for ( const auto& production : m_production_queue ) {
				auto* const def = GetProductionDef( production );
				if ( !def ) {
					GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown production definition: " + production.id );
				}
				result.push_back( def->Wrap( GSE_CALL ) );
			}
			return VALUE( gse::value::Array,, result );
		} )
	},
	{
		"can_produce",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( kind_string, 0, String );
			N_GETVALUE( def_id, 1, String );
			production_kind_t kind;
			if ( !ParseProductionKind( kind_string, kind ) ) {
				return VALUE( gse::value::Bool,, false );
			}
			return VALUE( gse::value::Bool,, CanProduce( { kind, def_id } ) );
		} )
	},
	{
		"can_queue_production",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( kind_string, 0, String );
			N_GETVALUE( def_id, 1, String );
			production_kind_t kind;
			if ( !ParseProductionKind( kind_string, kind ) ) {
				return VALUE( gse::value::Bool,, false );
			}
			return VALUE( gse::value::Bool,, CanQueueProduction( { kind, def_id } ) );
		} )
	},
	{
		"can_set_production",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( kind_string, 0, String );
			N_GETVALUE( def_id, 1, String );
			production_kind_t kind;
			if ( !ParseProductionKind( kind_string, kind ) ) {
				return VALUE( gse::value::Bool,, false );
			}
			return VALUE( gse::value::Bool,, CanSetProduction( { kind, def_id } ) );
		} )
	},
	{
		"set_production",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( kind_string, 0, String );
			N_GETVALUE( def_id, 1, String );
			production_kind_t kind;
			if ( !ParseProductionKind( kind_string, kind ) ) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Unknown production kind: " + kind_string );
			}
			SetProduction( GSE_CALL, { kind, def_id } );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"queue_production",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 2 );
			N_GETVALUE( kind_string, 0, String );
			N_GETVALUE( def_id, 1, String );
			production_kind_t kind;
			if ( !ParseProductionKind( kind_string, kind ) ) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Unknown production kind: " + kind_string );
			}
			EnqueueProduction( GSE_CALL, { kind, def_id } );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"remove_production",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( index, 0, Int );
			if ( index < 0 ) {
				GSE_ERROR( gse::EC.INVALID_CALL, "Production queue index is negative" );
			}
			RemoveProduction( GSE_CALL, static_cast< size_t >( index ) );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"set_production_queue",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( values, 0, Array );
			production_queue_t queue = {};
			queue.reserve( values.size() );
			for ( auto* const value : values ) {
				if ( value->type != gse::VT_OBJECT ) {
					GSE_ERROR( gse::EC.INVALID_CALL, "Production queue entries must be objects" );
				}
				const auto& properties = ( (gse::value::Object*)value )->value;
				{
					N_GETPROP( kind_string, properties, "kind", String );
					N_GETPROP( def_id, properties, "id", String );
					production_kind_t kind;
					if ( !ParseProductionKind( kind_string, kind ) ) {
						GSE_ERROR( gse::EC.INVALID_CALL, "Unknown production kind: " + kind_string );
					}
					queue.push_back( { kind, def_id } );
				}
			}
			SetProductionQueue( GSE_CALL, queue );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"clear_production",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 0 );
			ClearProduction();
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"has_facility",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( id, 0, String );
			return VALUE( gse::value::Bool,, HasFacility( id ) );
		} )
	},
	{
		"get_facilities",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			std::vector< std::string > ids( m_facilities.begin(), m_facilities.end() );
			std::sort( ids.begin(), ids.end() );
			gse::value::array_elements_t result = {};
			result.reserve( ids.size() );
			for ( const auto& id : ids ) {
				auto* const def = m_game->GetBM()->GetFacilityDef( id );
				if ( !def ) {
					GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown base facility: " + id );
				}
				result.push_back( def->Wrap( GSE_CALL ) );
			}
			return VALUE( gse::value::Array,, result );
		} )
	},
	{
		"add_facility",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( id, 0, String );
			AddFacility( GSE_CALL, id );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"remove_facility",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( id, 0, String );
			RemoveFacility( GSE_CALL, id );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"get_accumulated_minerals",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return VALUE( gse::value::Int,, m_accumulated_minerals );
		} )
	},
	{
		"set_accumulated_minerals",
		NATIVE_CALL( this ) {
			m_game->CheckRW( GSE_CALL );
			N_EXPECT_ARGS( 1 );
			N_GETVALUE( minerals, 0, Int );
			SetAccumulatedMinerals( GSE_CALL, minerals );
			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"set_owner",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( owner, 0, Player );
			SetOwner( GSE_CALL, owner );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"work_pop_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 2 );
			N_GETVALUE_UNWRAP( pop, 0, Pop );
			N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );
			WorkPopTile( GSE_CALL, pop, tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"unwork_pop_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 2 );
			N_GETVALUE_UNWRAP( pop, 0, Pop );
			N_GETVALUE_UNWRAP( tile, 1, map::tile::Tile );
			UnworkPopTile( GSE_CALL, pop, tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"create_pop",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE( data, 0, Object );
			N_GETPROP( def_id, data, "type", String );
			N_GETPROP_UNWRAP_OPT( worked_tile, data, "worked_tile", map::tile::Tile );
			auto* def = GetPopDef( GSE_CALL, def_id );
			const auto max_variants = (m_faction->m_flags & faction::Faction::FF_PROGENITOR)
				? 1 // aliens have 1 gender
				: 2; // humans have 2
			ASSERT( max_variants > 0, "no variants found for pop type: " + def_id );
			if (
				worked_tile &&
				(
					m_worked_tiles.find( worked_tile ) != m_worked_tiles.end() ||
					worked_tile->HasWorkingPopLink()
				)
			) {
				GSE_ERROR( gse::EC.GAME_ERROR, "worked tile already has a population" );
			}

			auto* const pop = AddPop( Pop( this, m_next_pop_id++, def, m_game->GetRandom()->GetUInt(0, max_variants - 1), worked_tile ) );
			if ( worked_tile ) {
				WorkPopTile( GSE_CALL, pop, worked_tile );
			}

			return pop->Wrap( GSE_CALL );
		} )
	},
	{
		"destroy_pop",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( pop, 0, Pop );

			const auto it = m_pops.find( pop->m_id );
			if ( it == m_pops.end() || &it->second != pop ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Base does not have pop " + std::to_string( pop->m_id ) );
			}

			RemovePop( GSE_CALL, pop->m_id );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"add_worked_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );

			if ( m_worked_tiles.find( tile ) != m_worked_tiles.end() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "This tile is already worked" );
			}
			auto* const pop = tile->GetWorkingPop();
			if ( !pop ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Worked tile has an invalid population link" );
			}
			WorkPopTile( GSE_CALL, pop, tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"remove_worked_tile",
		NATIVE_CALL( this ) {

			m_game->CheckRW( GSE_CALL );

			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );

			if ( m_worked_tiles.find( tile ) == m_worked_tiles.end() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "This tile is not worked" );
			}
			Pop* pop = nullptr;
			for ( auto& it : m_pops ) {
				if ( it.second.m_worked_tile == tile ) {
					if ( pop ) {
						GSE_ERROR( gse::EC.GAME_ERROR, "Multiple populations work this tile" );
					}
					pop = &it.second;
				}
			}
			if ( !pop ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "Worked tile does not identify its population" );
			}
			// Preserve compatibility with scripts that clear the two dynamic links first.
			tile->SetWorkingPop( GSE_CALL, pop );
			pop->SetWorkedTile( GSE_CALL, tile );
			UnworkPopTile( GSE_CALL, pop, tile );

			return VALUE( gse::value::Undefined );
		} )
	},
	{
		"is_tile_worked",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 1 );
			N_GETVALUE_UNWRAP( tile, 0, map::tile::Tile );
			return VALUE( gse::value::Bool,, m_worked_tiles.find( tile ) != m_worked_tiles.end() );
		} )
	},
	{
		"get_pops",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );

			gse::value::array_elements_t elements = {};

			for ( auto& it : m_pops ) {
				elements.push_back( it.second.Wrap( GSE_CALL ) );
			}

			return VALUE( gse::value::Array,, elements );
		} ),
	},
	{
		"get_size",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return VALUE( gse::value::Int,, m_pops.size() );
		} ),
	},
	{
		"get_workable_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetWorkableTiles( GSE_CALL );
		} ),
	},
	{
		"get_worked_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetWorkedTiles( GSE_CALL );
		} ),
	},
	{
		"get_unworked_tiles",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetUnworkedTiles( GSE_CALL );
		} ),
	},
	{
		"get_intake",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetIntake( GSE_CALL );
		} ),
	},
	{
		"get_consumption",
		NATIVE_CALL( this ) {
			N_EXPECT_ARGS( 0 );
			return GetConsumption( GSE_CALL );
		} ),
	},
WRAPIMPL_DYNAMIC_SETTERS( Base )
WRAPIMPL_DYNAMIC_ON_SET( Base )
	TriggerUpdate();
WRAPIMPL_DYNAMIC_END()

UNWRAPIMPL_PTR( Base )

void Base::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gse::Wrappable::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "pops" );
	for ( auto& it : m_pops ) {
		it.second.GetReachableObjects( reachable_objects );
	}
	GC_DEBUG_END();
}

const PopDef* const Base::GetPopDef( GSE_CALLABLE, const std::string& id ) const {
	auto* def = m_game->GetBM()->GetPopDef( id );
	if ( !def ) {
		GSE_ERROR( gse::EC.INVALID_DEFINITION, "Unknown pop type: " + id );
	}
	return def;
}

gse::value::Array* const Base::GetWorkableTiles( GSE_CALLABLE ) {
	auto* const result = m_game->GetBM()->Trigger( GSE_CALL, "get_base_workable_tiles", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
	}; } );
	ASSERT( result, "GetWorkableTiles result is null" );
	if ( result->type != gse::VT_ARRAY ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "get_base_workable_tiles must return array, got: " + result->ToString() );
	}
	for ( const auto& v : ((gse::value::Array*)result)->value ) {
		if ( v->type != gse::VT_OBJECT || ((gse::value::Object*)v)->object_class != "Tile" ) {
			GSE_ERROR( gse::EC.GAME_ERROR, "get_base_workable_tiles elements must be objects of type Tile, got: " + v->ToString() );
		}
	}
	return (gse::value::Array*)result;
}

gse::value::Array* const Base::GetWorkedTiles( GSE_CALLABLE ) {
	gse::value::array_elements_t result = {};
	for ( const auto& tile : m_worked_tiles ) {
		result.push_back( tile->Wrap( GSE_CALL ) );
	}
	return VALUE( gse::value::Array,, result );
}

gse::value::Array* const Base::GetUnworkedTiles( GSE_CALLABLE ) {
	gse::value::array_elements_t result = {};
	const auto* workable_tiles_v = GetWorkableTiles( GSE_CALL );
	for ( const auto& v : workable_tiles_v->value ) {
		ASSERT( v->type == gse::VT_OBJECT && ((gse::value::Object*)v)->object_class == "Tile", "invalid tile object" );
		auto* tile = (map::tile::Tile*)((gse::value::Object*)v)->wrapobj;
		if ( m_worked_tiles.find( tile ) == m_worked_tiles.end() ) {
			result.push_back( v );
		}
	}
	return VALUE( gse::value::Array,, result );
}

gse::value::Object* const Base::GetIntake( GSE_CALLABLE ) {
	return GetResourcesFromCallback( GSE_CALL, m_game->GetBM(), m_game->GetRM(), "get_base_intake", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
		{
			"player",
			m_owner->Wrap( GSE_CALL )
		},
	}; } );
}

gse::value::Object* const Base::GetConsumption( GSE_CALLABLE ) {
	return GetResourcesFromCallback( GSE_CALL, m_game->GetBM(), m_game->GetRM(), "get_base_consumption", ARGS_F( this ) {
		{
			"base",
			Wrap( GSE_CALL )
		},
		{
			"player",
			m_owner->Wrap( GSE_CALL )
		},
	}; } );
}

void Base::RestoreWorkedTiles( GSE_CALLABLE ) {
	if ( !m_worked_tiles.empty() ) {
		GSE_ERROR( gse::EC.GAME_ERROR, "base worked tiles were already restored" );
	}
	std::unordered_set< map::tile::Tile* > restored_tiles = {};
	for ( auto& it : m_pops ) {
		auto& pop = it.second;
		auto* const tile = pop.m_worked_tile;
		if ( tile ) {
			if ( !restored_tiles.insert( tile ).second ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "multiple populations work the same tile" );
			}
			if ( tile->HasWorkingPopLink() || pop.HasWorkedTileLink() ) {
				GSE_ERROR( gse::EC.GAME_ERROR, "serialized worker links were already restored" );
			}
		}
	}
	for ( auto& it : m_pops ) {
		auto& pop = it.second;
		auto* const tile = pop.m_worked_tile;
		if ( tile ) {
			tile->SetWorkingPop( GSE_CALL, &pop );
			pop.SetWorkedTile( GSE_CALL, tile );
		}
	}
	m_worked_tiles = restored_tiles;
}

bool Base::ValidateProductionQueue( const production_queue_t& production_queue, std::string& error ) const {
	if ( production_queue.size() > MAX_PRODUCTION_QUEUE_SIZE ) {
		error = "Production queue cannot contain more than " + std::to_string( MAX_PRODUCTION_QUEUE_SIZE ) + " entries";
		return false;
	}
	std::unordered_set< std::string > queued_constructions = {};
	size_t index = 0;
	for ( const auto& production : production_queue ) {
		if ( production.kind < PK_UNIT || production.kind > PK_PROJECT || production.id.empty() ) {
			error = "Invalid production queue entry";
			return false;
		}
		if ( !CanProduce( production ) ) {
			error = "Cannot produce " + GetProductionKindString( production.kind ) + ": " + production.id;
			return false;
		}
		bool is_repeatable = false;
		if ( production.kind == PK_FACILITY ) {
			const auto* const def = m_game->GetBM()->GetFacilityDef( production.id );
			is_repeatable = def && (
				def->m_mineral_to_energy_divisor > 0 ||
				!def->m_orbital_resource.empty() ||
				def->m_orbital_defense
			);
			if ( def && def->m_mineral_to_energy_divisor > 0 && index != 0 ) {
				error = "Repeatable production must be first in the production queue: " + production.id;
				return false;
			}
		}
		if (
			production.kind != PK_UNIT &&
			!is_repeatable &&
			!queued_constructions.insert( production.id ).second
		) {
			error = "Construction is already in the production queue: " + production.id;
			return false;
		}
		index++;
	}
	return true;
}

bool Base::ValidateFacilities( const facilities_t& facilities, std::string& error ) const {
	for ( const auto& id : facilities ) {
		const auto* const def = id.empty() ? nullptr : m_game->GetBM()->GetFacilityDef( id );
		if ( !def ) {
			error = "Unknown base facility: " + id;
			return false;
		}
		if (
			def->m_mineral_to_energy_divisor > 0 ||
			!def->m_orbital_resource.empty() ||
			def->m_orbital_defense
		) {
			error = "Repeatable production cannot be stored as a base facility: " + id;
			return false;
		}
	}
	return true;
}

void Base::TriggerUpdate() {
	m_game->GetBM()->AddUpdateTrigger( this );
}

}
}
}
