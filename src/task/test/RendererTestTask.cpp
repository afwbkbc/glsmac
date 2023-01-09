#include <cmath>
#include <cstdlib>

#include "RendererTestTask.h"

#include "scene/mesh/Vec2Mesh.h"
#include "scene/actor/MeshActor.h"
#include "engine/Engine.h"

namespace task {
namespace test {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

scene::mesh::Mesh *testmesh2() {
	auto *mesh = new scene::mesh::Vec2Mesh;
	auto v_one = mesh->AddVertex( scene::mesh::Vec2Mesh::vec2_t( -RANDNUM, -RANDNUM ) );
	auto v_two = mesh->AddVertex( scene::mesh::Vec2Mesh::vec2_t( RANDNUM, -RANDNUM ) );
	auto v_three = mesh->AddVertex( scene::mesh::Vec2Mesh::vec2_t( RANDNUM, RANDNUM ) );
	auto v_four = mesh->AddVertex( scene::mesh::Vec2Mesh::vec2_t( -RANDNUM, RANDNUM ) );
	mesh->AddSurface( v_one, v_two, v_three );
	mesh->AddSurface( v_three, v_four, v_one );
	return mesh;
}

void RendererTestTask::Start() {
	m_scene = new scene::Scene( "RendererTest", scene::SCENE_TYPE_ORTHO );
	m_logo = new scene::actor::MeshActor( "Logo", testmesh2() );
	m_scene->AddActor( m_logo );
	g_engine->GetRenderer()->AddScene( m_scene );
}

void RendererTestTask::Stop() {
	g_engine->GetRenderer()->RemoveScene( m_scene );
	delete m_logo;
	delete m_scene;
}

void RendererTestTask::Iterate() {
	if ( rand() % 50 == 0 ) {
		m_scene->RemoveActor( m_logo );
		delete m_logo;
		auto *actor = new scene::actor::MeshActor( "Logo", testmesh2() );
		actor->SetPosition( scene::actor::Actor::vec3_t( 0.0, 0.0, RANDNUM ) );
		actor->SetTintColor( types::Color( RANDNUM, RANDNUM, RANDNUM, RANDNUM ) );
		m_logo = actor;
		m_scene->AddActor( m_logo );
	}
}

} /* namespace test */
} /* namespace task */
