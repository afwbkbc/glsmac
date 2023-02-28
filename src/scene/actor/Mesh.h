#pragma once

#include <unordered_map>
#include <optional>

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
	
	void SetDataMesh( const mesh::Data* data_mesh );
	
	// render loop requests/responses are for things that need opengl to process
	// such as getting data mesh value at position or screenshotting to texture
	
	enum render_loop_request_type_t {
		RLR_NONE,
		RLR_GETDATA,
	};
	struct render_loop_response_t {
		union {
			struct {
				mesh::Data::data_t data;
			} getdata;
		};
	};
	struct render_loop_request_t {
		render_loop_request_type_t type;
		union {
			struct {
				size_t screen_x;
				size_t screen_inverse_y;
			} getdata;
		};
		bool is_processed = false;
		render_loop_response_t result;
	};
	typedef size_t render_loop_request_id_t;
	typedef std::unordered_map< render_loop_request_id_t, render_loop_request_t > render_loop_requests_t;
	
	// make sure to call these from same thread only
	render_loop_request_id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	
	std::pair< bool, std::optional< render_loop_response_t > > GetRenderLoopResponse( const render_loop_request_id_t id );
	void CancelRenderLoopRequest( const render_loop_request_id_t id );
	render_loop_requests_t* GetRenderLoopRequests();
	const bool HasRenderLoopRequestsOfType( const render_loop_request_type_t type ) const;
	
protected:
	const mesh::Mesh* m_mesh = nullptr;
	const Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	render_flag_t m_render_flags = RF_NONE;
	Color m_tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	// data mesh stuff
	const mesh::Data* m_data_mesh = nullptr;
	render_loop_requests_t m_render_loop_requests = {};
	render_loop_request_id_t m_last_render_loop_request_id = 0;
};

} /* namespace scene */
} /* namespace actor */
