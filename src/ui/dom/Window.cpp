#include "Window.h"

#include "Panel.h"
#include "Text.h"
#include "Surface.h"
#include "ui/UI.h"
#include "ui/geometry/Geometry.h"
#include "gse/type/Int.h"
#include "gse/type/Bool.h"

namespace ui {
namespace dom {

Window::Window( DOM_ARGS )
	: Container( DOM_ARGS_PASS, "window", false ) {

	m_geometry->SetAlign( geometry::Geometry::ALIGN_CENTER );

	{
		m_blocker = new Surface( GSE_CALL, ui, this, {} );
		Embed( m_blocker );
		auto* g = m_blocker->GetGeometry();
		g->SetPosition( geometry::Geometry::POSITION_ABSOLUTE );
		g->SetTop( 0 );
		g->SetBottom( 0 );
		g->SetLeft( 0 );
		g->SetRight( 0 );
	}
	{
		m_body = new Panel( GSE_CALL, ui, this, {}, "window_body", true );
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
			m_title->SetText( "ASDASD" );
		}
	}

	SetHeaderHeight( 20 );

	Property(
		GSE_CALL, "blocker", gse::type::Type::T_STRING, VALUE( gse::type::Undefined ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			WrapSetStatic( m_blocker, "background", v, GSE_CALL );
		},
		[ this ]( GSE_CALLABLE ) {
			WrapSetStatic( m_blocker, "background", VALUE( gse::type::Undefined ), GSE_CALL );
		}
	);

	ForwardProperty( GSE_CALL, "background", m_body );
	ForwardProperty( GSE_CALL, "border", m_body );

	ForwardProperty( GSE_CALL, "header_background", "background", m_header );
	ForwardProperty( GSE_CALL, "header_border", "border", m_header );
	Property(
		GSE_CALL, "header_height", gse::type::Type::T_INT, VALUE( gse::type::Int, 20 ), PF_NONE,
		[ this ]( GSE_CALLABLE, const gse::Value& v ) {
			SetHeaderHeight( ( (gse::type::Int*)v.Get() )->value );
		},
		[ this ]( GSE_CALLABLE ) {
			SetHeaderHeight( 0 );
		}
	);
	ForwardProperty( GSE_CALL, "title", "text", m_title );
	ForwardProperty( GSE_CALL, "header_color", "color", m_title );
	ForwardProperty( GSE_CALL, "header_font", "font", m_title );

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
