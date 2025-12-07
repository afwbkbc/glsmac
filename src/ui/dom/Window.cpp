#include "Window.h"

#include "Panel.h"
#include "Text.h"
#include "Surface.h"
#include "ui/UI.h"
#include "ui/geometry/Geometry.h"
#include "gse/value/Int.h"
#include "gse/value/Bool.h"

namespace ui {
namespace dom {

Window::Window( DOM_ARGS )
	: Panel( DOM_ARGS_PASS, "window", false ) {

	m_geometry->SetAlign( geometry::Geometry::ALIGN_CENTER );

	{
		m_blocker = new Surface( GSE_CALL, ui, this, {} );
		Embed( m_blocker, false );
		auto* g = m_blocker->GetGeometry();
		g->SetPosition( geometry::Geometry::POSITION_ABSOLUTE );
		g->SetTop( 0 );
		g->SetBottom( 0 );
		g->SetLeft( 0 );
		g->SetRight( 0 );
		// block clickthroughs
		m_blocker->On( GSE_CALL, "*", NATIVE_CALL( this ) {
			return VALUE( gse::value::Bool,, true );
		} ) );
	}
	{
		m_body = new Container( GSE_CALL, ui, this, {}, "window_body", true, true );
		Embed( m_body );
		auto* g = m_body->GetGeometry();
		g->SetAlign( geometry::Geometry::ALIGN_TOP_CENTER );
		g->SetLeft( 0 );
		g->SetRight( 0 );
		g->SetBottom( 0 );
	}
	{
		m_header = new Panel( GSE_CALL, ui, this, {}, "window_header" );
		Embed( m_header );
		auto* g = m_header->GetGeometry();
		g->SetAlign( geometry::Geometry::ALIGN_TOP_CENTER );
		g->SetLeft( 0 );
		g->SetRight( 0 );
		g->SetTop( 0 );
		{
			m_title = new Text( GSE_CALL, ui, m_header, {} );
			m_header->Embed( m_title );
			auto* g = m_title->GetGeometry();
			g->SetAlign( geometry::Geometry::ALIGN_CENTER );
			g->SetLeft( 0 );
			g->SetRight( 0 );
			g->SetTop( 0 );
			g->SetBottom( 0 );
		}
	}

	SetHeaderHeight( 20 );

	Property(
		GSE_CALL, "blocker", gse::Value::T_STRING, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* v ) {
			WrapSetStatic( m_blocker, "background", v, GSE_CALL );
			m_blocker->Show();
		},
		[ this ]( GSE_CALLABLE ) {
			m_blocker->Hide();
		}
	);

	ForwardProperty( GSE_CALL, "header_background", "background", m_header );
	ForwardProperty( GSE_CALL, "header_border", "border", m_header );
	Property(
		GSE_CALL, "header_height", gse::Value::T_INT, VALUE( gse::value::Int, , 20 ), PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			SetHeaderHeight( ( (gse::value::Int*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetHeaderHeight( 0 );
		}
	);
	Property(
		GSE_CALL, "header_padding", gse::Value::T_INT, nullptr, PF_NONE,
		[ this ]( GSE_CALLABLE, gse::Value* const v ) {
			m_header->GetGeometry()->SetPadding( ( (gse::value::Int*)v )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			m_header->GetGeometry()->SetPadding( 0 );
		}
	);
	ForwardProperty( GSE_CALL, "title", "text", m_title );
	ForwardProperty( GSE_CALL, "header_color", "color", m_title );
	ForwardProperty( GSE_CALL, "header_font", "font", m_title );
	ForwardProperty( GSE_CALL, "header_transform", "transform", m_title );

	ForwardFactories( GSE_CALL, m_body );
}

void Window::SetHeaderHeight( const coord_t px ) {
	m_header->GetGeometry()->SetHeight( px + 1 );
	m_body->GetGeometry()->SetTop( px );
}

void Window::SetTitle( const std::string& title ) {
	m_title->SetText( title );
}

}
}
