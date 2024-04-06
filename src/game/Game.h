#pragma once

#include <unordered_map>
#include <map>
#include <vector>

#include "base/MTModule.h"

#include "map/Map.h"
#include "map_editor/MapEditor.h"

#include "util/Random.h"
#include "types/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "Slot.h"
#include "FrontendRequest.h"
#include "BackendRequest.h"
#include "Turn.h"

#include "gse/GSE.h"
#include "gse/GlobalContext.h"

#include "animation/Def.h"
#include "unit/MoraleSet.h"
#include "unit/Def.h"
#include "unit/Unit.h"
#include "event/Event.h"

namespace game {

class State;
namespace connection {
class Connection;
}

namespace bindings {
class Binding;
}

enum op_t {
	OP_NONE,
	OP_PING,
	OP_INIT,
	OP_GET_MAP_DATA,
	OP_RESET,
	OP_SELECT_TILE,
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

enum tile_query_purpose_t {
	TQP_NONE,
	TQP_TILE_SELECT,
	TQP_UNIT_SELECT,
};

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
			map::Tile::direction_t tile_direction;
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
			map_editor::MapEditor::tool_type_t tool;
			map_editor::MapEditor::brush_type_t brush;
			map_editor::MapEditor::draw_mode_t draw_mode;
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

	vec3_t operator=( const Vec3& source ) {
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
	types::Texture* terrain_texture;
	types::mesh::Render* terrain_mesh;
	types::mesh::Data* terrain_data_mesh;
	std::string* path;
	struct {
		std::unordered_map< std::string, map::Map::sprite_actor_t >* actors;
		std::unordered_map< size_t, std::pair< std::string, Vec3 > >* instances;
	} sprites;
	~response_map_data_t() {
		if ( terrain_texture ) {
			DELETE( terrain_texture );
		}
		if ( terrain_mesh ) {
			DELETE( terrain_mesh );
		}
		if ( terrain_data_mesh ) {
			DELETE( terrain_data_mesh );
		}
	};
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
				map::Tile::elevation_t center;
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
				std::unordered_map< std::string, map::Map::sprite_actor_t >* actors_to_add;
				std::unordered_map< size_t, std::string >* instances_to_remove;
				std::unordered_map< size_t, std::pair< std::string, Vec3 > >* instances_to_add;
			} sprites;
		} edit_map;
		struct {
			std::vector< FrontendRequest >* requests;
		} get_frontend_requests;
	} data;
};

typedef base::MTModule< MT_Request, MT_Response > MTModule;

class InvalidEvent : public Exception {
public:
	InvalidEvent( const std::string& reason, const event::Event* event );
};

CLASS( Game, MTModule )

	// returns success as soon as this thread is ready (not busy with previous requests)
	mt_id_t MT_Ping();

	// initialize map and other things
	mt_id_t MT_Init( State* state );

	// send chat message
	mt_id_t MT_Chat( const std::string& message );

	// get map data for display
	mt_id_t MT_GetMapData();

	// deinitialize everything
	mt_id_t MT_Reset();

	// returns some data about tile
	mt_id_t MT_SelectTile(
		const tile_query_purpose_t tile_query_purpose,
		const types::Vec2< size_t >& tile_coords,
		const map::Tile::direction_t tile_direction = map::Tile::D_NONE,
		const tile_query_metadata_t tile_query_metadata = {}
	);

	// saves current map into file
	mt_id_t MT_SaveMap( const std::string& path );

	// perform edit operation on map tile(s)
	mt_id_t MT_EditMap( const types::Vec2< size_t >& tile_coords, map_editor::MapEditor::tool_type_t tool, map_editor::MapEditor::brush_type_t brush, map_editor::MapEditor::draw_mode_t draw_mode );

	// get all pending frontend requests (will be cleared after)
	mt_id_t MT_GetFrontendRequests();

	// send backend requests for processing
	mt_id_t MT_SendBackendRequests( const std::vector< BackendRequest >& requests );

	// send event
	mt_id_t MT_AddEvent( const event::Event* event );

#ifdef DEBUG

	mt_id_t MT_SaveDump( const std::string& path );
	mt_id_t MT_LoadDump( const std::string& path );

#endif

	void Start() override;
	void Stop() override;
	void Iterate() override;

	util::Random* GetRandom() const;
	map::Map* GetMap() const;
	State* GetState() const;
	const Player* GetPlayer() const;
	const size_t GetSlotNum() const;

