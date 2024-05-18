#pragma once

#include <unordered_set>
#include <unordered_map>

#include "base/Task.h"

#include "Types.h"
#include "base/MTTypes.h"
#include "ui/Types.h"
#include "game/turn/Types.h"
#include "game/map/Types.h"
#include "game/map_editor/Types.h"
#include "game/unit/Types.h"
#include "game/Types.h"
#include "rr/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"
#include "util/Clamper.h"
#include "util/Scroller.h"

// TODO: remove those
#include "game/map/tile/Tile.h"
#include "game/map/tile/TileState.h"
#include "Tile.h"
#include "game/BackendRequest.h"

// TODO: remove this
#include "game/map/Consts.h"
#include "util/FS.h"

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

namespace scene {
class Scene;
class Camera;
class Light;
namespace actor {
class Instanced;
}
}

namespace ui {
namespace event {
class UIEventHandler;
}
namespace style {
class Theme;
}
}

namespace game {
class State;
class FrontendRequest;
namespace animation {
class Def;
}
namespace unit {
class Def;
}
}

namespace task {
namespace game {

class InstancedSpriteManager;
class InstancedSprite;
class Sprite;
class Slot;
class AnimationDef;
class Animation;
class UnitDef;
class Unit;
class SlotBadges;
class BadgeDefs;

namespace ui {
class BottomBar;
class UnitsList;
namespace style {
class Theme;
}
}

namespace actor {
class TileSelection;
class Actor;
}

CLASS( Game, base::Task )

	Game( ::game::State* state, ::ui::ui_handler_t on_start = 0, ::ui::ui_handler_t on_cancel = 0 );
	~Game();

	void Start() override;
	void Stop() override;
	void Iterate() override;

	const bool IsMapEditingAllowed() const;

	struct consts_t {
		const struct {
			const struct {
				const float zoom_speed = 0.3f;
			} smooth_scrolling;
			struct {
				struct {
					const size_t fullscreen = 1;
					const size_t windowed = 10; // harder to place cursor at edge of window
				} edge_distance_px;
				const uint16_t scroll_step_ms = 2;
				const types::Vec2< float > speed = {
					0.002f * scroll_step_ms,
					0.003f * scroll_step_ms,
				};
			} static_scrolling;
			struct {
				const float scroll_if_selected_tile_farther_from_center_than = 0.4f;
			} key_scrolling;
		} map_scroll;
		const struct {
			const size_t draw_frequency_ms = 60; // TODO: this value doesn't seem realistic, why?
		} map_editing;
	};
	static const consts_t s_consts;

	InstancedSpriteManager* m_ism = nullptr;
	BadgeDefs* m_badge_defs = nullptr;

	const size_t GetMapWidth() const;
	const size_t GetMapHeight() const;
	const std::string& GetMapFilename() const;
	const std::string& GetMapLastDirectory() const;

	InstancedSpriteManager* GetISM() const;
	types::texture::Texture* GetSourceTexture( const std::string& name );
	InstancedSprite* GetTerrainInstancedSprite( const ::game::map::sprite_actor_t& actor );

	void CenterAtCoordinatePercents( const ::types::Vec2< float > position_percents );

	void SmoothScroll( const float scroll_value );
	void SmoothScroll( const ::types::Vec2< float > position, const float scroll_value );

	util::random::Random* GetRandom() const;

	void CloseMenus();

	typedef std::function< void() > f_exit_game;
	f_exit_game m_on_game_exit = nullptr;
	void ExitGame( const f_exit_game on_game_exit );
	void ReturnToMainMenu( const std::string reason = "" );

	const size_t GetBottomBarMiddleHeight() const;
	const size_t GetViewportHeight() const;

	void AddMessage( const std::string& text );
	void SendChatMessage( const std::string& text );
	void CompleteTurn();
	void UncompleteTurn();

	void LoadMap( const std::string& path );
	void SaveMap( const std::string& path );

	void ConfirmExit( ::ui::ui_handler_t on_confirm );

	types::texture::Texture* GetTerrainTexture() const;

	void SetEditorTool( ::game::map_editor::tool_type_t tool );
	const ::game::map_editor::tool_type_t GetEditorTool() const;

	void SetEditorBrush( ::game::map_editor::brush_type_t editor_brush );
	const ::game::map_editor::brush_type_t GetEditorBrush() const;

private:

	size_t m_slot_index = 0;
	bool m_is_turn_active = false;
	::game::turn::turn_status_t m_turn_status = ::game::turn::TS_PLEASE_WAIT;
	size_t m_turn_id = 0;
	void ActivateTurn();
	void DeactivateTurn();

	const std::string TERRAIN_SOURCE_PCX = "ter1.pcx";

	::game::map_editor::tool_type_t m_editor_tool = ::game::map_editor::TT_NONE;
	::game::map_editor::brush_type_t m_editor_brush = ::game::map_editor::BT_NONE;

