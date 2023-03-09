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
	Mesh( const std::string &name, const mesh::Mesh* mesh = nullptr, const Actor::type_t type = Actor::TYPE_MESH );
	virtual ~Mesh();
	
	void SetMesh( const mesh::Mesh* mesh );
	
	const mesh::Mesh *GetMesh() const;
	const mesh::Data* GetDataMesh() const;
	
	void SetTexture( const Texture* texture );
	const Texture* GetTexture() const;
	
	typedef uint32_t render_flag_t;
	static constexpr render_flag_t RF_NONE = 0;
	static constexpr render_flag_t RF_USE_TINT = 1 << 0;
	static constexpr render_flag_t RF_IGNORE_LIGHTING = 1 << 1;
	static constexpr render_flag_t RF_IGNORE_CAMERA = 1 << 2;
	static constexpr render_flag_t RF_IGNORE_DEPTH = 1 << 3;
	static constexpr render_flag_t RF_USE_COORDINATE_LIMITS = 1 << 4;
	
	void SetRenderFlags( const render_flag_t render_flags );
	const render_flag_t GetRenderFlags() const;
	void SetTintColor( const Color tint_color );
	const Color& GetTintColor() const;
	typedef std::pair< Vec3, Vec3 > coordinate_limits_t;
	void SetCoordinateLimits( const coordinate_limits_t limits );
	const coordinate_limits_t& GetCoordinateLimits() const;
	
	
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
	const Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	render_flag_t m_render_flags = RF_NONE;
	Color m_tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	coordinate_limits_t m_coordinate_limits = {};
	
	// data mesh stuff
	const mesh::Data* m_data_mesh = nullptr;
};

} /* namespace scene */
} /* namespace actor */
