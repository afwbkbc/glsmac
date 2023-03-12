#include "FPSCounter.h"

#include "engine/Engine.h"

using namespace ui::object;

namespace game {
namespace ui {

FPSCounter::FPSCounter()
	: Label( "FPSCounter" )
{
	//
}
	
void FPSCounter::Create() {
	Label::Create();
	
	SetText( "" );
	
	m_timer.SetInterval( 1000 );
}

void FPSCounter::Iterate() {
	Label::Iterate();
	
	while ( m_timer.Ticked() ) {
		SetText( std::to_string( g_engine->GetGraphics()->GetFramesCountAndReset() ) );
	}
}

void FPSCounter::Destroy() {
	m_timer.Stop();
	
	Label::Destroy();
}

}
}
