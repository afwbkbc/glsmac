#pragma once

#include <unordered_map>

#include "Actor.h"

#include "types/mesh/Simple.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "types/Color.h"
#include "types/Texture.h"

using namespace types;

namespace scene {
namespace actor {

CLASS( Mesh, Actor )

	// mesh can also be set in constructor of derived class, but it MUST be set
	Mesh( const std::string &name, const mesh::Mesh* mesh = nullptr, const type_t type = TYPE_MESH );
	~Mesh();
	
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
	
	void SetRenderFlags( const render_flag_t render_flags );
	const render_flag_t GetRenderFlags() const;
	void SetTintColor( const Color tint_color );
	const Color& GetTintColor();
	
	// data mesh stuff
	
	void SetDataMesh( const mesh::Data* data_mesh );
	
	struct data_request_t {
		size_t screen_x;
		size_t screen_inverse_y;
		bool is_processed;
		mesh::Data::data_t result;
	};
	typedef size_t data_request_id_t;
	typedef std::unordered_map< data_request_id_t, data_request_t > data_requests_t;
	
	// make sure to call these from same thread only
	data_request_id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	std::pair< bool, mesh::Data::data_t > GetDataResponse( const data_request_id_t id );
	void CancelDataRequest( const data_request_id_t id );
	data_requests_t* GetDataRequests();
	const data_request_id_t GetLastDataRequestId() const;
	
protected:
	const mesh::Mesh* m_mesh = nullptr;
	const Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	render_flag_t m_render_flags = RF_NONE;
	Color m_tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	// data mesh stuff
	const mesh::Data* m_data_mesh = nullptr;
	data_requests_t m_data_requests = {};
	data_request_id_t m_last_data_request_id = 0;
};

} /* namespace scene */
} /* namespace actor */
