#pragma once

#include <unordered_set>

#include "base/Task.h"

#include "../Settings.h"

#include "scene/Scene.h"
#include "util/Clamper.h"
#include "util/Random.h"
#include "map/Map.h"

#include "ui/event/UIEventHandler.h"

#include "ui/style/Theme.h"
#include "ui/bottom_bar/BottomBar.h"

#include "actor/TileSelection.h"

using namespace ui;
namespace ui {
	using namespace event;
}

using namespace util;
using namespace scene;

namespace game {
namespace world {
	
using namespace map;

CLASS( World, base::Task )
	
	World( const Settings& settings );
	~World();
	
	void Start();
	void Stop();
	void Iterate();

	struct consts_t {
		const struct {
			const uint16_t scroll_time_ms = 100;
			const uint16_t scroll_step_ms = 2;
			const uint8_t scroll_steps = scroll_time_ms / scroll_step_ms;
		} map_scroll;
	};
	static const consts_t s_consts;
	
protected:

private:
	
	void SetCameraPosition( const Vec3 camera_position );
	
	// game settings (tmp)
	const Settings m_settings;
	
	// seed needs to be consistent during session (to prevent save-scumming and for easier reproducing of bugs)
	Random* m_random = nullptr;
	
	// map rendering stuff
	Map* m_map = nullptr;
	Vec3 m_camera_position; // { x, y, zoom }
	Vec3 m_camera_angle;
	Camera* m_camera = nullptr;
	Light* m_light_a = nullptr; // Alpha Centauri A
	Light* m_light_b = nullptr; // Alpha Centauri B
	Scene* m_world_scene = nullptr;
	struct {
		util::Clamper<float> x;
		util::Clamper<float> y;
	} m_clamp = {};
	
	struct {
		util::Timer timer;
		Vec3 step;
		Vec3 target_position;
		uint8_t steps_left;
	} m_scrolling;
	
	struct {
		Vec2< size_t > min;
		Vec2< size_t > max;
		size_t bottom_bar_overlap;
		Vec2< float > ratio;
		size_t width;
		size_t height;
		float aspect_ratio;
		float viewport_aspect_ratio;
	} m_viewport;
	struct {
		Vec3 min;
		Vec3 max;
	} m_camera_range;
	
	struct {
		const UIEventHandler* keydown;
		const UIEventHandler* mousedown;
		const UIEventHandler* mousemove;
		const UIEventHandler* mouseup;
		const UIEventHandler* mousescroll;
	} m_handlers;
	bool m_is_dragging = false;
	bool m_is_rotating = false;
	Vec2<float> m_last_drag_position;
	Vec2<float> m_last_rotate_position;
	void UpdateViewport();
	void UpdateCameraPosition();
	void UpdateCameraAngle();
	void UpdateCameraScale();
	void UpdateCameraRange();
	void UpdateMapInstances();
	void UpdateUICamera();
	
	// UI stuff
	
	struct {
		ui::style::Theme theme;
		ui::BottomBar* bottom_bar = nullptr;
	} m_ui;
	
	void ReturnToMainMenu();
	
	// tiles stuff
	void SelectTileAtPoint( const size_t x, const size_t y );
	void SelectTile( const Map::tile_info_t& tile_info );
	void DeselectTile();
	
	struct {
		actor::TileSelection* tile_selection = nullptr;
	} m_actors;
	// some additional management of world actors such as calling Iterate()
	std::unordered_set< actor::Actor* > m_actors_vec = {};
	void AddActor( actor::Actor* actor );
	void RemoveActor( actor::Actor* actor );
	
	void ScrollTo( const Vec3& target );
	void ScrollToXY( const float x, const float y );
	void ScrollToTile( const Map::tile_state_t* ts );
	void CenterAtTile( const Map::tile_state_t* ts );
};

}
}
