#include <cstring>
#include <cmath>
#include <limits>
#include <memory>

#include "Mesh.h"

#include "util/Math.h"

namespace types {
namespace mesh {

Mesh::Mesh( const type_t type, const data_type_t data_type, const uint8_t vertex_size, const size_t vertex_count, const size_t surface_count )
	: m_type( type )
	, m_data_type( data_type )
	, VERTEX_SIZE( vertex_size )
	, m_vertex_count( vertex_count )
	, m_surface_count( surface_count )
	, m_index_count( surface_count * SURFACE_SIZE ) {
	m_vertex_data = (uint8_t*)malloc( GetVertexDataSize() );
	m_index_data = (uint8_t*)malloc( GetIndexDataSize() );
}

Mesh::Mesh( const Mesh& other )
	: m_type( other.m_type )
	, m_data_type( other.m_data_type )
	, VERTEX_SIZE( other.VERTEX_SIZE )
	, m_vertex_count( other.m_vertex_count )
	, m_surface_count( other.m_surface_count )
	, m_index_count( other.m_index_count )
	, m_vertex_i( other.m_vertex_i )
	, m_surface_i( other.m_surface_i )
	, m_update_counter( other.m_update_counter )
	, m_is_final( other.m_is_final ) {
	size_t sz = GetVertexDataSize();
	m_vertex_data = (uint8_t*)malloc( sz );
	memcpy( ptr( m_vertex_data, 0, sz ), ptr( other.m_vertex_data, 0, sz ), sz );
	sz = GetIndexDataSize();
	m_index_data = (uint8_t*)malloc( sz );
	memcpy( ptr( m_index_data, 0, sz ), ptr( other.m_index_data, 0, sz ), sz );
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

index_t Mesh::AddEmptyVertex() {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + std::to_string( m_vertex_i ) + " >= " + std::to_string( m_vertex_count ) + ")" );
	index_t ret = m_vertex_i;
	m_vertex_i++;
	return ret;
}

surface_id_t Mesh::AddSurface( const surface_t& surface ) {
	ASSERT( !m_is_final, "addsurface on already finalized mesh" );
	ASSERT( m_surface_i < m_surface_count, "surface out of bounds" );
	// add triangle
	memcpy( ptr( m_index_data, m_surface_i * SURFACE_SIZE * sizeof( index_t ), sizeof( surface ) ), &surface, sizeof( surface ) );
	surface_id_t ret = m_surface_i;
	m_surface_i++;
	return ret;
}

void Mesh::SetVertexCoord( const index_t index, const types::Vec3& coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord ) ), &coord, sizeof( coord ) );
	Update();
}

void Mesh::SetVertexCoord( const index_t index, const Vec2< coord_t >& coord ) {
	SetVertexCoord(
		index, {
			coord.x,
			coord.y,
			0.0
		}
	);
}

void Mesh::SetSurface( const index_t index, const surface_t& surface ) {
	ASSERT( index < m_surface_count, "surface out of bounds" );
	// add triangle
	memcpy( ptr( m_index_data, index * SURFACE_SIZE * sizeof( index_t ), sizeof( surface ) ), &surface, sizeof( surface ) );
	Update();
}

void Mesh::Finalize() {
	ASSERT( !m_is_final, "finalize on already finalized mesh" );
	ASSERT( m_vertex_i == m_vertex_count, "vertex data not fully initialized on finalize" );
	ASSERT( m_surface_i == m_surface_count, "surface data not fully initialized on finalize" );

	m_is_final = true;
}

void Mesh::GetVertexCoord( const index_t index, types::Vec3* coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( coord, 0, sizeof( types::Vec3 ) ), ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ), sizeof( types::Vec3 ) ), sizeof( types::Vec3 ) );
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

const Mesh::type_t Mesh::GetType() const {
	return m_type;
}

const Mesh::data_type_t Mesh::GetDataType() const {
	return m_data_type;
}