	void UpdateMapData( const types::Vec2< size_t >& map_size );

	void DefineSlot(
		const size_t slot_index,
		const types::Color& color,
		const bool is_progenitor
	);
	void DefineAnimation( const ::game::animation::Def* def );
	void ShowAnimation( AnimationDef* def, const size_t animation_id, const ::types::Vec3& render_coords );
	void DefineUnit( const ::game::unit::Def* def );
	void SpawnUnit(
		const size_t unit_id,
		const std::string& unitdef_name,
		const size_t slot_index,
		const ::types::Vec2< size_t >& tile_coords,
		const ::types::Vec3& render_coords,
		const ::game::unit::movement_t movement,
		const ::game::unit::morale_t morale,
		const std::string& morale_string,
		const ::game::unit::health_t health
	);
	void DespawnUnit( const size_t unit_id );
	void RefreshUnit( Unit* unit );
	void MoveUnit( Unit* unit, Tile* dst_tile, const types::Vec3& dst_render_coords );

	void ProcessRequest( const ::game::FrontendRequest* request );
	void SendBackendRequest( const ::game::BackendRequest* request );

	bool m_is_initialized = false;
	void Initialize(
		types::texture::Texture* terrain_texture,
		types::mesh::Render* terrain_mesh,
		types::mesh::Data* terrain_data_mesh,
		const std::unordered_map< std::string, ::game::map::sprite_actor_t >& sprite_actors,
		const std::unordered_map< size_t, std::pair< std::string, ::types::Vec3 > >& sprite_instances,
		const std::vector< ::game::map::tile::Tile >* tiles,
		const std::vector< ::game::map::tile::TileState >* tile_states
	);
	void Deinitialize();

	::ui::ui_handler_t m_on_start;
	::ui::ui_handler_t m_on_cancel;

	void SetCameraPosition( const ::types::Vec3 camera_position );

	::game::State* m_state = nullptr;

	// seed needs to be consistent during session (to prevent save-scumming and for easier reproducing of bugs)
	util::random::Random* m_random = nullptr;

	// map rendering stuff
	types::Vec3 m_camera_position; // { x, y, zoom }
	types::Vec3 m_camera_angle;
	scene::Camera* m_camera = nullptr;
	scene::Light* m_light_a = nullptr; // Alpha Centauri A
	scene::Light* m_light_b = nullptr; // Alpha Centauri B
	scene::Scene* m_world_scene = nullptr;

	bool m_is_editing_mode = false;
	::game::map_editor::draw_mode_t m_editor_draw_mode = ::game::map_editor::DM_NONE;
	util::Timer m_editing_draw_timer;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	util::Scroller< types::Vec3 > m_scroller;

	struct {
		bool is_dragging = false;
		types::Vec2< float > last_drag_position;
		struct {
			util::Timer timer;
			types::Vec2< float > speed = {
				0,
				0
			};
		} edge_scrolling;
		types::Vec2< float > last_mouse_position;
		float key_zooming = 0;
	} m_map_control = {};

	struct {
		size_t window_width;
		size_t window_height;
		types::Vec2< size_t > min;
		types::Vec2< size_t > max;
		size_t bottom_bar_overlap;
		types::Vec2< float > ratio;
		size_t width;
		size_t height;
		float aspect_ratio;
		float window_aspect_ratio;
		bool is_fullscreen;
	} m_viewport;
	struct {
		types::Vec3 min;
		types::Vec3 max;
	} m_camera_range;

	// shift x to center instance when needed
	const float GetFixedX( float x ) const;
	void FixCameraX();

	struct {
		const ::ui::event::UIEventHandler* keydown_before = nullptr;
		const ::ui::event::UIEventHandler* keydown_after = nullptr;
		const ::ui::event::UIEventHandler* keyup = nullptr;
		const ::ui::event::UIEventHandler* mousedown = nullptr;
		const ::ui::event::UIEventHandler* mousemove = nullptr;
		const ::ui::event::UIEventHandler* mouseup = nullptr;
		const ::ui::event::UIEventHandler* mousescroll = nullptr;
	} m_handlers;
	void UpdateViewport();
	void UpdateCameraPosition();
	void UpdateCameraAngle();
	void UpdateCameraScale();
	void UpdateCameraRange();
	void UpdateMapInstances();
	void UpdateUICamera();

	// TODO: move this to .cpp
	// structures received from game thread
	struct map_data_t {
		size_t width = 0;
		size_t height = 0;
		struct {
			types::Vec2< float > min = {};
			types::Vec2< float > max = {};
			struct {
				util::Clamper< float > x;
				util::Clamper< float > y;
			} percent_to_absolute;
		} range;
		std::string filename =
			::game::map::s_consts.fs.default_map_filename +
				::game::map::s_consts.fs.default_map_extension;
		std::string last_directory =
			util::FS::GetCurrentDirectory() +
				util::FS::PATH_SEPARATOR +
				::game::map::s_consts.fs.default_map_directory;
	};

