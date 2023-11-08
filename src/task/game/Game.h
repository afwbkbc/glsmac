#pragma once

#include <unordered_set>
#include <unordered_map>

#include "base/Task.h"

#include "Types.h"

#include "base/MTModule.h"

#include "game/State.h"

#include "scene/Scene.h"
#include "scene/actor/Instanced.h"

#include "types/Texture.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"

#include "util/Clamper.h"
#include "util/Random.h"
#include "util/Scroller.h"

#include "game/map_editor/MapEditor.h"

#include "../../ui/UI.h"
#include "ui/event/UIEventHandler.h"
#include "ui/style/Theme.h"
#include "ui/bottom_bar/BottomBar.h"

#include "actor/TileSelection.h"
#include "game/Game.h"
#include "game/Event.h"

#include "game/map/Consts.h"
#include "game/map_editor/MapEditor.h"

using namespace ui;
namespace ui {
using namespace event;
}

using namespace util;
using namespace scene;

namespace task {
namespace game {

CLASS( Game, base::Task )

	Game( ::game::State* state, ui_handler_t on_start = 0, ui_handler_t on_cancel = 0 );
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
				const Vec2< float > speed = {
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

	scene::actor::Instanced* GetTerrainSpriteActor( const std::string& name, const ::game::map::Consts::pcx_texture_coordinates_t& tex_coords, const float z_index );
	scene::actor::Instanced* GetTerrainSpriteActorByKey( const std::string& key ); // actor must already exist

	void CenterAtCoordinatePercents( const Vec2< float > position_percents );

	void SmoothScroll( const float scroll_value );
	void SmoothScroll( const Vec2< float > position, const float scroll_value );

	util::Random* GetRandom() const;

	void CloseMenus();

	typedef std::function< void() > f_exit_game;
	f_exit_game m_on_game_exit = nullptr;
	void ExitGame( const f_exit_game on_game_exit );
	void ReturnToMainMenu( const std::string reason = "" );

	const size_t GetBottomBarMiddleHeight() const;
	const size_t GetViewportHeight() const;

	void AddMessage( const std::string& text );
	void SendChatMessage( const std::string& text );

	void LoadMap( const std::string& path );
	void SaveMap( const std::string& path );

	void ConfirmExit( ::ui::ui_handler_t on_confirm );

	types::Texture* GetTerrainTexture() const;

	void SetEditorTool( ::game::map_editor::MapEditor::tool_type_t tool );
	const ::game::map_editor::MapEditor::tool_type_t GetEditorTool() const;

	void SetEditorBrush( ::game::map_editor::MapEditor::brush_type_t editor_brush );
	const ::game::map_editor::MapEditor::brush_type_t GetEditorBrush() const;

protected:

private:

	::game::map_editor::MapEditor::tool_type_t m_editor_tool = ::game::map_editor::MapEditor::TT_NONE;
	::game::map_editor::MapEditor::brush_type_t m_editor_brush = ::game::map_editor::MapEditor::BT_NONE;

	void UpdateMapData( const types::Vec2< size_t >& map_size );

	void ProcessEvent( const ::game::Event& event );

	bool m_is_initialized = false;
	void Initialize(
		types::Texture* terrain_texture,
		types::mesh::Render* terrain_mesh,
		types::mesh::Data* terrain_data_mesh,
		const std::unordered_map< std::string, ::game::map::Map::sprite_actor_t >& sprite_actors,
		const std::unordered_map< size_t, std::pair< std::string, Vec3 > >& sprite_instances
	);
	void Deinitialize();

	ui_handler_t m_on_start;
	ui_handler_t m_on_cancel;

	void SetCameraPosition( const Vec3 camera_position );

	::game::State* m_state = nullptr;

	// seed needs to be consistent during session (to prevent save-scumming and for easier reproducing of bugs)
	Random* m_random = nullptr;

	// map rendering stuff
	Vec3 m_camera_position; // { x, y, zoom }
	Vec3 m_camera_angle;
	Camera* m_camera = nullptr;
	Light* m_light_a = nullptr; // Alpha Centauri A
	Light* m_light_b = nullptr; // Alpha Centauri B
	Scene* m_world_scene = nullptr;

	bool m_is_editing_mode = false;
	::game::map_editor::MapEditor::draw_mode_t m_editor_draw_mode = ::game::map_editor::MapEditor::DM_NONE;
	util::Timer m_editing_draw_timer;

	struct {
		util::Clamper< float > x;
		util::Clamper< float > y;
	} m_clamp = {};

	util::Scroller< types::Vec3 > m_scroller;

	struct {
		bool is_dragging = false;
		Vec2< float > last_drag_position;
		struct {
			util::Timer timer;
			Vec2< float > speed = {
				0,
				0
			};
		} edge_scrolling;
		Vec2< float > last_mouse_position;
		float key_zooming = 0;
	} m_map_control = {};

	struct {
		size_t window_width;
		size_t window_height;
		Vec2< size_t > min;
		Vec2< size_t > max;
		size_t bottom_bar_overlap;
		Vec2< float > ratio;
		size_t width;
		size_t height;
		float aspect_ratio;
		float window_aspect_ratio;
		bool is_fullscreen;
	} m_viewport;
	struct {
		Vec3 min;
		Vec3 max;
	} m_camera_range;

	// shift x to center instance when needed
	const float GetFixedX( float x ) const;
	void FixCameraX();

	struct {
		const UIEventHandler* keydown_before = nullptr;
		const UIEventHandler* keydown_after = nullptr;
		const UIEventHandler* keyup = nullptr;
		const UIEventHandler* mousedown = nullptr;
		const UIEventHandler* mousemove = nullptr;
		const UIEventHandler* mouseup = nullptr;
		const UIEventHandler* mousescroll = nullptr;
	} m_handlers;
	void UpdateViewport();
	void UpdateCameraPosition();
	void UpdateCameraAngle();
	void UpdateCameraScale();
	void UpdateCameraRange();
	void UpdateMapInstances();
	void UpdateUICamera();

	// structures received from game thread
	struct map_data_t {
		size_t width = 0;
		size_t height = 0;
		struct {
			Vec2< float > min = {};
			Vec2< float > max = {};
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
				util::FS::GetPathSeparator() +
				::game::map::s_consts.fs.default_map_directory;
	};

	const bool m_is_map_editing_allowed = false;

	tile_data_t m_selected_tile_data = {};
	map_data_t m_map_data = {};

	// UI stuff

	struct {
		ui::style::Theme theme;
		ui::BottomBar* bottom_bar = nullptr;
	} m_ui;

	bool m_is_resize_handler_set = false;

	// tiles stuff
	void SelectTileAtPoint( const size_t x, const size_t y );
	void SelectTile( const tile_data_t& tile_data );
	void DeselectTile();

	struct {
		struct {
			types::Texture* ter1_pcx = nullptr;
		} source;
		types::Texture* terrain = nullptr;
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

	const Vec2< float > GetTileWindowCoordinates( const Vec3& tile_coords );

	void ScrollTo( const Vec3& target );
	void ScrollToTile( const tile_data_t& tile_data );

	struct tile_at_result_t {
		bool is_set = false;
		types::Vec2< size_t > tile_pos;
	};

	// tile request stuff
	rr::id_t m_tile_at_request_id = 0;
	void CancelTileAtRequest();
	void GetTileAtScreenCoords( const size_t screen_x, const size_t screen_inverse_y ); // async, y needs to be upside down
	const bool IsTileAtRequestPending() const;
	const tile_at_result_t GetTileAtScreenCoordsResult();

	void GetTileAtCoords( const Vec2< size_t >& tile_pos, const ::game::tile_direction_t tile_direction = ::game::TD_NONE );
	tile_data_t GetTileAtCoordsResult();

	// minimap stuff
	rr::id_t m_minimap_texture_request_id = 0;
	void GetMinimapTexture( scene::Camera* camera, const Vec2< size_t > texture_dimensions );
	Texture* GetMinimapTextureResult();
	void UpdateMinimap();

	void ResetMapState();

	struct {
		mt_id_t ping = 0;
		mt_id_t init = 0;
		mt_id_t get_map_data = 0;
		mt_id_t reset = 0;
		mt_id_t select_tile = 0;
		mt_id_t save_map = 0;
		mt_id_t edit_map = 0;
		mt_id_t chat = 0;
		mt_id_t get_events = 0;
#ifdef DEBUG
		mt_id_t save_dump = 0;
		// init will be used for loading dump
#endif
	} m_mt_ids = {};

	struct instanced_sprite_t {
		std::string name;
		::game::map::Consts::pcx_texture_coordinates_t tex_coords;
		scene::actor::Instanced* actor;
	};
	std::unordered_map< std::string, instanced_sprite_t > m_instanced_sprites = {};

	void CancelRequests();
	void CancelGame();
};

}
}
