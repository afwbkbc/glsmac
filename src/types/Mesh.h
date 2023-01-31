#pragma once

#include <vector>

#include "base/Base.h"

#include "Vec3.h"
#include "Color.h"

namespace types {

CLASS( Mesh, base::Base )
	
	static const uint8_t VERTEX_COORD_SIZE = 3; // x, y, z
	static const uint8_t VERTEX_TEXCOORD_SIZE = 2; // tx, ty
	static const uint8_t VERTEX_TINT_SIZE = 4; // rgba
	static const uint8_t VERTEX_NORMAL_SIZE = 3; // x, y, z
	static const uint8_t VERTEX_SIZE = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE + VERTEX_NORMAL_SIZE;
	static const uint8_t SURFACE_SIZE = 3; // triangles

	typedef float coord_t;
	typedef unsigned int index_t;
	typedef struct {
		const index_t v1;
		const index_t v2;
		const index_t v3;
	} surface_t;
	
	Mesh( const size_t vertex_count, const size_t surface_count );
	~Mesh();
	
	void Clear();
	void AddSurface( const surface_t& surface );
	index_t AddVertex( const Vec3 &coord, const Vec2<Mesh::coord_t> &tex_coord = { 0, 0 }, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	index_t AddVertex( const Vec2<Mesh::coord_t> &coord, const Vec2<Mesh::coord_t> &tex_coord = { 0, 0 }, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	void SetVertex( const index_t index, const Vec3 &coord, const Vec2<Mesh::coord_t> &tex_coord, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	void SetVertex( const index_t index, const Vec2<Mesh::coord_t> &coord, const Vec2<Mesh::coord_t> &tex_coord, const Color tint = { 1.0f, 1.0f, 1.0f, 1.0f }, const Vec3 &normal = { 0.0f, 0.0f, 0.0f } );
	void SetVertexCoord( const index_t index, const Vec3 &coord );
	void SetVertexCoord( const index_t index, const Vec2<Mesh::coord_t> &coord );
	void SetVertexTexCoord( const index_t index, const Vec2<Mesh::coord_t> &tex_coord );
	void SetVertexTint( const index_t index, const Color tint );
	void SetVertexNormal( const index_t index, const Vec3& normal );
	void SetSurface( const index_t index, const Mesh::surface_t& surface );
	void Finalize();
	
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
	
protected:

	bool m_is_final = false;
	
	const size_t m_vertex_count = 0;
	size_t m_vertex_i = 0;
	uint8_t* m_vertex_data = nullptr;

	const size_t m_index_count = 0;
	const size_t m_surface_count = 0;
	size_t m_surface_i = 0;
	uint8_t* m_index_data = nullptr;

	size_t m_update_counter = 0;

private:
	void UpdateNormals();
	
};

}
