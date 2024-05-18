#pragma once

#include <vector>
#include <map>

#include "Actor.h"

#include "scene/Types.h"

#include "types/Matrix44.h"

namespace scene {

class Camera;

namespace actor {

class Sprite;
class Mesh;

CLASS( Instanced, Actor )

	Instanced( Actor* actor );
	virtual ~Instanced();

	// GetAngle(), SetAngle(), etc
#define _XYZ_SETTER( _name ) \
    const types::Vec3& Get##_name() const override;\
    const float Get##_name##X() const override;\
    const float Get##_name##Y() const override;\
    const float Get##_name##Z() const override;\
    void Set##_name( const types::Vec3 & value ) override;\
    void Set##_name##X( const float value ) override;\
    void Set##_name##Y( const float value ) override;\
    void Set##_name##Z( const float value ) override;

	_XYZ_SETTER( Position );
	_XYZ_SETTER( Angle );

#undef _XYZ_SETTER

	typedef std::vector< types::Matrix44 > matrices_t;
	const matrices_t& GetInstanceMatrices();
	types::Matrix44& GetWorldMatrix() override;
	void GenerateInstanceMatrices( matrices_t* out_matrices, scene::Camera* camera );

	void UpdateWorldMatrix() override;
	void UpdatePosition() override;
	void UpdateMatrix() override;
	
	Sprite* GetSpriteActor() const;
	Mesh* GetMeshActor() const;

	typedef size_t instance_id_t;
	const instance_id_t AddInstance( const types::Vec3& position, const types::Vec3& angle = {} );
	void SetInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle = {} );
	void UpdateInstance( const instance_id_t instance_id, const types::Vec3& position, const types::Vec3& angle = {} );
	void RemoveInstance( const instance_id_t instance_id );
	const bool HasInstance( const instance_id_t instance_id );

	// instanced actors don't have normal z position so need to keep global z index separately
	const float GetZIndex() const;
	void SetZIndex( const float z_index );

	virtual const types::Buffer Serialize() const override;
	virtual void Unserialize( types::Buffer buf ) override;

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

	instance_id_t m_next_instance_id = 1;
	std::map< instance_id_t, instance_t > m_instances = {};

	const scene::instance_positions_t* GetWorldInstancePositions();

	void UpdateMatrixForInstance( instance_t& instance );
};

}
}
