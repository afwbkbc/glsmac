#include <cmath>
#include <cstring>

#include "Mesh.h"

#include "util/Math.h"

using namespace std;
using namespace util;

namespace types {

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

Mesh::index_t Mesh::AddVertex( const Vec3 &coord, const Vec2<coord_t> &tex_coord, const Color tint, const Vec3 &normal ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + to_string( m_vertex_i ) + " >= " + to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof(coord) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	offset += VERTEX_TEXCOORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tint ) ), &tint, sizeof( tint ) );
	offset += VERTEX_TINT_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( normal ) ), &normal, sizeof( normal ) );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}
Mesh::index_t Mesh::AddVertex( const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord, const Color tint, const Vec3 &normal ) {
	return AddVertex( Vec3( coord.x, coord.y, 0.0 ), tex_coord, tint, normal );
}

void Mesh::SetVertex( const index_t index, const Vec3 &coord, const Vec2<coord_t> &tex_coord, const Color tint, const Vec3 &normal ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	size_t offset = index * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
	offset += VERTEX_COORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	offset += VERTEX_TEXCOORD_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( tint ) ), &tint, sizeof( tint ) );
	offset += VERTEX_TINT_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( normal ) ), &normal, sizeof( normal ) );
	Update();
}

void Mesh::SetVertex( const index_t index, const Vec2<coord_t> &coord, const Vec2<coord_t> &tex_coord, const Color tint, const Vec3 &normal ) {
	SetVertex( index, { coord.x, coord.y, 0.0 }, tex_coord, tint, normal );
}

void Mesh::SetVertexCoord( const index_t index, const Vec3 &coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord ) ), &coord, sizeof( coord ) );
	Update();
}
void Mesh::SetVertexCoord( const index_t index, const Vec2<coord_t> &coord ) {
	SetVertexCoord( index, { coord.x, coord.y, 0.0 } );
}

void Mesh::SetVertexTexCoord( const index_t index, const Vec2<coord_t> &tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}

void Mesh::SetVertexTint( const index_t index, const Color tint ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE ) * sizeof( coord_t ), sizeof( Color ) ), &tint, sizeof( tint ) );
}

void Mesh::SetVertexNormal( const index_t index, const Vec3& normal ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE ) * sizeof( coord_t ), sizeof( normal ) ), &normal, sizeof( normal ) );
}

void Mesh::SetSurface( const index_t index, const surface_t& surface ) {
	ASSERT( index < m_surface_count, "surface out of bounds" );
	// add triangle
	memcpy( ptr( m_index_data, index * SURFACE_SIZE * sizeof( index_t ), sizeof( surface ) ), &surface, sizeof( surface ) );
}

const Vec3 Mesh::GetVertexNormal( const index_t index ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	Vec3 normal;
	memcpy( &normal, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE ) * sizeof( coord_t ), sizeof( normal ) ), sizeof( normal ) );
	return normal;
}

void Mesh::Finalize() {
	ASSERT( !m_is_final, "finalize on already finalized mesh" );
	ASSERT( m_vertex_i == m_vertex_count, "vertex data not fully initialized on finalize" );
	ASSERT( m_surface_i == m_surface_count, "surface data not fully initialized on finalize" );
	
	UpdateNormals();
	
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

void Mesh::UpdateNormals() {
	//Log( "Updating normals");
	
	// update normals
	const surface_t* surface;
	Vec3 *a, *b, *c;
	Vec3 ab, ac, n;
	const size_t vo = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE;
	
	for ( size_t v = 0 ; v < m_vertex_i ; v++ ) {
		memset( ptr( m_vertex_data, ( v * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) ), 0, sizeof( Vec3 ) );
	}
	for ( size_t s = 0 ; s < m_surface_i ; s++ ) {
		surface = (surface_t*)ptr( m_index_data, s * SURFACE_SIZE * sizeof( index_t ), sizeof( surface_t ) );
		a = (Vec3*)ptr( m_vertex_data, surface->v1 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );
		b = (Vec3*)ptr( m_vertex_data, surface->v2 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );
		c = (Vec3*)ptr( m_vertex_data, surface->v3 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );
        ab = *b - *a;
        ac = *c - *a;
        n = Math::Cross( ab, ac );
		*(Vec3*)ptr( m_vertex_data, ( surface->v1 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) ) += n;
		*(Vec3*)ptr( m_vertex_data, ( surface->v2 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) ) += n;
		*(Vec3*)ptr( m_vertex_data, ( surface->v3 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) ) += n;
	}
	for ( size_t v = 0 ; v < m_vertex_i ; v++ ) {
		*(Vec3*)ptr( m_vertex_data, ( v * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) )
			=
		Math::Normalize( *(Vec3*)ptr( m_vertex_data, ( v * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( Vec3 ) ) );
	}
	
	Update();
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
