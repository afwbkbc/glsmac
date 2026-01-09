#include "UI.h"

#include <algorithm>

#include "dom/Root.h"
#include "dom/Widget.h"
#include "types/texture/Texture.h"
#include "scene/Scene.h"
#include "engine/Engine.h"
#include "graphics/Graphics.h"
#include "input/Input.h"
#include "Class.h"
#include "gse/ExecutionPointer.h"
#include "gc/Space.h"
#include "dom/Focusable.h"
#include "gse/value/Bool.h"
#include "geometry/Geometry.h"

namespace ui {

UI::UI( GSE_CALLABLE )
	: gse::GCWrappable( gc_space )
	, m_ctx( ctx )
	, m_gc_space( gc_space )
	, m_scene(new scene::Scene( "Scene::UI", scene::SCENE_TYPE_UI ) ){

	m_clamp.x.SetRange(
		{
			{ 0.0,  (float)g_engine->GetGraphics()->GetViewportWidth() },
			{ -1.0, 1.0 }
		}
	);
	m_clamp.x.SetOverflowAllowed( true );

	m_clamp.y.SetRange(
		{
			{ 0.0,  (float)g_engine->GetGraphics()->GetViewportHeight() },
			{ -1.0, 1.0 }
		}
	);
	m_clamp.y.SetOverflowAllowed( true );
	m_clamp.y.SetInversed( true );

	g_engine->GetGraphics()->AddOnWindowResizeHandler(
		this, RH( this ) {
			Resize();
		}
	);

	g_engine->GetGraphics()->AddScene( m_scene );
	m_root = new dom::Root( GSE_CALL, this );
	Resize();

	g_engine->GetInput()->AddHandler( this, [ this, gc_space, ctx, si ]( const input::Event& event ){
		bool result = false;
		m_gc_space->Accumulate( this, [ this, &ctx, &gc_space, &si, &event, &result ] () {
			gse::ExecutionPointer ep;
			if ( event.type == input::EV_MOUSE_MOVE ) {
				m_last_mouse_position = {
					event.data.mouse.x,
					event.data.mouse.y
				};
			}
			for ( const auto& it : m_global_handlers ) {
				if ( it.second( GSE_CALL, event ) ) {
					result = true;
					break;
				}
			}
			if (
				!result &&
				event.type == input::EV_KEY_DOWN &&
				event.data.key.modifiers == input::KM_NONE &&
				event.data.key.code == input::K_TAB &&
				!m_focusable_elements.empty()
				) {
				FocusNext();
				result = true;
			}
			if ( !result ) {
				result = m_root->ProcessEvent( GSE_CALL, event );
			}
		});
		return result;
	});
}

UI::~UI() {

	g_engine->GetInput()->RemoveHandler( this );

	g_engine->GetGraphics()->RemoveScene( m_scene );
	delete m_scene;

	g_engine->GetGraphics()->RemoveOnWindowResizeHandler( this );

}

void UI::Iterate() {
	for ( const auto& it : m_iterables ) {
		it.second();
	}
}

WRAPIMPL_BEGIN( UI )
	WRAPIMPL_PROPS
	WRAPIMPL_TRIGGERS
			{
				"root",
				m_root->Wrap( GSE_CALL, true )
			},
			{
				"class",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS_MIN_MAX( 1, 3 ); // .class(name) or .class(name, properties) or .class(name, parent) or .class(name, parent, properties)
					N_GETVALUE( name, 0, String );
					auto it = m_classes.find( name );
					if ( it == m_classes.end() ) {
						Log( "Creating UI class: " + name );
						it = m_classes.insert({ name, new ui::Class( gc_space, this, name ) }).first;
						if ( arguments.size() >= 2 ) {
							const gse::value::object_properties_t* properties = nullptr;
							const std::string* parent_class = nullptr;
							if ( arguments.size() == 2 ) {
								if ( arguments.at(1)->type == gse::Value::T_STRING ) {
									N_GETVALUE( s, 1, String );
									parent_class = &s;
								}
								else {
									N_GETVALUE( p, 1, Object );
									properties = &p;
								}
							}
							else {
								N_GETVALUE( s, 1, String );
								N_GETVALUE( p, 2, Object );
								parent_class = &s;
								properties = &p;
							}
							if ( parent_class ) {
								it->second->SetParentClass( GSE_CALL, *parent_class );
							}
							if ( properties ) {
								it->second->SetProperties( GSE_CALL, *properties );
							}
						}
					}
					else if ( arguments.size() >= 2 ) {
						// passing parent class or properties in constructor works only if class didn't exist yet
						GSE_ERROR( gse::EC.UI_ERROR, "Class '" + name + "' already exists. Use .set() instead of constructor to update properties of existing classes." );
					}
					return it->second->Wrap( GSE_CALL, true );
				} )
			},
			{
				"error",
				NATIVE_CALL( this ) {
					N_EXPECT_ARGS( 2 );
					N_GETVALUE( errmsg, 0, String );
					N_GET_CALLABLE( on_close, 1 );
					Persist( on_close );

					Trigger( GSE_CALL, "error", ARGS_F( this, &errmsg, on_close ) {
						{
							"error",
							VALUEEXT( gse::value::String, m_gc_space, errmsg ),
						},
						{
							"on_close",
							VALUE( gse::callable::Native,, ctx, [ this, on_close ]( GSE_CALLABLE, const gse::value::function_arguments_t& arguments ) -> gse::Value* {
								if ( IsPersisted( on_close ) ) {
									on_close->Run( GSE_CALL, {} );
									Unpersist( on_close );
								}
								return VALUE( gse::value::Bool,, true );
							} )
						},
					}; } );
					return VALUE( gse::value::Undefined );
				} )
			},
		};
