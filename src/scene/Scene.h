#pragma once

#include <vector>

#include "base/Base.h"

#include "actor/Actor.h"
#include "Camera.h"
#include "types/Texture.h"

namespace scene {

enum scene_type_t {
	SCENE_TYPE_ORTHO,
	SCENE_TYPE_TEXT,
	SCENE_TYPE_PERSP,
};

MAJOR_CLASS( Scene, base::Base )
	Scene( const std::string &name, const scene_type_t scene_type ) : m_scene_type( scene_type ) {
		m_name = name;
	} ;
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
	void SetSkyboxTexture( types::Texture *skybox_texture );
	types::Texture *GetSkyboxTexture();
protected:
	friend class scene::Camera;

	const scene_type_t m_scene_type;

	std::vector<actor::Actor *> m_actors;

	Camera *m_camera = NULL;
	types::Texture *m_skybox_texture = NULL;
};

} /* namespace scene */
