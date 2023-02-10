#include "Mesh.h"

namespace scene {
namespace actor {

Mesh::Mesh( const std::string &name, const types::RenderMesh *mesh, const type_t type ) : Actor( type, name ), m_mesh( mesh ) {
	
}

Mesh::~Mesh() {
	DELETE( m_mesh );
	if ( m_data_mesh ) {
		DELETE( m_data_mesh );
	}
}

const types::RenderMesh *Mesh::GetMesh() const {
	return m_mesh;
}

const types::DataMesh* Mesh::GetDataMesh() const {
	return m_data_mesh;
}

void Mesh::SetTexture( const types::Texture* texture ) {
	m_texture = texture;
}

const types::Texture* Mesh::GetTexture() {
	return m_texture;
}

void Mesh::SetDataMesh( const types::DataMesh* data_mesh ) {
	ASSERT( !m_data_mesh, "data mesh already set" );
	m_data_mesh = data_mesh;
}

static Mesh::data_request_id_t s_next_data_request_id = 1;
	
Mesh::data_request_id_t Mesh::GetDataAt( const size_t screen_x, const size_t screen_inverse_y ) {
	data_request_id_t id = m_last_data_request_id = s_next_data_request_id++;
	// Log( "Requesting data at " + to_string( screen_x ) + "x" + to_string( screen_inverse_y ) );
	m_data_requests[ id ] = {
		screen_x,
		screen_inverse_y,
		false,
		0,
	};
	return id;
}

pair< bool, types::DataMesh::data_t > Mesh::GetDataResponse( const data_request_id_t id ) {
	auto it = m_data_requests.find( id );
	ASSERT( it != m_data_requests.end(), "data request not found" );
	
	if ( it->second.is_processed ) {
		pair< bool, types::DataMesh::data_t > result = { true, it->second.result };
		// Log( "Data request " + to_string( id ) + " completed" );
		m_data_requests.erase( it );
		return result;
	}
	else {
		return { false, 0 };
	}
}

void Mesh::CancelDataRequest( const data_request_id_t id ) {
	auto it = m_data_requests.find( id );
	ASSERT( it != m_data_requests.end(), "data request not found for removal" );
	// Log( "Canceling data request " + to_string( id ) );
	m_data_requests.erase( it );
}

Mesh::data_requests_t* Mesh::GetDataRequests() {
	return &m_data_requests;
}

const Mesh::data_request_id_t Mesh::GetLastDataRequestId() const {
	return m_last_data_request_id;
}

} /* namespace actor */
} /* namespace scene */