WRAPIMPL_END_PTR()

void UI::Resize() {
	const auto& g = g_engine->GetGraphics();
/*#ifdef DEBUG
			for ( auto& it : m_debug_frames ) {
				ResizeDebugFrame( it.first, &it.second );
			}
#endif*/
	m_clamp.x.SetSrcRange(
		{
			0.0,
			(float)g->GetViewportWidth()
		}
	);
	m_clamp.y.SetSrcRange(
		{
			0.0,
			(float)g->GetViewportHeight()
		}
	);
	m_root->Resize( g->GetViewportWidth(), g->GetViewportHeight() );
}

const types::mesh::coord_t UI::ClampX( const coord_t& x ) const {
	return m_clamp.x.Clamp( x );
}

const types::mesh::coord_t UI::ClampY( const coord_t& y ) const {
	return m_clamp.y.Clamp( y );
}

const types::Vec2< types::mesh::coord_t > UI::ClampXY( const types::Vec2< coord_t >& xy ) const {
	return {
		m_clamp.x.Clamp( xy.x ),
		m_clamp.y.Clamp( xy.y ),
	};
}

ui::Class* const UI::GetClass( const std::string& name ) const {
	const auto& it = m_classes.find( name );
	if ( it != m_classes.end() ) {
		return it->second;
	}
	else {
		return nullptr;
	}
}

const types::Vec2< ssize_t >& UI::GetLastMousePosition() const {
	return m_last_mouse_position;
}

void UI::Destroy( GSE_CALLABLE ) {
	m_root->Destroy( GSE_CALL );
}

void UI::GetReachableObjects( std::unordered_set< gc::Object* >& reachable_objects ) {
	gse::GCWrappable::GetReachableObjects( reachable_objects );

	GC_DEBUG_BEGIN( "UI" );

	GC_REACHABLE( m_root );

	GC_DEBUG_BEGIN( "classes" );
	for ( const auto& it : m_classes ) {
		GC_REACHABLE( it.second );
	}
	GC_DEBUG_END();

	{
		GC_DEBUG_BEGIN( "widgets" );
		std::lock_guard guard( m_widgets_mutex );
		for ( const auto& it : m_widgets ) {
			for ( const auto& it2 : it.second ) {
				GC_REACHABLE( it2.first );
				if ( it2.second.data ) {
					GC_REACHABLE( it2.second.data );
				}
			}
		}
		GC_DEBUG_END();
	}

	GC_DEBUG_END();
}

void UI::AddIterable( const dom::Object* const obj, const f_iterable_t& f ) {
	ASSERT( m_iterables.find( obj ) == m_iterables.end(), "iterable already exists" );
	m_iterables.insert( {obj, f } );
}

void UI::RemoveIterable( const dom::Object* const obj ) {
	ASSERT( m_iterables.find( obj ) != m_iterables.end(), "iterable not found" );
	m_iterables.erase( obj );
}

gc::Space* const UI::GetGCSpace() const {
	return m_gc_space;
}

void UI::WithGSE( const gse_func_t& f ) {
	m_gc_space->Accumulate(
		nullptr,
		[ this, f ]() {
			gse::ExecutionPointer ep;
			f( m_gc_space, m_ctx, {}, ep );
		}
	);
}

dom::Root* const UI::GetRoot() const {
	return m_root;
}

