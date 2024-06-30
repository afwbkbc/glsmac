#ifdef DEBUG

#include "DebugOverlay.h"

#include "engine/Engine.h"
#include "config/Config.h"
#include "loader/font/FontLoader.h"
#include "types/texture/Texture.h"
#include "ui/UI.h"
#include "ui/event/Types.h"
#include "ui/object/Label.h"
#include "ui/object/Surface.h"

namespace debug {

void DebugOverlay::Start() {

	DEBUG_STATS_SET_RO();

	m_font_size = 16;
	m_memory_stats_lines = 10;

	m_stats_font = g_engine->GetFontLoader()->LoadFont( resource::TTF_ARIALN, m_font_size );

	NEW( m_background_texture, types::texture::Texture, "OverlayBackground", 1, 1 );
	m_background_texture->SetPixel(
		0, 0, {
			0.0,
			0.0,
			0.0,
			0.7
		}
	);

	m_toggle_handler = g_engine->GetUI()->AddGlobalEventHandler(
		ui::event::EV_KEY_DOWN, EH( this ) {
			if ( data->key.code == ui::event::K_GRAVE && !data->key.modifiers ) {
				Toggle();
				return true;
			}
			return false;
		}, ui::UI::GH_BEFORE
	);

	DEBUG_STATS_SET_RW();

}

void DebugOverlay::Stop() {

	Hide();

	g_engine->GetUI()->RemoveGlobalEventHandler( m_toggle_handler );

	DELETE( m_background_texture );
}

void DebugOverlay::Show() {
	if ( !m_is_visible ) {
		Log( "Showing" );

		DEBUG_STATS_SET_RO();

		size_t stat_line = 0;
#define D( _stat ) \
            NEW( m_##_stats_label_##_stat, ui::object::Label ); \
            ActivateLabel( m_##_stats_label_##_stat, 3, (stat_line++) * ( m_font_size + 1 ) );
		DEBUG_STATS;
#undef D

		for ( int i = 0 ; i < m_memory_stats_lines ; i++ ) {
			NEWV( label, ui::object::Label );
			ActivateLabel( label, 340, i * ( m_font_size + 1 ) );
			m_memory_stats_labels.push_back( label );
		}

		NEW( m_background_left, ui::object::Surface );
		m_background_left->SetAlign( ui::ALIGN_TOP | ui::ALIGN_LEFT );
		m_background_left->SetLeft( 0 );
		m_background_left->SetRight( 0 );
		m_background_left->SetTop( 0 );
		m_background_left->SetHeight( ( stat_line ) * 18 );
		m_background_left->SetWidth( 330 );
		m_background_left->SetZIndex( 0.9 );
		m_background_left->SetTexture( m_background_texture );
		g_engine->GetUI()->AddObject( m_background_left );

		if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_MEMORYDEBUG ) ) {
			NEW( m_background_middle, ui::object::Surface );
			m_background_middle->SetAlign( ui::ALIGN_TOP | ui::ALIGN_LEFT );
			m_background_middle->SetLeft( 340 );
			m_background_middle->SetRight( 0 );
			m_background_middle->SetTop( 0 );
			m_background_middle->SetHeight( m_memory_stats_lines * 18 );
			m_background_middle->SetWidth( 330 );
			m_background_middle->SetZIndex( 0.9 );
			m_background_middle->SetTexture( m_background_texture );
			g_engine->GetUI()->AddObject( m_background_middle );
		}

		m_stats_timer.SetInterval( 1000 ); // track stats/second

		m_is_visible = true;

		DEBUG_STATS_SET_RW();

		ClearStats();
		Refresh();
	}
}

void DebugOverlay::Hide() {
	if ( m_is_visible ) {
		Log( "Hiding" );

		m_is_visible = false;

		DEBUG_STATS_SET_RO();

		m_stats_timer.Stop();

		for ( auto& it : m_memory_stats_labels ) {
			Log( "removing label " + it->GetName() );
			g_engine->GetUI()->RemoveObject( it );
		}
		m_memory_stats_labels.clear();

#define D( _stat ) \
            g_engine->GetUI()->RemoveObject( m_##_stats_label_##_stat );
		DEBUG_STATS;
#undef D

		g_engine->GetUI()->RemoveObject( m_background_left );

		if ( g_engine->GetConfig()->HasDebugFlag( config::Config::DF_MEMORYDEBUG ) ) {
			g_engine->GetUI()->RemoveObject( m_background_middle );
		}

		DEBUG_STATS_SET_RW();
	}
}

void DebugOverlay::Toggle() {

	if ( m_is_visible ) {
		Hide();
	}
	else {
		Show();
	}

}

void DebugOverlay::Refresh() {

	if ( m_is_visible ) {

		DEBUG_STATS_SET_RO();

		ssize_t total;
		ssize_t current;

		// common statistics
#define D( _stat ) \
            DEBUG_STAT_GET( _stat, total, current ); \
            m_##_stats_label_##_stat->SetText( (std::string) #_stat + " : " + std::to_string( total ) + " ( " + ( current > 0 ? "+" : "" ) + std::to_string( current ) + "/sec )" );
		DEBUG_STATS;
#undef D

		// memory statistics
		const auto stats = g_memory_watcher->GetLargestMemoryConsumerClasses( m_memory_stats_lines );
		for ( auto i = 0 ; i < stats.size() ; i++ ) {
			std::string size = std::to_string( stats[ i ].size ) + "b";
			size.insert( size.begin(), 14 - size.length() * 1.5, ' ' );
			std::string count = "(" + std::to_string( stats[ i ].count ) + ")";
			count.insert( count.begin(), 6 - count.length(), ' ' );
			m_memory_stats_labels[ i ]->SetText( size + "  " + count + "  " + stats[ i ].key );
		}

		DEBUG_STATS_SET_RW();

	}
}

void DebugOverlay::ClearStats() {
	// common statistics
#define D( _stat ) \
        DEBUG_STAT_CLEAR( _stat );
	DEBUG_STATS;
#undef D
}

void DebugOverlay::Iterate() {

	if ( m_is_visible ) {

		if ( m_stats_timer.HasTicked() ) {
			Refresh();

			ClearStats();
		}

	}
}

// not using themes because overlay should be independent of them
void DebugOverlay::ActivateLabel( ui::object::Label* label, const size_t left, const size_t top ) {
	DEBUG_STATS_SET_RO();

	Log( "created label " + label->GetName() );
	label->SetFont( m_stats_font );
	label->SetTextColor(
		{
			0.5,
			1.0,
			0.5,
			1.0
		}
	);
	label->SetLeft( left );
	label->SetTop( top );
	label->SetAlign( ui::ALIGN_TOP | ui::ALIGN_LEFT );
	g_engine->GetUI()->AddObject( label );

	DEBUG_STATS_SET_RW();
}

}

#endif

