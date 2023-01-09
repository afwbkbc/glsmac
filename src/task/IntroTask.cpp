#include <cmath>

#include "IntroTask.h"

#include "scene/actor/ImageActor.h"
#include "types/Texture.h"
#include "engine/Engine.h"

using namespace ui::object;

namespace task {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void IntroTask::Start() {
	m_logo_image = g_engine->GetTextureLoader()->LoadTexture("logo.pcx");
	
	m_panel = new Panel;

	//m_panel->SetBackgroundColor( types::Color( 0.0, 0.2, 0.0, 0.3 ) );
	m_panel->SetBackgroundTexture( m_logo_image );

	m_panel->SetWidth(500);
	m_panel->SetHeight(300);
	
	m_panel->SetAlign( UIObject::ALIGN_CENTER );

	g_engine->GetUI()->AddObject( m_panel );

}

void IntroTask::Stop() {
	g_engine->GetUI()->RemoveObject( m_panel );
}

void IntroTask::Iterate() {

}

} /* namespace task */
