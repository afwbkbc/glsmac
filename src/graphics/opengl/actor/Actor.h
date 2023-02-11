#pragma once

#include "base/Base.h"

#include "scene/actor/Actor.h"
#include "scene/Camera.h"
#include "../shader_program/ShaderProgram.h"

#include "types/Vec3.h"

using namespace types;
using namespace scene;

namespace graphics {
namespace opengl {

CLASS( Actor, base::Base )
	
	Actor( actor::Actor *actor );
	~Actor();
	
	virtual void LoadMesh() {};
	virtual void LoadTexture() {};
	virtual void UnloadMesh() {};
	virtual void UnloadTexture() {};
	virtual bool MeshReloadNeeded() { return false; }
	virtual bool TextureReloadNeeded() { return false; }
	
	virtual void Draw( shader_program::ShaderProgram *shader_program, Camera *camera = nullptr ) = 0;
	actor::Actor *GetActor() const {
		return m_actor;
	}
	void UnlinkActor() {
		m_actor = nullptr;
	}
	const Vec3& GetPosition() const;
	void SetPosition( const Vec3& position );
	
	virtual void OnResize() {};
	
protected:
	actor::Actor *m_actor;
	
	Vec3 m_position;
	
};

} /* namespace opengl */
} /* namespace graphics */
