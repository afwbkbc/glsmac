#pragma once

#include <unordered_set>
#include <unordered_map>

#include "common/Task.h"

#include "Types.h"
#include "common/MTTypes.h"
#include "ui/Types.h"
#include "game/backend/turn/Types.h"
#include "game/backend/map/Types.h"
#include "game/backend/map_editor/Types.h"
#include "game/backend/unit/Types.h"
#include "game/backend/Types.h"
#include "rr/Types.h"
#include "resource/Types.h"

#include "types/Vec2.h"
#include "types/Vec3.h"
#include "types/Color.h"
#include "util/Clamper.h"
#include "util/Scroller.h"

// TODO: remove those
#include "game/backend/map/tile/TileState.h"
#include "task/game/tile/Tile.h"
#include "game/BackendRequest.h"

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
class FrontendRequest;
namespace backend {
class State;
namespace animation {
class Def;
}
namespace unit {
class Def;
}
}
}

namespace task {
namespace game {

namespace faction {
class FactionManager;
class Faction;
}

namespace tile {
class TileManager;
class Tile;
}

namespace unit {
class UnitDef;
class Unit;
class UnitManager;
}

namespace base {
class BaseManager;
class Base;
}

namespace sprite {
class InstancedSpriteManager;
class InstancedSprite;
class Sprite;
}

namespace text {
class InstancedTextManager;
}

class Slot;
class AnimationDef;
class Animation;

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

CLASS( Game, common::Task )

	static constexpr size_t SCROLL_DURATION_MS = 100;

	Game( ::game::backend::State* state, ::ui::ui_handler_t on_start = 0, ::ui::ui_handler_t on_cancel = 0 );
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

	const size_t GetMapWidth() const;
	const size_t GetMapHeight() const;
	const std::string& GetMapFilename() const;
	const std::string& GetMapLastDirectory() const;

	tile::TileManager* GetTM() const;
	unit::UnitManager* GetUM() const;
	base::BaseManager* GetBM() const;
	sprite::InstancedSpriteManager* GetISM() const;
	text::InstancedTextManager* GetITM() const;

	types::texture::Texture* GetSourceTexture( const resource::resource_t res );
	sprite::InstancedSprite* GetTerrainInstancedSprite( const ::game::backend::map::sprite_actor_t& actor );

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

	// TODO: move this to .cpp
	// structures received from game thread
	struct map_data_t {
		map_data_t();
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
		std::string filename;
		std::string last_directory;
	};

	map_data_t m_map_data = {};

	types::texture::Texture* GetTerrainTexture() const;

	void SetEditorTool( ::game::backend::map_editor::tool_type_t tool );
	const ::game::backend::map_editor::tool_type_t GetEditorTool() const;

	void SetEditorBrush( ::game::backend::map_editor::brush_type_t editor_brush );
	const ::game::backend::map_editor::brush_type_t GetEditorBrush() const;

	const types::Vec3 GetCloserCoords( const types::Vec3& coords, const types::Vec3& ref_coords ) const;

	Slot* GetSlot( const size_t index ) const;

private:

	faction::FactionManager* m_fm = nullptr;
	tile::TileManager* m_tm = nullptr;
	unit::UnitManager* m_um = nullptr;
	base::BaseManager* m_bm = nullptr;
	sprite::InstancedSpriteManager* m_ism = nullptr;
	text::InstancedTextManager* m_itm = nullptr;

	size_t m_slot_index = 0;
	bool m_is_turn_active = false;
	::game::backend::turn::turn_status_t m_turn_status = ::game::backend::turn::TS_PLEASE_WAIT;
	size_t m_turn_id = 0;

	::game::backend::map_editor::tool_type_t m_editor_tool = ::game::backend::map_editor::TT_NONE;
	::game::backend::map_editor::brush_type_t m_editor_brush = ::game::backend::map_editor::BT_NONE;

	void UpdateMapData( const types::Vec2< size_t >& map_size );

	void DefineSlot(
		const size_t slot_index,
		faction::Faction* faction
	);
	void DefineAnimation( const ::game::backend::animation::Def* def );
	void ShowAnimation( AnimationDef* def, const size_t animation_id, const ::types::Vec3& render_coords );

	void ProcessRequest( const ::game::FrontendRequest* request );
	void SendBackendRequest( const ::game::BackendRequest* request );

