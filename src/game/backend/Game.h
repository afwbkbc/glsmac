#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <map>
#include <vector>

#include "common/MTModule.h"
#include "gse/GCWrappable.h"

#include "Types.h"
#include "game/backend/map/Types.h"
#include "game/backend/map_editor/Types.h"

#include "types/Exception.h"
#include "gse/Exception.h"
#include "game/FrontendRequest.h"
#include "game/BackendRequest.h"
#include "game/backend/turn/Turn.h"

// TODO: remove those
#include "game/backend/map/tile/Tile.h"
#include "game/backend/map/tile/TileState.h"

class GLSMAC;

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Render;

class Data;
}
}

namespace game {
namespace backend {

class Random;

namespace animation {
class Def;
}

namespace faction {
class Faction;
}

namespace map {
namespace tile {
class TileManager;
}
}

namespace resource {
class ResourceManager;
}

namespace unit {
class UnitManager;
}

namespace base {
class BaseManager;
}

namespace animation {
class AnimationManager;
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

namespace event {
class Event;
class EventHandler;
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

typedef common::MTModule< MT_Request, MT_Response > MTModule;

class InvalidEvent : public types::Exception {
public:
	InvalidEvent( const std::string& reason, const event::Event* event );
};

CLASS2( Game, MTModule, gse::GCWrappable )

	Game();

	// returns success as soon as this thread is ready (not busy with previous requests)
	common::mt_id_t MT_Ping();

	// initialize map and other things
	common::mt_id_t MT_Init( State* state );

	// send chat message
	common::mt_id_t MT_Chat( const std::string& message );

	// get map data for display
	common::mt_id_t MT_GetMapData();

	// deinitialize everything
	common::mt_id_t MT_Reset();

	// saves current map into file
	common::mt_id_t MT_SaveMap( const std::string& path );

	// perform edit operation on map tile(s)
	common::mt_id_t MT_EditMap( const types::Vec2< size_t >& tile_coords, map_editor::tool_type_t tool, map_editor::brush_type_t brush, map_editor::draw_mode_t draw_mode );

	// get all pending frontend requests (will be cleared after)
	common::mt_id_t MT_GetFrontendRequests();

	// send backend requests for processing
	common::mt_id_t MT_SendBackendRequests( const std::vector< BackendRequest >& requests );

#ifdef DEBUG

	common::mt_id_t MT_SaveDump( const std::string& path );
	common::mt_id_t MT_LoadDump( const std::string& path );

#endif

	void Start() override;
	void Stop() override;
	void Iterate() override;

	const bool IsStarted() const;
	Random* GetRandom() const;
	map::Map* GetMap() const;
	State* GetState() const;
	const Player* GetPlayer() const;
	const size_t GetSlotNum() const;

	void ShowLoader( const std::string& text );
	void SetLoaderText( const std::string& text );
	void HideLoader();

	void Event( GSE_CALLABLE, const std::string& name, const gse::value::object_properties_t& args );
	void AddEvent( event::Event* const event );
	void AddEventResponse( const std::string& event_id, const bool result );

	WRAPDEFS_PTR( Game )

	void GetReachableObjects( std::unordered_set< Object* >& reachable_objects ) override;

protected:

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

public:
	typedef std::function< void() > cb_oncomplete;

	// for bindings etc
	void Message( const std::string& text );
	void Quit( const std::string& reason );
	void OnError( std::runtime_error& err );
	void OnGSEError( const gse::Exception& e );
	const size_t GetTurnId() const;
	const bool IsTurnCompleted( const size_t slot_num ) const;
	const bool IsTurnChecksumValid( const util::crc32::crc_t checksum ) const;
	void CompleteTurn( GSE_CALLABLE, const size_t slot_num );
	void UncompleteTurn( const size_t slot_num );
	void AdvanceTurn( const size_t turn_id );

	void GlobalFinalizeTurn( GSE_CALLABLE );
	void GlobalAdvanceTurn( GSE_CALLABLE );

	faction::Faction* GetFaction( const std::string& id ) const;

	map::tile::TileManager* GetTM() const;
	resource::ResourceManager* GetRM() const;
	unit::UnitManager* GetUM() const;
	base::BaseManager* GetBM() const;
	animation::AnimationManager* GetAM() const;

	gc::Space* const GetGCSpace() const;

	void CheckRW( GSE_CALLABLE );

	void SetSlotNum( const size_t slotnum ); // not thread-safe, make sure to call from game thread only

private:

	const types::Vec3 GetTileRenderCoords( const map::tile::Tile* tile );

	void UpdateYields( GSE_CALLABLE, map::tile::Tile* tile ) const;

	map::tile::TileManager* m_tm = nullptr;
	resource::ResourceManager* m_rm = nullptr;
	unit::UnitManager* m_um = nullptr;
	base::BaseManager* m_bm = nullptr;
	animation::AnimationManager* m_am = nullptr;

	enum game_state_t {
		GS_NONE,
		GS_PREPARING_MAP,
		GS_INITIALIZING,
		GS_STARTING,
		GS_RUNNING,
	};
	std::atomic< game_state_t > m_game_state = GS_NONE;
	common::mt_flag_t m_init_cancel = false;
	std::string m_initialization_error = "";

	Player* m_player = nullptr;
	bool m_is_master = true;
	size_t m_slot_num = 0;
	slot::Slot* m_slot = nullptr;

	response_map_data_t* m_response_map_data = nullptr;

	util::crc32::crc_t m_turn_checksum = 0;
	std::unordered_set< size_t > m_verified_turn_checksum_slots = {};

	std::vector< FrontendRequest >* m_pending_frontend_requests = nullptr;

	void InitGame( MT_Response& response, MT_CANCELABLE );
	void ResetGame();

	// seed needs to be consistent during session (to prevent save-scumming and for easier reproduction of bugs)
	Random* m_random = nullptr;
	State* m_state = nullptr;

	map::Map* m_map = nullptr;
	map::Map* m_old_map = nullptr; // to restore state, for example if loading of another map failed
	map_editor::MapEditor* m_map_editor = nullptr;

	turn::Turn m_current_turn = {};

	bool m_is_turn_complete = false;
	void CheckTurnComplete();

	std::unordered_map< std::string, event::EventHandler* > m_event_handlers = {};

	std::vector< event::Event* > m_pending_events = {};
	std::mutex m_pending_events_mutex;

	struct event_waiting_for_response_t {
		event::Event* event;
		gse::Value* rollback_data;
	};
	std::unordered_map< std::string, event_waiting_for_response_t > m_events_waiting_for_responses = {};

	std::vector< std::pair< std::string, bool > > m_pending_event_responses = {};
	std::mutex m_pending_event_responses_mutex;

	std::atomic< uint64_t > m_next_event_id = 0;

	std::atomic< size_t > m_rw_counter = 0;
	void WithRW( const std::function< void() >& f );

	void InitComplete( GSE_CALLABLE );

private:
	friend class map::tile::TileManager;
	friend class resource::ResourceManager;
	friend class unit::UnitManager;
	friend class base::BaseManager;
	friend class animation::AnimationManager;
	void AddFrontendRequest( const FrontendRequest& request );

	const bool IsRunning() const;

private:
	friend class event::Event;
	const std::string GenerateEventId();

private:
	friend class ::GLSMAC;
	void SetState( State* const state );

};

}
}
