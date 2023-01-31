#pragma once

#include "base/Task.h"

#include "../Settings.h"

#include "ui/event/UIEventHandler.h"

#include "types/Texture.h"
#include "types/Mesh.h"

#include "scene/actor/Mesh.h"
#include "scene/Scene.h"

#include "util/Clamper.h"

#include "map/Map.h"

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
	
	void Start();
	void Stop();
	void Iterate();
	
	void SetCameraPosition( const Vec3 camera_position );
	
protected:

private:
	
	Map* m_map = nullptr;
	
	Vec3 m_camera_position; // { x, y, zoom }
	Vec3 m_camera_angle;
	
	const Settings m_settings;

	Scene* m_world_scene = nullptr;

	//vector<types::Texture*> m_textures = {};
	
	//mesh::Mesh* m_mesh = nullptr;
	//vector<actor::Mesh*> m_test_actors = {};
	
	Camera* m_camera = nullptr;
	
	Light* m_light = nullptr;
	
	struct {
		util::Clamper<float> x;
		util::Clamper<float> y;
	} m_clamp = {};
	
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
	
	void UpdateCameraPosition();
	void UpdateCameraAngle();
	void UpdateCameraScale();
	
	void ReturnToMainMenu();
	
};

}
}
