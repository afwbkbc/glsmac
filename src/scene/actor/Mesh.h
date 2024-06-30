#pragma once

#include <optional>

#include "Actor.h"
#include "common/RRAware.h"

#include "types/Color.h"

#include "rr/GetData.h"

namespace types {
namespace texture {
class Texture;
}
namespace mesh {
class Mesh;
class Data;
}
}

namespace scene {

class Camera;

namespace actor {

CLASS2( Mesh, Actor, common::RRAware )

	// mesh can also be set in constructor of derived class, but it MUST be set
	Mesh( const std::string& name, const types::mesh::Mesh* mesh = nullptr, const type_t type = Actor::TYPE_MESH );
	virtual ~Mesh();

	void SetMesh( const types::mesh::Mesh* mesh );

	const types::mesh::Mesh* GetMesh() const;
	const types::mesh::Data* GetDataMesh() const;

	void SetTexture( types::texture::Texture* texture );
	types::texture::Texture* GetTexture() const;

	void SetTintColor( const types::Color tint_color );
	const types::Color& GetTintColor() const;

	void SetDataMesh( const types::mesh::Data* data_mesh );

	// data mesh stuff
	typedef std::pair< bool, std::optional< rr::GetData::data_t > > data_response_t;
	rr::id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	data_response_t GetDataResponse( const rr::id_t request_id );
	void CancelDataRequest( const rr::id_t request_id );

	// capture rendered mesh to texture
	rr::id_t CaptureToTexture( scene::Camera* camera, const types::Vec2< size_t > texture_dimensions );
	types::texture::Texture* GetCaptureToTextureResponse( const rr::id_t request_id );
	void CancelCaptureToTextureRequest( const rr::id_t request_id );

protected:
	const types::mesh::Mesh* m_mesh = nullptr;
	types::texture::Texture* m_texture = nullptr;

	types::Color m_tint_color = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};

	// data mesh stuff
	const types::mesh::Data* m_data_mesh = nullptr;
};

}
}
