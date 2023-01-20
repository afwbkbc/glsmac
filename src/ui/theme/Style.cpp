#include "Style.h"

#include "engine/Engine.h"

using namespace types;

namespace ui {
namespace theme {

Style::Style( const string& style_name, const StyleSheet* stylesheet )
	: m_style_name( style_name )
	, m_stylesheet( stylesheet )
{
	//
}
	
void Style::Initialize() {
	ASSERT( !m_is_initialized, "style already initialized" );
	m_is_initialized = true;
	for ( modifier_t modifiers = M_NONE ; modifiers < MODIFIER_MAX ; modifiers++ ) {
		PrepareAttributes( modifiers );
	}
}

void Style::SetStyleHandler( const style_handler_t style_handler ) {
	ASSERT( !m_style_handler, "style handler already set" );
	m_style_handler = style_handler;
}

void Style::Set( const attribute_type_t attribute_type ) {
#if DEBUG
	CheckSet( attribute_type );
#endif
	(*m_attributes_ptr)[ attribute_type ].is_set = true;
}

void Style::Set( const attribute_type_t attribute_type, const float value ) {
#if DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	(*m_attributes_ptr)[ attribute_type ].value.scalar = value;
}

void Style::SetColor( const attribute_type_t attribute_type, const Color& value ) {
#if DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	(*m_attributes_ptr)[ attribute_type ].value.color = value;
}

void Style::SetObject( const attribute_type_t attribute_type, const void* ptr ) {
#if DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	(*m_attributes_ptr)[ attribute_type ].value.ptr = ptr;
}

void Style::SetTexture( const attribute_type_t attribute_type, const std::string& name ) {
	SetObject( attribute_type, g_engine->GetTextureLoader()->LoadTexture( name ) );
}

void Style::SetTexture( const attribute_type_t attribute_type, const string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	SetObject( attribute_type, g_engine->GetTextureLoader()->LoadTexture( name, x1, y1, x2, y2, flags, value ) );
}

void Style::SetFont( const attribute_type_t attribute_type, const std::string &name, const unsigned char size ) {
	SetObject( attribute_type, g_engine->GetFontLoader()->LoadFont( name, size ) );
}

void Style::SetSound( const attribute_type_t attribute_type, const std::string& name ) {
	SetObject( attribute_type, g_engine->GetSoundLoader()->LoadSound( name ) );
}

bool Style::Has( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	return m_attributes[ modifiers ][ attribute_type ].is_set;
}

const float Style::Get( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#if DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.scalar;
}

const Color Style::GetColor( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#if DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.color;
}

const void* Style::GetObject( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#if DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.ptr;
}

const string& Style::GetStyleName() const {
	return m_style_name;
}

void Style::SetStyle() {
	ASSERT( m_style_handler, "style handler not set and SetStyle not overridden" );
	m_style_handler( this );
}

void Style::Include( const string& style_name ) {
	m_includes.push_back( style_name );
}

bool Style::Is( const modifier_t modifier ) const {
	return ( ( m_modifiers & modifier ) == modifier );
}

void Style::SetIncludes( const includes_t& includes ) {
	m_includes = includes;
}

void Style::SetStyleForInclude( attributes_t* attributes_ptr, const modifier_t modifiers ) {
	attributes_t* attributes_ptr_old = m_attributes_ptr;
	const modifier_t modifiers_old = m_modifiers;
	
	m_attributes_ptr = attributes_ptr;
	m_modifiers = modifiers;
	SetStyle();
	
	m_modifiers = modifiers_old;
	m_attributes_ptr = attributes_ptr_old;
}

const Style::modifier_t Style::GetModifiers() const {
	return m_modifiers;
}

void Style::SetAttributesPtr( attributes_t* attributes ) {
	ASSERT( !m_attributes_ptr, "attributes ptr already set" );
	m_attributes_ptr = attributes;
}

void Style::UnsetAttributesPtr() {
	ASSERT( m_attributes_ptr, "attributes ptr already unset" );
	m_attributes_ptr = nullptr;
}

void Style::PrepareAttributes( const modifier_t modifiers ) {
	m_modifiers = modifiers;
	SetAttributesPtr( &m_attributes[ modifiers ] );
	if ( !m_includes.empty() ) {
		ASSERT( m_stylesheet, "include needed but stylesheet not set" );
		for (auto& include : m_includes) {
			auto* style = m_stylesheet->GetStylePtr( include );
			style->SetStyleForInclude( m_attributes_ptr, m_modifiers );
		}
	}
	SetStyle();
	UnsetAttributesPtr();
}

#if DEBUG
void Style::CheckSet( const attribute_type_t attribute_type ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	ASSERT( m_attributes_ptr, "attributes ptr not set" );
	ASSERT( !(*m_attributes_ptr)[ attribute_type ].is_set, "style attribute '" + to_string( attribute_type ) + "' in style '" + GetStyleName() + "' already set" );
}

void Style::CheckGet( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	ASSERT( m_attributes[ modifiers ][ attribute_type ].is_set, "style attribute not set" );
}
#endif

}
}