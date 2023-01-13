#include "Overlay.h"

#include "engine/Engine.h"

using namespace std;

namespace ui {
namespace debug {

void Overlay::Create() {
	UIContainer::Create();
	
	const size_t font_size = 16;
	const float opacity = 0.7;
	
	m_stats_font = g_engine->GetFontLoader()->LoadFont( "arialn.ttf", font_size );
	
	size_t stat_line = 0;
	#define D( _stat ) \
		m_##_stats_label_##_stat = new Label(); \
		m_##_stats_label_##_stat->SetFont( m_stats_font ); \
		m_##_stats_label_##_stat->SetTextColor( { 1.0, 1.0, 1.0, opacity } ) ; \
		m_##_stats_label_##_stat->SetAlign( UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT ); \
		m_##_stats_label_##_stat->SetLeft( 3 ); \
		m_##_stats_label_##_stat->SetTop( (stat_line++) * font_size ); \
		g_engine->GetUI()->AddObject( m_##_stats_label_##_stat );
	DEBUG_STATS;
	#undef D
	
	m_stats_timer.SetInterval( 1000 ); // track stats/second
}

void Overlay::Destroy() {
	
	m_stats_timer.Stop();

	#define D( _stat ) \
		g_engine->GetUI()->RemoveObject( m_##_stats_label_##_stat );
	DEBUG_STATS;
	#undef D
	
	UIContainer::Destroy();
}

void Overlay::Iterate() {
	UIContainer::Iterate();

	if (m_stats_timer.Ticked()) {
		size_t total;
		size_t current;
		
		#define D( _stat ) \
			DEBUG_STAT_GET( _stat, total, current ); \
			m_##_stats_label_##_stat->SetText( (string) #_stat + " : " + to_string( total ) + " (" + to_string( current ) + "/sec)" );
		DEBUG_STATS;
		#undef D	
	}
}	

}
}
