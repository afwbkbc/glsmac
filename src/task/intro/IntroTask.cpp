#include <cmath>

#include "IntroTask.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace task {

#define RANDNUM ( (float)rand() / (float)RAND_MAX )

void IntroTask::Start() {
	m_image = new Image( "logo.pcx" );
	m_image->SetAlign( UIObject::ALIGN_CENTER );
	g_engine->GetUI()->AddObject( m_image );

}

void IntroTask::Stop() {
	g_engine->GetUI()->RemoveObject( m_image );
}

void IntroTask::Iterate() {

}

} /* namespace task */