protected:

	const MT_Response ProcessRequest( const MT_Request& request, MT_CANCELABLE ) override;
	void DestroyRequest( const MT_Request& request ) override;
	void DestroyResponse( const MT_Response& response ) override;

public:
	typedef std::function< void() > cb_animation_oncomplete;

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
	const std::string* ShowAnimationOnTile( const std::string& animation_id, const map::Tile* tile, const cb_animation_oncomplete& on_complete );
	void DefineMoraleSet( unit::MoraleSet* moraleset );
	void DefineUnit( unit::Def* def );
	void SpawnUnit( unit::Unit* unit );
	void SkipUnitTurn( const size_t unit_id );
	void DespawnUnit( const size_t unit_id );
	const std::string* MoveUnitValidate( unit::Unit* unit, map::Tile* dst_tile );
	const gse::Value MoveUnitResolve( unit::Unit* unit, map::Tile* dst_tile );
	void MoveUnitApply( unit::Unit* unit, map::Tile* dst_tile, const gse::Value resolutions );
	const std::string* AttackUnitValidate( unit::Unit* attacker, unit::Unit* defender );
	const gse::Value AttackUnitResolve( unit::Unit* attacker, unit::Unit* defender );
	void AttackUnitApply( unit::Unit* attacker, unit::Unit* defender, const gse::Value resolutions );
	const size_t GetTurnId() const;
	const bool IsTurnActive() const;
	const bool IsTurnCompleted( const size_t slot_num ) const;
	const bool IsTurnChecksumValid( const util::CRC32::crc_t checksum ) const;
	void CompleteTurn( const size_t slot_num );
	void UncompleteTurn( const size_t slot_num );
	void FinalizeTurn();
	void AdvanceTurn( const size_t turn_id );

	void GlobalFinalizeTurn();
	void GlobalProcessTurnFinalized( const size_t slot_num, const util::CRC32::crc_t checksum );
	void GlobalAdvanceTurn();

private:

	void ValidateEvent( event::Event* event );
	const gse::Value ProcessEvent( event::Event* event );

	const types::Vec3 GetTileRenderCoords( const map::Tile* tile );
	const types::Vec3 GetUnitRenderCoords( const unit::Unit* unit );

	std::unordered_set< std::string > m_defined_animations = {};

	std::unordered_map< std::string, unit::MoraleSet* > m_unit_moralesets = {};
	std::unordered_map< std::string, unit::Def* > m_unit_defs = {};
	std::map< size_t, unit::Unit* > m_units = {};
	void SerializeUnits( types::Buffer& buf ) const;
	void UnserializeUnits( types::Buffer& buf );

	enum game_state_t {
		GS_NONE,
		GS_PREPARING_MAP,
		GS_INITIALIZING,
		GS_RUNNING,
	};
	game_state_t m_game_state = GS_NONE;
	mt_flag_t m_init_cancel = false;
	std::string m_initialization_error = "";

	const Player* m_player = nullptr;
	size_t m_slot_num = 0;
	Slot* m_slot = nullptr;

	response_map_data_t* m_response_map_data = nullptr;

	CRC32::crc_t m_turn_checksum = 0;
	std::unordered_set< size_t > m_verified_turn_checksum_slots = {};

	std::vector< FrontendRequest >* m_pending_frontend_requests = nullptr;
	void AddFrontendRequest( const FrontendRequest& request );

	void InitGame( MT_Response& response, MT_CANCELABLE );
	void ResetGame();

	// seed needs to be consistent during session (to prevent save-scumming and for easier reproducing of bugs)
	util::Random* m_random = nullptr;
	State* m_state = nullptr;
	connection::Connection* m_connection = nullptr;

	map::Map* m_map = nullptr;
	map::Map* m_old_map = nullptr; // to restore state, for example if loading of another map failed
	map_editor::MapEditor* m_map_editor = nullptr;

	std::vector< game::event::Event* > m_unprocessed_events = {};
	std::vector< unit::Unit* > m_unprocessed_units = {};

	Turn m_current_turn = {};

	bool m_is_turn_complete = false;
	void CheckTurnComplete();

	size_t m_next_running_animation_id = 1;
	std::unordered_map< size_t, cb_animation_oncomplete > m_running_animations = {};
};

}
