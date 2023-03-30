#pragma once

#include <vector>
#include <unordered_map>

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
	
	// GetAngle(), SetAngle(), etc
#define _XYZ_SETTER( _name ) \
	const types::Vec3& Get##_name() const;\
	const float Get##_name##X() const;\
	const float Get##_name##Y() const;\
	const float Get##_name##Z() const;\
	void Set##_name( const types::Vec3 & value );\
	void Set##_name##X( const float value );\
	void Set##_name##Y( const float value );\
	void Set##_name##Z( const float value );

	_XYZ_SETTER( Position );
	_XYZ_SETTER( Angle );

#undef _XYZ_SETTER

	typedef std::vector< types::Matrix44 > matrices_t;
	const matrices_t& GetInstanceMatrices();
	types::Matrix44 & GetWorldMatrix();
	void GenerateInstanceMatrices( matrices_t* out_matrices, scene::Camera* camera );
	
	void UpdateWorldMatrix();
	void UpdatePosition();
	void UpdateMatrix();
	
	Sprite* GetSpriteActor() const;
	Mesh* GetMeshActor() const;

	typedef size_t instance_id_t;
	const instance_id_t AddInstance( const types::Vec3& position, const types::Vec3& angle = { 0.0f, 0.0f, 0.0f } );
	void SetInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle = { 0.0f, 0.0f, 0.0f } );
	void RemoveInstance( const instance_id_t instance_id );
	
	// instanced actors don't have normal z position so need to keep global z index separately
	const float GetZIndex() const;
	void SetZIndex( const float z_index );
	
	virtual const types::Buffer Serialize() const;
	virtual void Unserialize( types::Buffer buf );
	
private:
	Actor* m_actor = nullptr;

	// used for draw ordering
	float m_z_index = 0.0f;
	
	matrices_t m_instance_matrices = {};
	
	struct instanced_matrices_t {
		types::Matrix44 translate;
		types::Matrix44 matrix;
	};
	
	typedef struct {
		types::Vec3 position;
		types::Vec3 angle;
		std::vector< instanced_matrices_t > matrices;
		bool need_update;
	} instance_t;
	
	instance_id_t m_next_instance_id = 0;
	std::unordered_map< instance_id_t, instance_t > m_instances = {};
	
	const scene::Scene::instance_positions_t* GetWorldInstancePositions();
	
	void UpdateInstance( instance_t& instance );
};

}
}

#include "Sprite.h"
#include "Mesh.h"
