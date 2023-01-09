#pragma once

#include <vector>

#include "base/Base.h"

namespace scene {
namespace mesh {

MAJOR_CLASS( Mesh, base::Base )
	Mesh( const uint8_t vertex_size );
	~Mesh();
	typedef float coord_t;
	typedef unsigned int index_t;
	index_t AddCoord( const coord_t coord );
	void AddIndex( const index_t index );
	void AddSurface( const index_t v1, const index_t v2, const index_t v3 );

	const uint8_t GetVertexSize() { return m_vertex_size; }
	const uint8_t GetSurfaceSize() { return m_surface_size; }
	std::vector<coord_t> *GetVertexData() { return &m_vertex_data; }
	std::vector<index_t> *GetIndexData() { return &m_index_data; }
	void Clear() {
		m_vertex_data.clear();
		m_index_data.clear();
	}
	void Update() {
		m_update_timestamp++;
	}
	const size_t UpdatedAt() {
		return m_update_timestamp;
	}
protected:
	const uint8_t m_vertex_size;
	const uint8_t m_surface_size;

	std::vector<coord_t> m_vertex_data;
	std::vector<index_t> m_index_data;

	size_t m_update_timestamp = 0;
};

} /* namespace mesh */
} /* namespace scene */
