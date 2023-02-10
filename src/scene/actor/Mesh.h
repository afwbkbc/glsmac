#pragma once

#include "Actor.h"

#include <unordered_map>

#include "types/mesh/Simple.h"
#include "types/mesh/Render.h"
#include "types/mesh/Data.h"
#include "types/Color.h"
#include "types/Texture.h"

namespace scene {
namespace actor {

CLASS( Mesh, Actor )

	Mesh( const std::string &name, const types::mesh::Mesh *mesh, const type_t type = TYPE_MESH );
	~Mesh();
	
	void Iterate();
	
	const types::mesh::Mesh *GetMesh() const;
	const types::mesh::Data* GetDataMesh() const;
	
	void SetTexture( const types::Texture* texture );
	const types::Texture* GetTexture();
	
	void SetDataMesh( const types::mesh::Data* data_mesh );
	
	typedef struct {
		size_t screen_x;
		size_t screen_inverse_y;
		bool is_processed;
		types::mesh::Data::data_t result;
	} data_request_t;
	typedef size_t data_request_id_t;
	typedef unordered_map< data_request_id_t, data_request_t > data_requests_t;
	
	// make sure to call these from same thread only
	data_request_id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	pair< bool, types::mesh::Data::data_t > GetDataResponse( const data_request_id_t id );
	void CancelDataRequest( const data_request_id_t id );
	data_requests_t* GetDataRequests();
	const data_request_id_t GetLastDataRequestId() const;
	
protected:
	const types::mesh::Mesh* m_mesh = nullptr;
	const types::Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	const types::mesh::Data* m_data_mesh = nullptr;
	
	data_requests_t m_data_requests = {};
	data_request_id_t m_last_data_request_id = 0;
};

} /* namespace scene */
} /* namespace actor */