	bool m_is_initialized = false;
	void Initialize(
		const types::Vec2< size_t >& map_size,
		types::texture::Texture* terrain_texture,
		types::mesh::Render* terrain_mesh,
		types::mesh::Data* terrain_data_mesh,
		const std::unordered_map< std::string, ::game::backend::map::sprite_actor_t >& sprite_actors,
		const std::unordered_map< size_t, std::pair< std::string, ::types::Vec3 > >& sprite_instances,
		const std::vector< ::game::backend::map::tile::Tile >* tiles,
		const std::vector< ::game::backend::map::tile::TileState >* tile_states
	);
	void Deinitialize();

	::ui::ui_handler_t m_on_start;
	::ui::ui_handler_t m_on_cancel;

	void SetCameraPosition( const ::types::Vec3 camera_position );

	::game::backend::State* m_state = nullptr;

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
	::game::backend::map_editor::draw_mode_t m_editor_draw_mode = ::game::backend::map_editor::DM_NONE;
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

	const bool m_is_map_editing_allowed = false;

	// UI stuff

	struct {
		ui::style::Theme* theme = nullptr;
		ui::BottomBar* bottom_bar = nullptr;
	} m_ui;

	bool m_is_resize_handler_set = false;

	void SelectTileAtPoint( const ::game::backend::tile_query_purpose_t tile_query_purpose, const size_t x, const size_t y );
	void SelectTileOrUnit( tile::Tile* tile, const size_t selected_unit_id = 0 );
	void DeselectTileOrUnit();

private:
	friend class ui::UnitsList;

	struct {
		std::unordered_map< resource::resource_t, types::texture::Texture* > source;
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

	const types::Vec2< float > GetTileWindowCoordinates( const tile::Tile* tile ) const;

	void ScrollTo( const ::types::Vec3& target );
	void ScrollToTile( const tile::Tile* tile, bool center_on_tile );

	struct tile_at_result_t {
		bool is_set = false;
		types::Vec2< size_t > tile_pos;
	};

	// tile request stuff
	rr::id_t m_tile_at_request_id = 0;
	::game::backend::tile_query_purpose_t m_tile_at_query_purpose = ::game::backend::TQP_NONE;

	void CancelTileAtRequest();
	void GetTileAtScreenCoords( const ::game::backend::tile_query_purpose_t tile_query_purpose, const size_t screen_x, const size_t screen_inverse_y ); // async, y needs to be upside down
	const bool IsTileAtRequestPending() const;
	const tile_at_result_t GetTileAtScreenCoordsResult();

	// minimap stuff
	rr::id_t m_minimap_texture_request_id = 0;
	void GetMinimapTexture( scene::Camera* camera, const ::types::Vec2< size_t > texture_dimensions );
	types::texture::Texture* GetMinimapTextureResult();
	void UpdateMinimap();

	void ResetMapState();

	struct {
		common::mt_id_t ping = 0;
		common::mt_id_t init = 0;
		common::mt_id_t get_map_data = 0;
		common::mt_id_t reset = 0;
		std::unordered_set< common::mt_id_t > select_tile = {};
		common::mt_id_t save_map = 0;
		common::mt_id_t edit_map = 0;
		common::mt_id_t chat = 0;
		common::mt_id_t get_frontend_requests = 0;
		common::mt_id_t send_backend_requests = 0;
#ifdef DEBUG
		common::mt_id_t save_dump = 0;
		// init will be used for loading dump
#endif
	} m_mt_ids = {};

	std::unordered_map< size_t, Slot* > m_slots = {};
	std::unordered_map< std::string, AnimationDef* > m_animationdefs = {};
	std::unordered_map< size_t, Animation* > m_animations;

	void CancelRequests();
	void CancelGame();

	std::vector< ::game::BackendRequest > m_pending_backend_requests = {};

	const float GetFixedX( const float x ) const;
	const float GetCloserX( const float x, const float ref_x ) const;

	void ShowTileSelector();
	void HideTileSelector();
	void RenderTile( tile::Tile* tile, const unit::Unit* selected_unit );

	void SendAnimationFinished( const size_t animation_id );

private:
	friend class unit::UnitManager;
	friend class base::BaseManager;

	const bool IsTurnActive() const;

	const size_t GetMySlotIndex() const;

	void SetSelectedTile( tile::Tile* tile );
	void RefreshSelectedTile( unit::Unit* selected_unit );
	void RefreshSelectedTileIf( tile::Tile* if_tile, unit::Unit* selected_unit );
	void ScrollToSelectedTile( const bool center_on_tile );
	void SelectUnitOrSelectedTile( unit::Unit* selected_unit );
	unit::Unit* GetSelectedTileMostImportantUnit() const;

};

}
}
