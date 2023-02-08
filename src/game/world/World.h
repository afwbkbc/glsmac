#pragma once

#include "base/Task.h"

#include "../Settings.h"

#include "types/Texture.h"
#include "types/Mesh.h"
#include "scene/actor/Mesh.h"
#include "scene/Scene.h"
#include "util/Clamper.h"
#include "util/Random.h"
#include "map/Map.h"

#include "ui/event/UIEventHandler.h"

#include "ui/style/Theme.h"
#include "ui/bottom_bar/BottomBar.h"

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
	Light* m_light = nullptr;
	Scene* m_world_scene = nullptr;
	struct {
		util::Clamper<float> x;
		util::Clamper<float> y;
	} m_clamp = {};
	
	struct {
		Vec2< size_t > min;
		Vec2< size_t > max;
		Vec2< float > ratio;
		size_t width;
		size_t height;
		float aspect_ratio;
		float window_aspect_ratio;
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
	
	// UI stuff
	
	struct {
		ui::Theme theme;
		ui::BottomBar* bottom_bar = nullptr;
	} m_ui;
	
	void ReturnToMainMenu();
	
};

}
}
