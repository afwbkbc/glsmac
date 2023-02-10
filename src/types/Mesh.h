#pragma once

#include "types/Serializable.h"

#include <vector>

#include "Vec3.h"
#include "Color.h"

namespace types {

CLASS( Mesh, Serializable )
	
	const uint8_t VERTEX_SIZE; // set in constructor
	static const uint8_t VERTEX_COORD_SIZE = 3; // x, y, z
	static const uint8_t SURFACE_SIZE = 3; // triangles

	typedef float coord_t;
	typedef unsigned int index_t;
	typedef struct {
		const index_t v1;
		const index_t v2;
		const index_t v3;
	} surface_t;
	
	Mesh( const uint8_t vertex_size, const size_t vertex_count, const size_t surface_count );
	~Mesh();
	
	void Clear();
	
	void AddSurface( const surface_t& surface );
	
	void SetVertexCoord( const index_t index, const Vec3 &coord );
	void SetVertexCoord( const index_t index, const Vec2<coord_t> &coord );
	void SetSurface( const index_t index, const surface_t& surface );
	
	virtual void Finalize();
	
	void GetVertexCoord( const index_t index, Vec3* coord );
	
	const size_t GetVertexCount() const;
	const size_t GetVertexDataSize() const;
	const uint8_t* GetVertexData() const;
	const size_t GetIndexCount() const;
	const size_t GetSurfaceCount() const;
	const size_t GetIndexDataSize() const;
	const uint8_t* GetIndexData() const;
	
	void Update();
	const size_t UpdatedCount() const;
	
	const Buffer Serialize() const;
	void Unserialize( Buffer buf );
protected:

	bool m_is_final = false;
	
	size_t m_vertex_count = 0;
	size_t m_vertex_i = 0;
	uint8_t* m_vertex_data = nullptr;

	size_t m_index_count = 0;
	size_t m_surface_count = 0;
	size_t m_surface_i = 0;
	uint8_t* m_index_data = nullptr;

	size_t m_update_counter = 0;
};

}
