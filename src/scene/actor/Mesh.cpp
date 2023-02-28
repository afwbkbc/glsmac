#include "Mesh.h"

namespace scene {
namespace actor {

Mesh::Mesh( const std::string &name, const mesh::Mesh *mesh, const type_t type ) : Actor( type, name ), m_mesh( mesh ) {
	
}

Mesh::~Mesh() {
	if ( m_mesh ) {
		DELETE( m_mesh );
	}
	if ( m_data_mesh ) {
		DELETE( m_data_mesh );
	}
}

void Mesh::SetMesh( const mesh::Mesh* mesh ) {
	ASSERT( !m_mesh, "mesh already set" );
	m_mesh = mesh;
}

const mesh::Mesh *Mesh::GetMesh() const {
	ASSERT( m_mesh, "mesh not set" );
	return m_mesh;
}

void Mesh::SetRenderFlags( const render_flag_t render_flags ) {
	m_render_flags = render_flags;
}

const Mesh::render_flag_t Mesh::GetRenderFlags() const {
	return m_render_flags;
}

void Mesh::SetTintColor( const Color tint_color ) {
	m_tint_color = tint_color;
}

const Color& Mesh::GetTintColor() {
	return m_tint_color;
}

const mesh::Data* Mesh::GetDataMesh() const {
	return m_data_mesh;
}

void Mesh::SetTexture( const Texture* texture ) {
	m_texture = texture;
}

const Texture* Mesh::GetTexture() const {
	return m_texture;
}

void Mesh::SetDataMesh( const mesh::Data* data_mesh ) {
	ASSERT( !m_data_mesh, "data mesh already set" );
	m_data_mesh = data_mesh;
}

static Mesh::render_loop_request_id_t s_next_render_loop_request_id = 1;
	
Mesh::render_loop_request_id_t Mesh::GetDataAt( const size_t screen_x, const size_t screen_inverse_y ) {
	render_loop_request_id_t id = m_last_render_loop_request_id = s_next_render_loop_request_id++;
	//Log( "Requesting data at " + std::to_string( screen_x ) + "x" + std::to_string( screen_inverse_y ) );
	auto& request = m_render_loop_requests[ id ] = {};
	request.type = RLR_GETDATA;
	request.getdata.screen_x = screen_x;
	request.getdata.screen_inverse_y = screen_inverse_y;
	return id;
}

std::pair< bool, std::optional< Mesh::render_loop_response_t > > Mesh::GetRenderLoopResponse( const render_loop_request_id_t id ) {
	auto it = m_render_loop_requests.find( id );
	ASSERT( it != m_render_loop_requests.end(), "data request not found" );
	
	if ( it->second.is_processed ) {
		std::pair< bool, std::optional< render_loop_response_t > > result = { true, it->second.result };
		//Log( "Render loop request " + std::to_string( id ) + " completed" );
		m_render_loop_requests.erase( it );
		return result;
	}
	else {
		return { false, std::nullopt };
	}
}

void Mesh::CancelRenderLoopRequest( const render_loop_request_id_t id ) {
	auto it = m_render_loop_requests.find( id );
	ASSERT( it != m_render_loop_requests.end(), "data request not found for removal" );
	//Log( "Canceling render loop request " + std::to_string( id ) );
	m_render_loop_requests.erase( it );
}

Mesh::render_loop_requests_t* Mesh::GetRenderLoopRequests() {
	return &m_render_loop_requests;
}

const bool Mesh::HasRenderLoopRequestsOfType( const render_loop_request_type_t type ) const {
	for ( auto& r : m_render_loop_requests ) {
		if ( r.second.type == type ) {
			return true;
		}
	}
	return false;
}

} /* namespace actor */
} /* namespace scene */
