#include "Mesh.h"

#include <cmath>
#include <cstring>

#include "util/Math.h"

using namespace std;
using namespace util;

namespace types {

Mesh::Mesh( const uint8_t vertex_size, const size_t vertex_count, const size_t surface_count )
	: VERTEX_SIZE( vertex_size )
	, m_vertex_count( vertex_count )
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
	memset( ptr( m_vertex_data, 0, GetVertexDataSize() ), 0, GetVertexDataSize() );
	memset( ptr( m_index_data, 0, GetIndexDataSize() ), 0, GetIndexDataSize() );
	m_is_final = true;
}

void Mesh::AddSurface( const surface_t& surface  ) {
	ASSERT( !m_is_final, "addsurface on already finalized mesh" );
	ASSERT( m_surface_i < m_surface_count, "surface out of bounds" );
	// add triangle
	memcpy( ptr( m_index_data, m_surface_i * SURFACE_SIZE * sizeof( index_t ), sizeof( surface ) ), &surface, sizeof( surface ) );
	m_surface_i++;
}

void Mesh::SetVertexCoord( const index_t index, const Vec3 &coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord ) ), &coord, sizeof( coord ) );
	Update();
}
void Mesh::SetVertexCoord( const index_t index, const Vec2<coord_t> &coord ) {
	SetVertexCoord( index, { coord.x, coord.y, 0.0 } );
}

void Mesh::SetSurface( const index_t index, const surface_t& surface ) {
	ASSERT( index < m_surface_count, "surface out of bounds" );
	// add triangle
	memcpy( ptr( m_index_data, index * SURFACE_SIZE * sizeof( index_t ), sizeof( surface ) ), &surface, sizeof( surface ) );
}

void Mesh::Finalize() {
	ASSERT( !m_is_final, "finalize on already finalized mesh" );
	ASSERT( m_vertex_i == m_vertex_count, "vertex data not fully initialized on finalize" );
	ASSERT( m_surface_i == m_surface_count, "surface data not fully initialized on finalize" );
	
	m_is_final = true;
}

void Mesh::GetVertexCoord( const index_t index, Vec3* coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( coord, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord ) ), sizeof( coord ) );
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

const Buffer Mesh::Serialize() const {
	Buffer buf;
	
	buf.WriteBool( m_is_final );
	
	buf.WriteInt( m_vertex_count );
	buf.WriteInt( m_vertex_i );
	buf.WriteData( m_vertex_data, GetVertexDataSize() );

	buf.WriteInt( m_index_count );
	buf.WriteInt( m_surface_count );
	buf.WriteInt( m_surface_i );
	buf.WriteData( m_index_data, GetIndexDataSize() );

	return buf;
}

void Mesh::Unserialize( Buffer buf ) {
	
	m_is_final = buf.ReadBool();
	
	size_t vertex_count = buf.ReadInt();
	ASSERT( vertex_count == m_vertex_count, "mesh read vertex count mismatch ( " + to_string( vertex_count ) + " != " + to_string( m_vertex_count ) + " )" );
	m_vertex_i = buf.ReadInt();
	m_vertex_data = (uint8_t*)buf.ReadData( GetVertexDataSize() );
	
	size_t index_count = buf.ReadInt();
	ASSERT( index_count == m_index_count, "mesh read index count mismatch ( " + to_string( index_count ) + " != " + to_string( m_index_count ) + " )" );
	
	size_t surface_count = buf.ReadInt();
	ASSERT( surface_count == m_surface_count, "mesh read surface count mismatch ( " + to_string( surface_count ) + " != " + to_string( m_surface_count ) + " )" );
	
	m_surface_i = buf.ReadInt();
	m_index_data = (uint8_t*)buf.ReadData( GetIndexDataSize() );

	Update();
}

}