void UI::AddFocusable( dom::Focusable* const element ) {
	ASSERT( element, "element is null" );
	ASSERT( std::find( m_focusable_elements.begin(), m_focusable_elements.end(), element ) == m_focusable_elements.end(), "focusable already exists");
	ASSERT( m_focusable_elements_idx.find( element ) == m_focusable_elements_idx.end(), "focusable idx already exists");
	m_focusable_elements_idx.insert({ element, m_focusable_elements_idx.size() });
	m_focusable_elements.push_back( element );
	if ( !m_focused_element ) {
		Focus( element );
	}
}

void UI::RemoveFocusable( dom::Focusable* const element ) {
	ASSERT( element, "element is null" );
	const auto& it = m_focusable_elements_idx.find( element );
	ASSERT( it != m_focusable_elements_idx.end(), "focusable idx not found");
	ASSERT( std::find( m_focusable_elements.begin(), m_focusable_elements.end(), element ) != m_focusable_elements.end(), "focusable not found");
	const auto idx = it->second;
	ASSERT( idx < m_focusable_elements.size(), "focusable idx overflow" );
	m_focusable_elements.erase( m_focusable_elements.begin() + idx );
	m_focusable_elements_idx.erase( it );
	for ( auto& e : m_focusable_elements_idx ) {
		if ( e.second >= idx ) {
			e.second--;
		}
	}
	if ( element == m_focused_element ) {
		Defocus( element );
		FocusNext();
	}
}

void UI::Focus( dom::Focusable* const element ) {
	ASSERT( element, "element is null" );
	if ( m_focused_element ) {
		Defocus( m_focused_element );
	}
	m_focused_element = element;
	m_focused_element->OnFocus();
}

void UI::FocusNext() {
	ASSERT( !m_focused_element || !m_focusable_elements.empty(), "something is focused but focusable elements empty" );
	ASSERT( m_focusable_elements.size() == m_focusable_elements_idx.size(), "focusables size mismatch" );
	const auto idx = m_focused_element ? m_focusable_elements_idx.at( m_focused_element ) : 0;
	const auto nextidx = idx < m_focusable_elements_idx.size() - 1 ? idx + 1 : 0;
	if ( nextidx < m_focusable_elements.size() ) {
		Focus( m_focusable_elements.at( nextidx ) );
	}
}

const size_t UI::AddGlobalHandler( const global_handler_t& global_handler ) {
	const auto handler_id = m_next_global_handler_id++;
	ASSERT( m_global_handlers.find( handler_id ) == m_global_handlers.end(), "global handler already set" );
	m_global_handlers.insert({ handler_id, global_handler } );
	return handler_id;
}

void UI::RemoveGlobalHandler( const size_t handler_id ) {
	ASSERT( m_global_handlers.find( handler_id ) != m_global_handlers.end(), "global handler not found" );
	m_global_handlers.erase( handler_id );
}

void UI::RegisterWidget( const widget_type_t type, const widget_config_t& config ) {
	ASSERT( m_widget_configs.find( type ) == m_widget_configs.end(), "widget type already registered" );
	ASSERT( m_widget_strs.find( config.str ) == m_widget_strs.end(), "widget str already registered" );
	m_widget_configs.insert({ type, config });
	m_widget_strs.insert({ config.str, type } );
}

void UI::UnregisterWidget( const ui::widget_type_t type ) {
	const auto& it = m_widget_configs.find( type );
	ASSERT( it != m_widget_configs.end(), "widget config not registered" );
	ASSERT( m_widget_strs.find( it->second.str ) != m_widget_strs.end(), "widget str not registered" );
	m_widget_strs.erase( it->second.str );
	m_widget_configs.erase( type );
}

const widget_type_t UI::GetWidgetTypeByString( GSE_CALLABLE, const std::string& str ) const {
	const auto& it = m_widget_strs.find( str );
	if ( it == m_widget_strs.end() ) {
		std::string errstr = "Unknown widget type: " + str + " . Available types:";
		for ( const auto& s : m_widget_strs ) {
			errstr += " " +s.first;
		}
		GSE_ERROR( gse::EC.GAME_ERROR, errstr );
	}
	return it->second;
}

void UI::AttachWidget( dom::Widget* const widget, const widget_type_t type ) {
	std::lock_guard guard( m_widgets_mutex );
	ASSERT( m_widget_configs.find( type ) != m_widget_configs.end(), "widget config not found" );
	ASSERT( m_widget_types.find( widget ) == m_widget_types.end(), "widget already attached" );
	auto it = m_widgets.find( type );
	if ( it == m_widgets.end() ) {
		it = m_widgets.insert({ type, {} } ).first;
	}
	ASSERT( it->second.find( widget ) == it->second.end(), "widget already attached" );
	widget_state_t state = {};
	state.config = &m_widget_configs.at( type );
	it->second.insert( { widget, state } );
	m_widget_types.insert({ widget, type } );
	ASSERT( state.config->f_init, "widget init not defined" );
	state.config->f_init( widget );
}

