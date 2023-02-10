#pragma once

#include "Actor.h"

#include <unordered_map>

#include "types/RenderMesh.h"
#include "types/DataMesh.h"
#include "types/Color.h"
#include "types/Texture.h"

namespace scene {
namespace actor {

CLASS( Mesh, Actor )

	Mesh( const std::string &name, const types::RenderMesh *mesh, const type_t type = TYPE_MESH );
	~Mesh();
	
	void Iterate();
	
	const types::RenderMesh *GetMesh() const;
	const types::DataMesh* GetDataMesh() const;
	
	void SetTexture( const types::Texture* texture );
	const types::Texture* GetTexture();
	
	void SetDataMesh( const types::DataMesh* data_mesh );
	
	typedef struct {
		size_t screen_x;
		size_t screen_inverse_y;
		bool is_processed;
		types::DataMesh::data_t result;
	} data_request_t;
	typedef size_t data_request_id_t;
	typedef unordered_map< data_request_id_t, data_request_t > data_requests_t;
	
	// make sure to call these from same thread only
	data_request_id_t GetDataAt( const size_t screen_x, const size_t screen_inverse_y );
	pair< bool, types::DataMesh::data_t > GetDataResponse( const data_request_id_t id );
	void CancelDataRequest( const data_request_id_t id );
	data_requests_t* GetDataRequests();
	const data_request_id_t GetLastDataRequestId() const;
	
protected:
	const types::RenderMesh* m_mesh = nullptr;
	const types::Texture* m_texture = nullptr;
	const bool m_is_texture_tiled = false;

	const types::DataMesh* m_data_mesh = nullptr;
	
	data_requests_t m_data_requests = {};
	data_request_id_t m_last_data_request_id = 0;
};

} /* namespace scene */
} /* namespace actor */
