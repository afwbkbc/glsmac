#include <cmath>
#include <cstring>

#include "Render.h"

#include "util/Math.h"

namespace types {
namespace mesh {

Render::Render( const size_t vertex_count, const size_t surface_count )
	: Mesh( MT_RENDER, VERTEX_SIZE, vertex_count, surface_count ) {

}

index_t Render::AddVertex( const types::Vec3& coord, const Vec2< coord_t >& tex_coord, const Color tint, const types::Vec3& normal ) {
	ASSERT( !m_is_final, "addvertex on already finalized mesh" );
	ASSERT( m_vertex_i < m_vertex_count, "vertex out of bounds (" + std::to_string( m_vertex_i ) + " >= " + std::to_string( m_vertex_count ) + ")" );
	size_t offset = m_vertex_i * VERTEX_SIZE * sizeof( coord_t );
	memcpy( ptr( m_vertex_data, offset, sizeof( coord ) ), &coord, sizeof( coord ) );
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

index_t Render::AddVertex( const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord, const Color tint, const types::Vec3& normal ) {
	return AddVertex( types::Vec3( coord.x, coord.y, 0.0f ), tex_coord, tint, normal );
}

void Render::SetVertex( const index_t index, const types::Vec3& coord, const Vec2< coord_t >& tex_coord, const Color tint, const types::Vec3& normal ) {
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

void Render::SetVertex( const index_t index, const Vec2< coord_t >& coord, const Vec2< coord_t >& tex_coord, const Color tint, const types::Vec3& normal ) {
	SetVertex(
		index, {
			coord.x,
			coord.y,
			0.0f
		}, tex_coord, tint, normal
	);
}

void Render::SetVertexTexCoord( const index_t index, const Vec2< coord_t >& tex_coord ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( tex_coord ) ), &tex_coord, sizeof( tex_coord ) );
	Update();
}

void Render::SetVertexTint( const index_t index, const Color tint ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE ) * sizeof( coord_t ), sizeof( Color ) ), &tint, sizeof( tint ) );
}

void Render::SetVertexNormal( const index_t index, const types::Vec3& normal ) {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE ) * sizeof( coord_t ), sizeof( normal ) ), &normal, sizeof( normal ) );
}

void Render::GetVertexTexCoord( const index_t index, Vec2< coord_t >* coord ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	memcpy( coord, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + VERTEX_COORD_SIZE * sizeof( coord_t ), sizeof( Vec2< coord_t > ) ), sizeof( Vec2< coord_t > ) );
}

const types::Vec3 Render::GetVertexNormal( const index_t index ) const {
	ASSERT( index < m_vertex_count, "index out of bounds" );
	types::Vec3 normal;
	memcpy( &normal, ptr( m_vertex_data, index * VERTEX_SIZE * sizeof( coord_t ) + ( VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE ) * sizeof( coord_t ), sizeof( normal ) ), sizeof( normal ) );
	return normal;
}

void Render::CombineNormals( const std::vector< index_t >& indices ) {
	ASSERT( !indices.empty(), "normals list empty" );
/*#ifdef DEBUG
	std::string logstr = "";
	for ( auto i : indices ) {
		logstr += std::to_string( i ) + " ";
	}
	Log( "Combining normals ( " + logstr + ")" );
#endif*/
	types::Vec3 normal = {
		0.0f,
		0.0f,
		0.0f
	};
	for ( auto i : indices ) {
		normal += GetVertexNormal( i );
	}
	normal /= indices.size();
	for ( auto i : indices ) {
		SetVertexNormal( i, normal );
	}
}

void Render::Finalize() {
	Mesh::Finalize();
}

void Render::UpdateNormals( const std::vector< surface_id_t >& surfaces ) {
	//Log( "Updating normals for " + std::to_string( surfaces.size() ) + " surface(s)" );

	const surface_t* surface;
	types::Vec3* a, * b, * c;
	types::Vec3 n;
	const size_t vo = VERTEX_COORD_SIZE + VERTEX_TEXCOORD_SIZE + VERTEX_TINT_SIZE;

	for ( surface_id_t surface_id : surfaces ) {
		surface = (surface_t*)ptr( m_index_data, surface_id * SURFACE_SIZE * sizeof( index_t ), sizeof( surface_t ) );

		memset( ptr( m_vertex_data, ( surface->v1 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ), 0, sizeof( types::Vec3 ) );
		memset( ptr( m_vertex_data, ( surface->v2 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ), 0, sizeof( types::Vec3 ) );
		memset( ptr( m_vertex_data, ( surface->v3 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ), 0, sizeof( types::Vec3 ) );

		a = (Vec3*)ptr( m_vertex_data, surface->v1 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );
		b = (Vec3*)ptr( m_vertex_data, surface->v2 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );
		c = (Vec3*)ptr( m_vertex_data, surface->v3 * VERTEX_SIZE * sizeof( coord_t ), sizeof( coord_t ) );

		n = util::Math::Cross( *b - *a, *c - *a );

		*(Vec3*)ptr( m_vertex_data, ( surface->v1 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) += n;
		*(Vec3*)ptr( m_vertex_data, ( surface->v2 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) += n;
		*(Vec3*)ptr( m_vertex_data, ( surface->v3 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) += n;

		*(Vec3*)ptr( m_vertex_data, ( surface->v1 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) )
			=
			util::Math::Normalize( *(Vec3*)ptr( m_vertex_data, ( surface->v1 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) );
		*(Vec3*)ptr( m_vertex_data, ( surface->v2 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) )
			=
			util::Math::Normalize( *(Vec3*)ptr( m_vertex_data, ( surface->v2 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) );
		*(Vec3*)ptr( m_vertex_data, ( surface->v3 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) )
			=
			util::Math::Normalize( *(Vec3*)ptr( m_vertex_data, ( surface->v3 * VERTEX_SIZE + vo ) * sizeof( coord_t ), sizeof( types::Vec3 ) ) );
	}

	Update();
}

void Render::UpdateAllNormals() {
	std::vector< surface_id_t > surfaces = {};
	surfaces.reserve( m_surface_i );
	for ( surface_id_t i = 0 ; i < m_surface_i ; i++ ) {
		surfaces.push_back( i );
	}
	UpdateNormals( surfaces );
}

Render* Render::Rectangle( float w, float h, const tex_coords_t tex_coords ) {
	w /= 2;
	h /= 2;
	NEWV( mesh, Render, 4, 2 );
	const auto top_left = mesh->AddVertex(
		{
			-w,
			-h,
			0.0f
		}, {
			tex_coords.first.x,
			tex_coords.first.y
		}
	);
	const auto top_right = mesh->AddVertex(
		{
			w,
			-h,
			0.0f
		}, {
			tex_coords.second.x,
			tex_coords.first.y
		}
	);
	const auto bottom_right = mesh->AddVertex(
		{
			w,
			h,
			0.0f
		}, {
			tex_coords.second.x,
			tex_coords.second.y
		}
	);
	const auto bottom_left = mesh->AddVertex(
		{
			-w,
			h,
			0.0f
		}, {
			tex_coords.first.x,
			tex_coords.second.y
		}
	);
	mesh->AddSurface(
		{
			bottom_right,
			bottom_left,
			top_left
		}
	);
	mesh->AddSurface(
		{
			top_left,
			top_right,
			bottom_right
		}
	);
	mesh->Finalize();
	return mesh;
}

}
}
