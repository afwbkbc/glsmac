#include "Mesh.h"

#include "rr/GetData.h"
#include "rr/Capture.h"
#include "types/mesh/Mesh.h"
#include "types/mesh/Data.h"
#include "types/texture/Texture.h"

namespace scene {
namespace actor {

Mesh::Mesh( const std::string& name, const types::mesh::Mesh* mesh, const type_t type )
	: Actor( type, name )
	, m_mesh( mesh ) {
	//
}

Mesh::~Mesh() {
	if ( m_mesh ) {
		DELETE( m_mesh );
	}
	if ( m_data_mesh ) {
		DELETE( m_data_mesh );
	}
}

void Mesh::SetMesh( const types::mesh::Mesh* mesh ) {
	ASSERT( !m_mesh, "mesh already set" );
	m_mesh = mesh;
}

const types::mesh::Mesh* Mesh::GetMesh() const {
	ASSERT( m_mesh, "mesh not set" );
	return m_mesh;
}

void Mesh::SetTintColor( const types::Color tint_color ) {
	if ( !( m_render_flags & RF_USE_TINT ) || m_tint_color != tint_color ) {
		m_render_flags |= RF_USE_TINT;
		m_tint_color = tint_color;
		UpdateCache();
	}
}

const types::Color& Mesh::GetTintColor() const {
	return m_tint_color;
}

const types::mesh::Data* Mesh::GetDataMesh() const {
	return m_data_mesh;
}

void Mesh::SetTexture( types::texture::Texture* texture ) {
	ASSERT( texture, "texture is null" );
	const auto counter = texture->UpdatedCount();
	if ( m_texture != texture || counter != m_texture_update_counter ) {
		m_texture = texture;
		m_texture_update_counter = counter;
		UpdateCache();
	}
}

types::texture::Texture* Mesh::GetTexture() const {
	return m_texture;
}

void Mesh::SetDataMesh( const types::mesh::Data* data_mesh ) {
	ASSERT( !m_data_mesh, "data mesh already set" );
	m_data_mesh = data_mesh;
}

rr::id_t Mesh::GetDataAt( const size_t screen_x, const size_t screen_inverse_y ) {
	//Log( "Requesting data at " + std::to_string( screen_x ) + "x" + std::to_string( screen_inverse_y ) );
	NEWV( request, rr::GetData );
	request->screen_x = screen_x;
	request->screen_inverse_y = screen_inverse_y;
	return RR_Send( request );
}

Mesh::data_response_t Mesh::GetDataResponse( const rr::id_t request_id ) {
	auto* r = RR_GetResponse< rr::GetData >( request_id );
	if ( r ) {
		data_response_t result = {
			true,
			r->data
		};
		//Log( "Data request " + std::to_string( id ) + " completed" );
		DELETE( r );
		return result;
	}
	else {
		return {
			false,
			std::nullopt
		};
	}
}

void Mesh::CancelDataRequest( const rr::id_t request_id ) {
	//Log( "Canceling data request " + std::to_string( id ) );
	RR_Cancel< rr::GetData >( request_id );
}

rr::id_t Mesh::CaptureToTexture( scene::Camera* camera, const types::Vec2< size_t > texture_dimensions ) {
	Log( "Requesting capture-to-texture" );
	NEWV( request, rr::Capture );
	request->camera = camera;
	request->texture_width = texture_dimensions.x;
	request->texture_height = texture_dimensions.y;
	return RR_Send( request );
}

types::texture::Texture* Mesh::GetCaptureToTextureResponse( const rr::id_t request_id ) {
	auto* r = RR_GetResponse< rr::Capture >( request_id );
	if ( r ) {
		types::texture::Texture* result = r->texture;
		r->texture = nullptr; // to prevent deletion in destructor
		ASSERT( result, "received null texture response for " + std::to_string( request_id ) );
		Log( "Received capture-to-texture response for " + std::to_string( request_id ) );
		DELETE( r );
		return result;
	}
	return nullptr;
}

void Mesh::CancelCaptureToTextureRequest( const rr::id_t request_id ) {
	Log( "Canceling capture-to-texture request " + std::to_string( request_id ) );
}

}
}
