#pragma once

#include <vector>
#include <unordered_set>

#include "common/Common.h"

#include "Types.h"

namespace types::texture {
class Texture;
}

namespace scene {

class Camera;
class Light;

namespace actor {
class Actor;
}

CLASS( Scene, common::Class )
	Scene( const std::string& name, const scene_type_t scene_type )
		: m_scene_type( scene_type ) {
		m_name = name;
	}
	~Scene();
	void AddActor( actor::Actor* actor );
	void RemoveActor( actor::Actor* actor );
	std::vector< actor::Actor* >* GetActors() {
		return &m_actors;
	}
	const scene_type_t GetType() const {
		return m_scene_type;
	}
	void SetCamera( Camera* camera );
	Camera* GetCamera() const;
	void AddLight( Light* light );
	std::unordered_set< Light* >* GetLights();

	void SetWorldInstancePositions( const instance_positions_t& world_instance_positions );
	const instance_positions_t& GetWorldInstancePositions() const;

protected:
	friend class scene::Camera;

	Camera* m_local_camera = nullptr;

	const scene_type_t m_scene_type = SCENE_TYPE_NONE;

	std::vector< actor::Actor* > m_actors = {};

	Camera* m_camera = nullptr;
	std::unordered_set< Light* > m_lights = {};
	types::texture::Texture* m_skybox_texture = nullptr;

	instance_positions_t m_game_instance_positions = {
		{
			0.0f,
			0.0f,
			0.0f
		}
	}; // only 1 instance by default
};

}
