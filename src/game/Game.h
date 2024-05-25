#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "base/MTModule.h"

#include "Types.h"
#include "map/Types.h"
#include "map_editor/Types.h"

#include "types/Exception.h"
#include "gse/Exception.h"
#include "FrontendRequest.h"
#include "BackendRequest.h"
#include "game/turn/Turn.h"
#include "TileLock.h"

// TODO: remove those
#include "map/tile/Tile.h"
#include "map/tile/TileState.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Render;
class Data;
}
}

namespace util::random {
class Random;
}

namespace game {

namespace animation {
class Def;
}

namespace unit {
class MoraleSet;
class Def;
class Unit;
}

namespace event {
class Event;
}

class State;
class Player;

namespace connection {
class Connection;
}

namespace bindings {
class Bindings;
}

namespace map {
class Map;
}

namespace map_editor {
class MapEditor;
}

namespace slot {
class Slot;
}

enum op_t {
	OP_NONE,
	OP_PING,
	OP_INIT,
	OP_GET_MAP_DATA,
	OP_RESET,
	OP_SAVE_MAP,
	OP_EDIT_MAP,
	OP_CHAT,
	OP_GET_FRONTEND_REQUESTS,
	OP_SEND_BACKEND_REQUESTS,
	OP_ADD_EVENT,
#ifdef DEBUG
	OP_SAVE_DUMP,
	OP_LOAD_DUMP,
#endif
};

enum result_t {
	R_NONE,
	R_SUCCESS,
	R_ABORTED,
	R_ERROR,
	R_PENDING,
};

typedef std::vector< types::mesh::Render* > data_tile_meshes_t;

typedef union {
	struct {
		bool try_next_unit;
		bool no_scroll_after;
	} tile_select;
	struct {
		size_t unit_id;
		bool no_scroll_after;
	} unit_select;
} tile_query_metadata_t;

struct MT_Request {
	op_t op;
	union {
		struct {
			State* state;
		} init;
		struct {
			tile_query_purpose_t purpose;
			size_t tile_x;
			size_t tile_y;
			map::tile::direction_t tile_direction;
			tile_query_metadata_t metadata;
		} select_tile;
		struct {
			std::string* path;
		} save_map;
		struct {
			std::string* message;
		} chat;
#ifdef DEBUG
		struct {
			std::string* path;
		} dump;
#endif
		struct {
			size_t tile_x;
			size_t tile_y;
			map_editor::tool_type_t tool;
			map_editor::brush_type_t brush;
			map_editor::draw_mode_t draw_mode;
		} edit_map;
		struct {
			std::string* serialized_event;
		} add_event;
		struct {
			std::vector< BackendRequest >* requests;
		} send_backend_requests;
	} data;
};

// can't use types::Vec* because only simple structs can be used in union
struct vec3_t {
	float x;
	float y;
	float z;

	vec3_t operator=( const types::Vec3& source ) {
		x = source.x;
		y = source.y;
		z = source.z;
		return *this;
	};

	void operator-=( const vec3_t rhs ) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
	};
};

struct vec2_t {
	float x;
	float y;
};

struct vertices_t {
	vec3_t center;
	vec3_t left;
	vec3_t top;
	vec3_t right;
	vec3_t bottom;
};

struct response_map_data_t {
	size_t map_width;
	size_t map_height;
	types::texture::Texture* terrain_texture;
	types::mesh::Render* terrain_mesh;
	types::mesh::Data* terrain_data_mesh;
	std::string* path;
	struct {
		std::unordered_map< std::string, map::sprite_actor_t >* actors;
		std::unordered_map< size_t, std::pair< std::string, types::Vec3 > >* instances;
	} sprites;
	const std::vector< map::tile::Tile >* tiles;
	const std::vector< map::tile::TileState >* tile_states;
	~response_map_data_t();
};

struct MT_Response {
	op_t op;
	result_t result;
	union {
		struct {
			size_t slot_index;
		} init;
		response_map_data_t* get_map_data;
		struct {
			const std::string* error_text;
		} error;
		struct {
			tile_query_purpose_t purpose;
			size_t tile_x;
			size_t tile_y;
			vec2_t coords;
			struct {
				map::tile::elevation_t center;
			} elevation;
			vertices_t selection_coords;
			data_tile_meshes_t* preview_meshes;
			std::vector< std::string >* preview_lines;
			bool scroll_adaptively;
			std::vector< std::string >* sprites;
			std::vector< size_t >* unit_ids;
			tile_query_metadata_t metadata;
		} select_tile;
		struct {
			std::string* path;
		} save_map;
		struct {
			struct {
				std::unordered_map< std::string, map::sprite_actor_t >* actors_to_add;
				std::unordered_map< size_t, std::string >* instances_to_remove;
				std::unordered_map< size_t, std::pair< std::string, types::Vec3 > >* instances_to_add;
			} sprites;
		} edit_map;
		struct {
			std::vector< FrontendRequest >* requests;
		} get_frontend_requests;
	} data;
};

