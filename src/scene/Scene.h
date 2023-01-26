#pragma once

#include <vector>

#include "base/Base.h"

#include "actor/Actor.h"
#include "Camera.h"
#include "types/Texture.h"

namespace scene {

enum scene_type_t {
	SCENE_TYPE_NONE,
	SCENE_TYPE_SIMPLE2D,
	SCENE_TYPE_ORTHO,
	SCENE_TYPE_TEXT,
	SCENE_TYPE_PERSP,
};

CLASS( Scene, base::Base )
	Scene( const std::string &name, const scene_type_t scene_type ) : m_scene_type( scene_type ) {
		m_name = name;
	}
	~Scene();
	void AddActor( actor::Actor *actor );
	void RemoveActor( actor::Actor *actor );
	std::vector<actor::Actor *> *GetActors() {
		return &m_actors;
	}
	const scene_type_t GetType() const {
		return m_scene_type;
	}
	void SetCamera( Camera *camera );
	Camera * GetCamera();
	void SetSimple2DCamera();
	void SetSkyboxTexture( types::Texture *skybox_texture );
	types::Texture *GetSkyboxTexture();
protected:
	friend class scene::Camera;

	Camera *m_local_camera = nullptr;
	
	const scene_type_t m_scene_type = SCENE_TYPE_NONE;

	std::vector<actor::Actor *> m_actors = {};

	Camera *m_camera = nullptr;
	types::Texture *m_skybox_texture = nullptr;
};

} /* namespace scene */