void UI::DetachWidget( dom::Widget* const widget ) {
	std::lock_guard guard( m_widgets_mutex );
	const auto& it = m_widget_types.find( widget );
	ASSERT( it != m_widget_types.end(), "widget not attached" );
	const auto& widgets = m_widgets.find( it->second );
	ASSERT( widgets != m_widgets.end(), "widget type not attached" );
	ASSERT( widgets->second.find( widget ) != widgets->second.end(), "widget not attached" );
	widget->Clear();
	widgets->second.erase( widget );
	if ( widgets->second.empty() ) {
		m_widgets.erase( it->second );
	}
	m_widget_types.erase( widget );
}

void UI::ValidateWidgetData( GSE_CALLABLE, const widget_type_t type, gse::value::Object* const data ) {
	const auto& data_it = m_widget_configs.find( type );
	if ( data_it != m_widget_configs.end() ) {
		if ( data == nullptr ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Widget data missing" );
		}
		for ( const auto& d : data->value ) {
			const auto& data_it2 = data_it->second.data_config.find( d.first );
			if ( data_it2 == data_it->second.data_config.end() ) {
				GSE_ERROR( gse::EC.UI_ERROR, "Unexpected widget data: " + d.first );
			}
			if ( data_it2->second != d.second->type ) {
				GSE_ERROR( gse::EC.UI_ERROR, "Widget data " + d.first + " is expected to be " + gse::Value::GetTypeStringStatic( data_it2->second ) + ", got " + d.second->GetTypeString() );
			}
		}
		for ( const auto& d : data_it->second.data_config ) {
			if ( data->value.find( d.first ) == data->value.end() ) {
				GSE_ERROR( gse::EC.UI_ERROR, "Widget data missing: " + d.first );
			}
		}
	}
	else {
		if ( data != nullptr && !data->value.empty() ) {
			GSE_ERROR( gse::EC.UI_ERROR, "Unexpected widget data" );
		}
	}
}

void UI::SetWidgetData( dom::Widget* const widget, gse::value::Object* const data ) {
	std::lock_guard guard( m_widgets_mutex );
	const auto& it = m_widget_types.find( widget );
	ASSERT( it != m_widget_types.end(), "widget type not found" );
	const auto& it2 = m_widgets.find( it->second );
	ASSERT( it2 != m_widgets.end(), "widget not found" );
	const auto& it3 = it2->second.find( widget );
	ASSERT( it3 != it2->second.end(), "widget not found" );
	it3->second.data = data;
}

void UI::WithWidget( const widget_type_t type, const f_with_widget_t& f ) {
	std::lock_guard guard( m_widgets_mutex );
	const auto& it = m_widgets.find( type );
	if ( it != m_widgets.end() ) {
		for ( const auto& it2 : it->second ) {
			const auto& widget = it2.first;
			const auto* const g = widget->GetGeometry();
			if ( g->GetWidth() > 0 && g->GetHeight() > 0 ) {
				f( widget );
				widget->Refresh();
			}
		}
	}
}

void UI::Defocus( dom::Focusable* const element ) {
	ASSERT( element, "element is null" );
	if ( element == m_focused_element ) {
		m_focused_element->OnDefocus();
		m_focused_element = nullptr;
	}
}

void UI::SetGlobalSingleton( GSE_CALLABLE, dom::Object* const object, const std::function< void() >& f_on_deglobalize ) {
	ASSERT( object, "object is null" );
	if ( m_global_singleton != object ) {
		if ( m_global_singleton ) {
			RemoveGlobalSingleton( GSE_CALL, m_global_singleton );
		}
		m_global_singleton = object;
		m_f_global_singleton_on_deglobalize = f_on_deglobalize;
		m_global_singleton->Show();
		m_root->AddChild( GSE_CALL, m_global_singleton, false );
	}
}

void UI::RemoveGlobalSingleton( GSE_CALLABLE, dom::Object* const object ) {
	ASSERT( object, "object is null" );
	if ( m_global_singleton == object ) {
		if ( m_f_global_singleton_on_deglobalize ) {
			m_f_global_singleton_on_deglobalize();
		}
		m_root->RemoveChild( GSE_CALL, m_global_singleton, true );
		m_global_singleton->Hide();
		m_global_singleton = nullptr;
		m_f_global_singleton_on_deglobalize = nullptr;
	}
}

}
