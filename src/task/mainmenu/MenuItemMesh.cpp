#include "MenuItemMesh.h"

using namespace scene::mesh;

namespace task {
namespace mainmenu {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

MenuItemMesh::MenuItemMesh() 
	: Vec3Mesh()
{

	auto v_one = AddVertex( Vec3Mesh::vec3_t( -RANDNUM, -RANDNUM, -RANDNUM ) );
	auto v_two = AddVertex( Vec3Mesh::vec3_t( RANDNUM, -RANDNUM, -RANDNUM ) );
	auto v_three = AddVertex( Vec3Mesh::vec3_t( RANDNUM, RANDNUM, -RANDNUM ) );
	auto v_four = AddVertex( Vec3Mesh::vec3_t( -RANDNUM, RANDNUM, -RANDNUM ) );
	auto v_five = AddVertex( Vec3Mesh::vec3_t( -RANDNUM, -RANDNUM, RANDNUM ) );
	auto v_six = AddVertex( Vec3Mesh::vec3_t( RANDNUM, -RANDNUM, RANDNUM ) );
	auto v_seven = AddVertex( Vec3Mesh::vec3_t( RANDNUM, RANDNUM, RANDNUM ) );
	auto v_eight = AddVertex( Vec3Mesh::vec3_t( -RANDNUM, RANDNUM, RANDNUM ) );
	AddSurface( v_one, v_two, v_three );
	AddSurface( v_three, v_four, v_one );
	AddSurface( v_four, v_five, v_six );
	AddSurface( v_five, v_six, v_four );

}

}
}