typedef base::MTModule< MT_Request, MT_Response > MTModule;

class InvalidEvent : public types::Exception {
public:
	InvalidEvent( const std::string& reason, const event::Event* event );
};

CLASS( Game, MTModule )

	// returns success as soon as this thread is ready (not busy with previous requests)
	base::mt_id_t MT_Ping();

	// initialize map and other things
	base::mt_id_t MT_Init( State* state );

	// send chat message
	base::mt_id_t MT_Chat( const std::string& message );

	// get map data for display
	base::mt_id_t MT_GetMapData();

	// deinitialize everything
	base::mt_id_t MT_Reset();

	// saves current map into file
	base::mt_id_t MT_SaveMap( const std::string& path );

	// perform edit operation on map tile(s)
	base::mt_id_t MT_EditMap( const types::Vec2< size_t >& tile_coords, map_editor::tool_type_t tool, map_editor::brush_type_t brush, map_editor::draw_mode_t draw_mode );

	// get all pending frontend requests (will be cleared after)
	base::mt_id_t MT_GetFrontendRequests();

	// send backend requests for processing
	base::mt_id_t MT_SendBackendRequests( const std::vector< BackendRequest >& requests );

	// send event
	base::mt_id_t MT_AddEvent( const event::Event* event );

#ifdef DEBUG

	base::mt_id_t MT_SaveDump( const std::string& path );
	base::mt_id_t MT_LoadDump( const std::string& path );

#endif

	void Start() override;
	void Stop() override;
	void Iterate() override;

	util::random::Random* GetRandom() const;
	map::Map* GetMap() const;
	State* GetState() const;
	const Player* GetPlayer() const;
	const size_t GetSlotNum() const;

protected:

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

public:
	typedef std::function< void() > cb_oncomplete;

	// for bindings etc
	void Message( const std::string& text );
	void Quit( const std::string& reason );
	void OnGSEError( gse::Exception& e );
	unit::MoraleSet* GetMoraleSet( const std::string& name ) const;
	unit::Unit* GetUnit( const size_t id ) const;
	unit::Def* GetUnitDef( const std::string& name ) const;
	void AddEvent( event::Event* event );
	void RefreshUnit( const unit::Unit* unit );
	void DefineAnimation( animation::Def* def );
	const std::string* ShowAnimationOnTile( const std::string& animation_id, map::tile::Tile* tile, const cb_oncomplete& on_complete );
	void DefineMoraleSet( unit::MoraleSet* moraleset );
	void DefineUnit( unit::Def* def );
	void SpawnUnit( unit::Unit* unit );
	void SkipUnitTurn( const size_t unit_id );
	void DespawnUnit( const size_t unit_id );
	const std::string* MoveUnitValidate( unit::Unit* unit, map::tile::Tile* dst_tile );
	const gse::Value MoveUnitResolve( unit::Unit* unit, map::tile::Tile* dst_tile );
	void MoveUnitApply( unit::Unit* unit, map::tile::Tile* dst_tile, const gse::Value resolutions );
	const std::string* MoveUnitToTile( unit::Unit* unit, map::tile::Tile* dst_tile, const cb_oncomplete& on_complete );
	const std::string* AttackUnitValidate( unit::Unit* attacker, unit::Unit* defender );
	const gse::Value AttackUnitResolve( unit::Unit* attacker, unit::Unit* defender );
	void AttackUnitApply( unit::Unit* attacker, unit::Unit* defender, const gse::Value resolutions );
	const size_t GetTurnId() const;
	const bool IsTurnActive() const;
	const bool IsTurnCompleted( const size_t slot_num ) const;
	const bool IsTurnChecksumValid( const util::crc32::crc_t checksum ) const;
	void CompleteTurn( const size_t slot_num );
	void UncompleteTurn( const size_t slot_num );
	void FinalizeTurn();
	void AdvanceTurn( const size_t turn_id );

	void GlobalFinalizeTurn();
	void GlobalProcessTurnFinalized( const size_t slot_num, const util::crc32::crc_t checksum );
	void GlobalAdvanceTurn();

	void SendTileLockRequest( const map::tile::positions_t& tile_positions, const cb_oncomplete& on_complete );
	void RequestTileLocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions );
	void LockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions );

	void SendTileUnlockRequest( const map::tile::positions_t& tile_positions );
	void RequestTileUnlocks( const size_t initiator_slot, const map::tile::positions_t& tile_positions );
	void UnlockTiles( const size_t initiator_slot, const map::tile::positions_t& tile_positions );

