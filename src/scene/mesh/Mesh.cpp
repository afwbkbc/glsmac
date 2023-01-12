#include <cmath>

#include "Mesh.h"

using namespace std;

namespace scene {
namespace mesh {

Mesh::Mesh( const size_t vertex_count, const size_t surface_count )
	: m_vertex_count( vertex_count )
	, m_surface_count( surface_count )
	, m_index_count( surface_count * SURFACE_SIZE )
{
	m_vertex_data = (uint8_t*)malloc( GetVertexDataSize() );
	m_index_data = (uint8_t*)malloc( GetIndexDataSize() );
}

Mesh::~Mesh() {
	free( m_vertex_data );
	free( m_index_data );
}

void Mesh::Clear() {
	memset( m_vertex_data, 0, GetVertexDataSize() );
	memset( m_index_data, 0, GetIndexDataSize() );
	m_is_final = true;
}

void Mesh::AddSurface( const Mesh::surface_t& surface  ) {
#if DEBUG
	if ( m_is_final ) {
		throw MeshError( "addsurface on already finalized mesh" );
	}
	if ( m_surface_i >= m_surface_count ) {
		throw MeshError( "surface out of bounds" );
	}
#endif
	// add triangle
	memcpy( m_index_data + m_surface_i * SURFACE_SIZE * sizeof( index_t ), &surface, sizeof(surface) );
}

Mesh::index_t Mesh::AddVertex( const Vec3 &coord, const Vec2<Mesh::coord_t> &tex_coord ) {
#if DEBUG
	if ( m_is_final ) {
		throw MeshError( "addvertex on already finalized mesh" );
	}
	if ( m_vertex_i >= m_vertex_count ) {
		throw MeshError( "vertex out of bounds" );
	}
#endif
	memcpy( m_vertex_data + m_vertex_i * VERTEX_SIZE * sizeof( coord_t ), &coord, sizeof(coord) );
	memcpy( m_vertex_data + m_vertex_i * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), &tex_coord, sizeof(tex_coord) );
	Mesh::index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}
Mesh::index_t Mesh::AddVertex( const Vec2<Mesh::coord_t> &coord, const Vec2<Mesh::coord_t> &tex_coord ) {
	return AddVertex( Vec3( coord.x, coord.y, 0.0 ), tex_coord );
}

void Mesh::SetVertexCoord( const index_t index, const Vec3 &coord ) {
#if DEBUG
	if ( index >= m_vertex_count ) {
		throw MeshError( "index out of bounds" );
	}
#endif
	memcpy( m_vertex_data + index * VERTEX_SIZE * sizeof( coord_t ), &coord, sizeof(coord) );
	Update();
}
void Mesh::SetVertexCoord( const index_t index, const Vec2<Mesh::coord_t> &coord ) {
	SetVertexCoord( index, { coord.x, coord.y, 0.0 } );
}

void Mesh::SetVertexTexCoord( const index_t index, const Vec2<Mesh::coord_t> &tex_coord ) {
#if DEBUG
	if ( index >= m_vertex_count ) {
		throw MeshError( "index out of bounds" );
	}
#endif
	memcpy( m_vertex_data + index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), &tex_coord, sizeof(tex_coord) );
	Update();
}

void Mesh::SetSurface( const index_t index, const Mesh::surface_t& surface ) {
#if DEBUG
	if ( index >= m_surface_count ) {
		throw MeshError( "surface out of bounds" );
	}
#endif
	// add triangle
	memcpy( m_index_data + index * SURFACE_SIZE * sizeof( index_t ), &surface, sizeof(surface) );
}


void Mesh::Finalize() {
#if DEBUG
	if ( !m_is_final ) {
		throw MeshError( "finalize on already finalized mesh" );
	}
	if ( m_vertex_i != m_vertex_count ) {
		throw MeshError( "vertex data not fully initialized on finalize" );
	}
	if ( m_surface_i != m_surface_count ) {
		throw MeshError( "surface data not fully initialized on finalize" );
	}
#endif
	m_is_final = true;
	Update();
}

void Mesh::SetVertex( const index_t index, const Vec3 &coord, const Vec2<Mesh::coord_t> &tex_coord ) {
#if DEBUG
	if ( index >= m_vertex_count ) {
		throw MeshError( "index out of bounds" );
	}
#endif
	memcpy( m_vertex_data + index * VERTEX_SIZE * sizeof( coord_t ), &coord, sizeof(coord) );
	memcpy( m_vertex_data + index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), &tex_coord, sizeof(tex_coord) );
	Update();
}
void Mesh::SetVertex( const index_t index, const Vec2<Mesh::coord_t> &coord, const Vec2<Mesh::coord_t> &tex_coord ) {
	SetVertex( index, { coord.x, coord.y, 0.0 }, tex_coord );
}

const size_t Mesh::GetVertexCount() const {
	return m_vertex_count;
}
const size_t Mesh::GetVertexDataSize() const {
	return m_vertex_count * VERTEX_SIZE * sizeof( coord_t );
}
const uint8_t* Mesh::GetVertexData() const {
	return m_vertex_data;
}

const size_t Mesh::GetIndexCount() const {
	return m_index_count;
}
const size_t Mesh::GetSurfaceCount() const {
	return m_surface_count;
}
const size_t Mesh::GetIndexDataSize() const {
	return m_index_count * sizeof( index_t );
}
const uint8_t* Mesh::GetIndexData() const {
	return m_index_data;
}

void Mesh::Update() {
	m_update_counter++;
}

const size_t Mesh::UpdatedCount() const {
	return m_update_counter;
}

} /* namespace mesh */
} /* namespace scene */
