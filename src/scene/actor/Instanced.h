#pragma once

#include <vector>

#include "Actor.h"

#include "types/Matrix44.h"
#include "scene/Scene.h"
#include "scene/Camera.h"

namespace scene {
namespace actor {

class Sprite;
class Mesh;
	
CLASS( Instanced, Actor )

	Instanced( Actor* actor );
	virtual ~Instanced();
	
	typedef std::vector< types::Matrix44 > world_matrices_t;
	const world_matrices_t& GetWorldMatrices();
	types::Matrix44 & GetWorldMatrix();
	world_matrices_t GenerateWorldMatrices( scene::Camera* camera );
	
	void UpdateWorldMatrix();
	void UpdatePosition();
	void UpdateMatrix();
	
	Sprite* GetSpriteActor() const;
	Mesh* GetMeshActor() const;

private:
	Actor* m_actor = nullptr;

	world_matrices_t m_world_matrices = {};
	
	struct instanced_matrices_t {
		types::Matrix44 translate;
		types::Matrix44 matrix;
		types::Matrix44 world;
	};
	
	std::vector< instanced_matrices_t > m_instance_matrices = {};
	
	const scene::Scene::instances_t* GetInstances();
};

}
}

#include "Sprite.h"
#include "Mesh.h"