const types::Buffer Mesh::Serialize() const {
	types::Buffer buf;

	buf.WriteInt( m_type );
	buf.WriteInt( m_data_type );

	buf.WriteInt( m_vertex_count );
	buf.WriteInt( m_vertex_i );
	buf.WriteData( m_vertex_data, GetVertexDataSize() );

	buf.WriteInt( m_index_count );
	buf.WriteInt( m_surface_count );
	buf.WriteInt( m_surface_i );
	buf.WriteData( m_index_data, GetIndexDataSize() );

	buf.WriteBool( m_is_final );

	return buf;
}

void Mesh::Deserialize( types::Buffer buf ) {

	const auto type = buf.ReadInt();
	if ( type != m_type ) {
		THROW( "mesh type mismatch" );
	}

	const auto data_type = buf.ReadInt();
	if ( data_type != m_data_type ) {
		THROW( "mesh data type mismatch" );
	}

	const auto vertex_count = buf.ReadInt< index_t >( "mesh vertex count" );
	if ( vertex_count != m_vertex_count ) {
		THROW( "mesh read vertex count mismatch ( " + std::to_string( vertex_count ) + " != " + std::to_string( m_vertex_count ) + " )" );
	}
	const auto vertex_i = buf.ReadInt< index_t >( "mesh vertex position" );
	if ( vertex_i > m_vertex_count ) {
		THROW( "mesh read vertex position overflow" );
	}
	if ( GetVertexDataSize() > ( std::numeric_limits< uint32_t >::max )() ) {
		THROW( "serialized mesh vertex data exceeds buffer capacity" );
	}
	std::unique_ptr< uint8_t, decltype( &free ) > vertex_data(
		(uint8_t*)buf.ReadData( static_cast< uint32_t >( GetVertexDataSize() ) ),
		&free
	);

	const auto index_count = buf.ReadInt< index_t >( "mesh index count" );
	if ( index_count != m_index_count ) {
		THROW( "mesh read index count mismatch ( " + std::to_string( index_count ) + " != " + std::to_string( m_index_count ) + " )" );
	}

	const auto surface_count = buf.ReadInt< surface_id_t >( "mesh surface count" );
	if ( surface_count != m_surface_count ) {
		THROW( "mesh read surface count mismatch ( " + std::to_string( surface_count ) + " != " + std::to_string( m_surface_count ) + " )" );
	}

	const auto surface_i = buf.ReadInt< surface_id_t >( "mesh surface position" );
	if ( surface_i > m_surface_count ) {
		THROW( "mesh read surface position overflow" );
	}
	if ( GetIndexDataSize() > ( std::numeric_limits< uint32_t >::max )() ) {
		THROW( "serialized mesh index data exceeds buffer capacity" );
	}
	std::unique_ptr< uint8_t, decltype( &free ) > index_data(
		(uint8_t*)buf.ReadData( static_cast< uint32_t >( GetIndexDataSize() ) ),
		&free
	);
	const bool is_final = buf.ReadBool();
	if ( buf.GetRemaining() != 0 ) {
		THROW( "unexpected data after serialized mesh" );
	}

	const auto* const vertex_values = reinterpret_cast< const coord_t* >( vertex_data.get() );
	const size_t finite_values_per_vertex = m_data_type == DT_DATA
		? VERTEX_COORD_SIZE
		: VERTEX_SIZE;
	for ( size_t vertex = 0 ; vertex < m_vertex_count ; vertex++ ) {
		for ( size_t component = 0 ; component < finite_values_per_vertex ; component++ ) {
			if ( !std::isfinite( vertex_values[ vertex * VERTEX_SIZE + component ] ) ) {
				THROW( "serialized mesh contains a non-finite vertex value" );
			}
		}
	}
	const auto* const indices = reinterpret_cast< const index_t* >( index_data.get() );
	for ( size_t i = 0 ; i < m_index_count ; i++ ) {
		if ( indices[ i ] >= m_vertex_count ) {
			THROW( "serialized mesh contains an out-of-bounds vertex index" );
		}
	}

	free( m_vertex_data );
	free( m_index_data );
	m_vertex_i = vertex_i;
	m_vertex_data = vertex_data.release();
	m_surface_i = surface_i;
	m_index_data = index_data.release();
	m_is_final = is_final;

	Update();
}

}
}
