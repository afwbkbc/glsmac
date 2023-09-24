#pragma once

#include <unordered_map>
#include <optional>

#include "Actor.h"
#include "base/RRAware.h"

#include "types/mesh/Simple.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "types/Color.h"
#include "types/Texture.h"
#include "scene/Camera.h"

#include "rr/GetData.h"

using namespace types;

namespace scene {
namespace actor {

CLASS2( Mesh, Actor, RRAware )

	// mesh can also be set in constructor of derived class, but it MUST be set
	Mesh( const std::string& name, const mesh::Mesh* mesh = nullptr, const Actor::type_t type = Actor::TYPE_MESH );
	virtual ~Mesh();

	void SetMesh( const mesh::Mesh* mesh );

	const mesh::Mesh* GetMesh() const;
	const mesh::Data* GetDataMesh() const;

	void SetTexture( Texture* texture );
	Texture* GetTexture() const;

	void SetTintColor( const Color tint_color );
	const Color& GetTintColor() const;

	void SetDataMesh( const mesh::Data* data_mesh );

	// data mesh stuff
	typedef std::pair< bool, std::optional< rr::GetData::data_t > > data_response_t;
	rr::id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	data_response_t GetDataResponse( const rr::id_t request_id );
	void CancelDataRequest( const rr::id_t request_id );

	// capture rendered mesh to texture
	rr::id_t CaptureToTexture( scene::Camera* camera, const Vec2< size_t > texture_dimensions );
	Texture* GetCaptureToTextureResponse( const rr::id_t request_id );
	void CancelCaptureToTextureRequest( const rr::id_t request_id );

protected:
	const mesh::Mesh* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	Color m_tint_color = {
		1.0f,
		1.0f,
		1.0f,
		1.0f
	};

	// data mesh stuff
	const mesh::Data* m_data_mesh = nullptr;
};

} /* namespace scene */
} /* namespace actor */
