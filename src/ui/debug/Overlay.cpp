#include "Overlay.h"

#include "engine/Engine.h"

using namespace std;

using namespace types;

namespace ui {

using namespace object;

namespace debug {

void Overlay::Create() {
	UIContainer::Create();

	NEW( m_background_texture, types::Texture, "OverlayBackground", 1, 1 );
	m_background_texture->SetPixel( 0, 0, { 0.0, 0.0, 0.0, 0.5 } );
	
	NEW( m_background, Surface );
	m_background->SetAlign( UIObject::ALIGN_TOP );
	m_background->SetLeft( 0 );
	m_background->SetRight( 0 );
	m_background->SetTop( 0 );
	m_background->SetHeight( 200 );
	m_background->SetZIndex( 0.8 );
	m_background->SetTexture( m_background_texture );
	
	g_engine->GetUI()->AddObject( m_background );
	
	m_font_size = 16;
	m_memory_stats_lines = 10;
	
	m_stats_font = g_engine->GetFontLoader()->LoadFont( "arialn.ttf", m_font_size );
	
	size_t stat_line = 0;
	#define D( _stat ) \
		NEW( m_##_stats_label_##_stat, Label ); \
		ActivateLabel( m_##_stats_label_##_stat, 3, (stat_line++) * m_font_size );
	DEBUG_STATS;
	#undef D
	
	for ( int i = 0 ; i < m_memory_stats_lines ; i++ ) {
		NEWV( label, Label );
		ActivateLabel( label, 263, i * ( m_font_size + 1 ) );
		m_memory_stats_labels.push_back( label );
	}
	
	m_stats_timer.SetInterval( 1000 ); // track stats/second
}

void Overlay::Destroy() {
	
	m_stats_timer.Stop();

	for ( auto& it : m_memory_stats_labels ) {
		Log( "removing label " + it->GetName() );
		g_engine->GetUI()->RemoveObject( it );
	}
	
	#define D( _stat ) \
		g_engine->GetUI()->RemoveObject( m_##_stats_label_##_stat );
	DEBUG_STATS;
	#undef D
	
	g_engine->GetUI()->RemoveObject( m_background );
	DELETE( m_background_texture );
	
	UIContainer::Destroy();
}

void Overlay::Iterate() {
	UIContainer::Iterate();

	if (m_stats_timer.Ticked()) {
		ssize_t total;
		ssize_t current;
		
		// common statistics
		#define D( _stat ) \
			DEBUG_STAT_GET( _stat, total, current ); \
			m_##_stats_label_##_stat->SetText( (string) #_stat + " : " + to_string( total ) + " ( " + ( current > 0 ? "+" : "" ) + to_string( current ) + "/sec )" );
		DEBUG_STATS;
		#undef D

		// memory statistics
		const auto stats = g_memory_watcher.GetLargestMemoryConsumerClasses( m_memory_stats_lines );
		for ( auto i = 0 ; i < stats.size() ; i++ ) {
			string size = to_string( stats[i].size ) + "b";
			size.insert(size.begin(), 14 - size.length() * 1.5 , ' '); 
			string count = "(" + to_string( stats[i].count ) + ")";
			count.insert(count.begin(), 6 - count.length() , ' '); 
			m_memory_stats_labels[i]->SetText( size + "  " + count + "  " + stats[i].key );
		}
	}
}	

// not using themes because overlay should be independent of them
void Overlay::ActivateLabel( Label* label, const size_t left, const size_t top ) {
	Log( "created label " + label->GetName() );
	label->SetFont( m_stats_font );
	label->SetTextColor( { 0.5, 1.0, 0.5, 1.0 } );
	label->SetLeft( left );
	label->SetTop( top );
	label->SetAlign( UIObject::ALIGN_TOP | UIObject::ALIGN_LEFT );
	g_engine->GetUI()->AddObject( label );
}

}
}
