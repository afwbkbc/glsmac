#include <cstdlib>

#include "WorldTestTask.h"

#include "scene/mesh/Vec3Mesh.h"
#include "scene/actor/MeshActor.h"
#include "engine/Engine.h"

namespace task {
namespace test {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )
//#define RANDNUM 1.0

scene::mesh::Mesh *testmesh3() {
	auto *mesh = new scene::mesh::Vec3Mesh;
	auto v_one = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( -RANDNUM, -RANDNUM, -RANDNUM ) );
	auto v_two = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( RANDNUM, -RANDNUM, -RANDNUM ) );
	auto v_three = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( RANDNUM, RANDNUM, -RANDNUM ) );
	auto v_four = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( -RANDNUM, RANDNUM, -RANDNUM ) );
	auto v_five = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( -RANDNUM, -RANDNUM, RANDNUM ) );
	auto v_six = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( RANDNUM, -RANDNUM, RANDNUM ) );
	auto v_seven = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( RANDNUM, RANDNUM, RANDNUM ) );
	auto v_eight = mesh->AddVertex( scene::mesh::Vec3Mesh::vec3_t( -RANDNUM, RANDNUM, RANDNUM ) );

	mesh->AddSurface( v_one, v_four, v_three );
	mesh->AddSurface( v_three, v_two, v_one );
	mesh->AddSurface( v_five, v_six, v_seven );
	mesh->AddSurface( v_seven, v_eight, v_five );

	mesh->AddSurface( v_one, v_two, v_six );
	mesh->AddSurface( v_six, v_five, v_one );
	mesh->AddSurface( v_three, v_four, v_eight );
	mesh->AddSurface( v_eight, v_seven, v_three );

	mesh->AddSurface( v_seven, v_six, v_two );
	mesh->AddSurface( v_two, v_three, v_seven );
	mesh->AddSurface( v_one, v_five, v_eight );
	mesh->AddSurface( v_eight, v_four, v_one );

	return mesh;
}

void WorldTestTask::Start() {
	m_scene = new scene::Scene( "RendererTest", scene::SCENE_TYPE_PERSP );

	m_camera = new scene::Camera();

	m_scene->SetCamera( m_camera );

	m_planet = new scene::actor::MeshActor( "Planet", testmesh3() );
	m_planet->SetPosition( types::Vec3( 0.0, 0.0, -2 ) );
	m_scene->AddActor( m_planet );
	g_engine->GetRenderer()->AddScene( m_scene );

}

void WorldTestTask::Stop() {
	g_engine->GetRenderer()->RemoveScene( m_scene );
	delete m_planet;

	delete m_camera;
	delete m_scene;
}

void WorldTestTask::Iterate() {

	//m_camera->SetAngleX( m_camera->GetAngleX() + 0.002 );

	float speed = 0.3;

	m_planet->SetAngleX( m_planet->GetAngleX() + 0.052 * speed );
	m_planet->SetAngleY( m_planet->GetAngleY() + 0.033 * speed );
	m_planet->SetAngleZ( m_planet->GetAngleZ() + 0.021 * speed );

	if ( rand()%3000 == 0 ) {
		m_scene->RemoveActor( m_planet );
		delete m_planet;
		m_planet = new scene::actor::MeshActor( "Planet", testmesh3() );
		m_planet->SetPosition( types::Vec3( 0.0, 0.0, -2 ) );
		m_scene->AddActor( m_planet );
	}

	/*if ( rand()%50 == 0 ) {
		if ( rand()%2 )
			m_scene->SetCamera( NULL );
		else
			m_scene->SetCamera( m_camera );
	}*/
}

} /* namespace test */
} /* namespace task */