private:

	void ValidateEvent( event::Event* event );
	const gse::Value ProcessEvent( event::Event* event );

	const types::Vec3 GetTileRenderCoords( const map::tile::Tile* tile );
	const types::Vec3 GetUnitRenderCoords( const unit::Unit* unit );

	std::unordered_map< std::string, unit::MoraleSet* > m_unit_moralesets = {};
	std::unordered_map< std::string, unit::Def* > m_unit_defs = {};
	std::map< size_t, unit::Unit* > m_units = {};
	void SerializeUnits( types::Buffer& buf ) const;
	void UnserializeUnits( types::Buffer& buf );

	std::unordered_map< std::string, animation::Def* > m_animation_defs = {};
	void SerializeAnimations( types::Buffer& buf ) const;
	void UnserializeAnimations( types::Buffer& buf );

	enum game_state_t {
		GS_NONE,
		GS_PREPARING_MAP,
		GS_INITIALIZING,
		GS_RUNNING,
	};
	game_state_t m_game_state = GS_NONE;
	base::mt_flag_t m_init_cancel = false;
	std::string m_initialization_error = "";

	const Player* m_player = nullptr;
	size_t m_slot_num = 0;
	slot::Slot* m_slot = nullptr;

	response_map_data_t* m_response_map_data = nullptr;

	util::crc32::crc_t m_turn_checksum = 0;
	std::unordered_set< size_t > m_verified_turn_checksum_slots = {};

	std::vector< FrontendRequest >* m_pending_frontend_requests = nullptr;
	void AddFrontendRequest( const FrontendRequest& request );

	void InitGame( MT_Response& response, MT_CANCELABLE );
	void ResetGame();

	// seed needs to be consistent during session (to prevent save-scumming and for easier reproducing of bugs)
	util::random::Random* m_random = nullptr;
	State* m_state = nullptr;
	connection::Connection* m_connection = nullptr;

	map::Map* m_map = nullptr;
	map::Map* m_old_map = nullptr; // to restore state, for example if loading of another map failed
	map_editor::MapEditor* m_map_editor = nullptr;

	std::vector< game::event::Event* > m_unprocessed_events = {};
	std::vector< unit::Unit* > m_unprocessed_units = {};

	turn::Turn m_current_turn = {};

	bool m_is_turn_complete = false;
	void CheckTurnComplete();

	size_t m_next_running_animation_id = 1;
	std::unordered_map< size_t, cb_oncomplete > m_running_animations_callbacks = {};

	enum unit_update_op_t : uint8_t {
		UUO_NONE = 0,
		UUO_SPAWN = 1 << 0,
		UUO_REFRESH = 1 << 1,
		UUO_DESPAWN = 1 << 2,
	};
	struct unit_update_t {
		unit_update_op_t ops = UUO_NONE;
		const unit::Unit* unit = nullptr;
	};
	std::unordered_map< size_t, unit_update_t > m_unit_updates = {};

	void QueueUnitUpdate( const unit::Unit* unit, const unit_update_op_t op );

	// server-side lock tracking
	struct tile_lock_request_t {
		const bool is_lock; // lock or unlock
		const size_t initiator_slot;
		const map::tile::positions_t tile_positions;
	};
	typedef std::vector< tile_lock_request_t > tile_lock_requests_t;  // requests fifo
	tile_lock_requests_t m_tile_lock_requests = {};
	void AddTileLockRequest( const bool is_lock, const size_t initiator_slot, const map::tile::positions_t& tile_positions );
	std::unordered_map< size_t, std::vector< TileLock > > m_tile_locks = {}; // slot id, locks
	void ProcessTileLockRequests();

	std::vector< std::pair< map::tile::positions_t, cb_oncomplete > > m_tile_lock_callbacks = {}; // tile positions (for matching), callback

private:
	friend class bindings::Bindings;
	void PushUnitUpdates();

};

}
