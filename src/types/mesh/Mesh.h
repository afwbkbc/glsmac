#pragma once

#include <vector>

#include "types/Serializable.h"

#include "../Vec3.h"
#include "../Color.h"

namespace types {
namespace mesh {

CLASS( Mesh, Serializable )
	
	enum mesh_type_t {
		MT_BARE, // coordinates only
		MT_SIMPLE, // coordinates, texture
		MT_RENDER, // coordinates, texture, tint, normals
		MT_DATA // coordinates, data
	};
	
	const uint8_t VERTEX_SIZE; // set in constructor
	static constexpr uint8_t VERTEX_COORD_SIZE = 3; // x, y, z
	static constexpr uint8_t SURFACE_SIZE = 3; // triangles

	typedef float coord_t;
	typedef uint32_t index_t;
	typedef uint32_t surface_id_t;
	struct surface_t {
		const index_t v1;
		const index_t v2;
		const index_t v3;
	};
	
	Mesh( const mesh_type_t type, const uint8_t vertex_size, const size_t vertex_count, const size_t surface_count );
	Mesh( const Mesh& other ); // copy from other
	~Mesh();
	
	void Clear();
	
	index_t AddEmptyVertex(); // empty vertex (to be modified later)
	surface_id_t AddSurface( const surface_t& surface );
	
	void SetVertexCoord( const index_t index, const Vec3 &coord );
	void SetVertexCoord( const index_t index, const Vec2<coord_t> &coord );
	void SetSurface( const index_t index, const surface_t& surface );
	
	virtual void Finalize();
	
	void GetVertexCoord( const index_t index, Vec3* coord ) const;
	
	const size_t GetVertexCount() const;
	const size_t GetVertexDataSize() const;
	const uint8_t* GetVertexData() const;
	const size_t GetIndexCount() const;
	const size_t GetSurfaceCount() const;
	const size_t GetIndexDataSize() const;
	const uint8_t* GetIndexData() const;
	
	void Update();
	const size_t UpdatedCount() const;
	
	const mesh_type_t GetType() const;
	
	const Buffer Serialize() const override;
	void Unserialize( Buffer buf ) override;
	
protected:

	const mesh_type_t m_mesh_type = MT_BARE;
	
	bool m_is_final = false;
	
	index_t m_vertex_count = 0;
	index_t m_vertex_i = 0;
	uint8_t* m_vertex_data = nullptr;

	index_t m_index_count = 0;
	
	surface_id_t m_surface_count = 0;
	surface_id_t m_surface_i = 0;
	uint8_t* m_index_data = nullptr;

	size_t m_update_counter = 0;
};

}
}