	const bool m_is_map_editing_allowed = false;

	Tile* m_selected_tile = nullptr;
	Unit* m_selected_unit = nullptr;
	map_data_t m_map_data = {};

	// UI stuff

	struct {
		ui::style::Theme* theme = nullptr;
		task::game::ui::BottomBar* bottom_bar = nullptr;
	} m_ui;

	bool m_is_resize_handler_set = false;

	void SelectTileAtPoint( const ::game::tile_query_purpose_t tile_query_purpose, const size_t x, const size_t y );
	void SelectTileOrUnit( Tile* tile, const size_t selected_unit_id = 0 );
	void DeselectTileOrUnit();
	Unit* GetFirstSelectableUnit( const std::unordered_map< size_t, Unit* >& units ) const;

private:
	friend class ui::UnitsList;
	void SelectUnit( Unit* unit_data, const bool actually_select_unit );

	struct {
		std::unordered_map< std::string, types::texture::Texture* > source;
		types::texture::Texture* terrain = nullptr;
	} m_textures;

	struct {
		actor::TileSelection* tile_selection = nullptr;
		scene::actor::Instanced* terrain = nullptr;
	} m_actors;

	// some additional management of world actors such as calling Iterate()
	// note that all world actors must be instanced
	std::unordered_map< actor::Actor*, scene::actor::Instanced* > m_actors_map = {};

	void AddActor( actor::Actor* actor );
	void RemoveActor( actor::Actor* actor );

	const ::types::Vec2< float > GetTileWindowCoordinates( const ::types::Vec3& tile_coords );

	void ScrollTo( const ::types::Vec3& target );
	void ScrollToTile( const Tile* tile, bool center_on_tile );

	struct tile_at_result_t {
		bool is_set = false;
		types::Vec2< size_t > tile_pos;
	};

	// tile request stuff
	rr::id_t m_tile_at_request_id = 0;
	::game::tile_query_purpose_t m_tile_at_query_purpose = ::game::TQP_NONE;

	void CancelTileAtRequest();
	void GetTileAtScreenCoords( const ::game::tile_query_purpose_t tile_query_purpose, const size_t screen_x, const size_t screen_inverse_y ); // async, y needs to be upside down
	const bool IsTileAtRequestPending() const;
	const tile_at_result_t GetTileAtScreenCoordsResult();

	// minimap stuff
	rr::id_t m_minimap_texture_request_id = 0;
	void GetMinimapTexture( scene::Camera* camera, const ::types::Vec2< size_t > texture_dimensions );
	types::texture::Texture* GetMinimapTextureResult();
	void UpdateMinimap();

	void ResetMapState();

	struct {
		base::mt_id_t ping = 0;
		base::mt_id_t init = 0;
		base::mt_id_t get_map_data = 0;
		base::mt_id_t reset = 0;
		std::unordered_set< base::mt_id_t > select_tile = {};
		base::mt_id_t save_map = 0;
		base::mt_id_t edit_map = 0;
		base::mt_id_t chat = 0;
		base::mt_id_t get_frontend_requests = 0;
		base::mt_id_t send_backend_requests = 0;
#ifdef DEBUG
		base::mt_id_t save_dump = 0;
		// init will be used for loading dump
#endif
	} m_mt_ids = {};

	std::unordered_map< size_t, Tile > m_tiles = {};

	std::unordered_map< size_t, Slot* > m_slots = {};
	std::unordered_map< std::string, AnimationDef* > m_animationdefs = {};
	std::unordered_map< size_t, Animation* > m_animations;
	std::unordered_map< std::string, UnitDef* > m_unitdefs = {};
	std::unordered_map< size_t, Unit* > m_units = {};
	struct {
		std::vector< Unit* > units = {};
		size_t selected_id_index = 0;
	} m_selectables = {};
	void UpdateSelectable( Unit* unit );
	void AddSelectable( Unit* unit );
	void RemoveSelectable( Unit* unit );
	void SetCurrentSelectable( Unit* unit );
	Unit* GetCurrentSelectable();
	Unit* GetNextSelectable();
	const bool SelectNextUnitMaybe();
	void SelectNextUnitOrSwitchToTileSelection();

	void CancelRequests();
	void CancelGame();

	std::vector< ::game::BackendRequest > m_pending_backend_requests = {};

	enum unit_update_flags_t : uint8_t {
		UUF_NONE = 0,
		UUF_POSITION = 1 << 0,
		UUF_MORALE = 1 << 1,
		UUF_HEALTH = 1 << 2,
		UUF_ALL = 0xff,
	};

	Tile* GetTile( const size_t x, const size_t y );
	Tile* GetTile( const types::Vec2< size_t >& coords );

	void ShowTileSelector();
	void HideTileSelector();
	void RenderTile( Tile* tile );

};

}
}
