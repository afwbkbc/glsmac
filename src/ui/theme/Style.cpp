#include "Style.h"

#include "engine/Engine.h"
#include "loader/texture/TextureLoader.h"
#include "loader/sound/SoundLoader.h"
#include "loader/font/FontLoader.h"
#include "StyleSheet.h"

namespace ui {
namespace theme {

Style::Style( const std::string& style_name, const StyleSheet* stylesheet )
	: m_style_name( style_name )
	, m_stylesheet( stylesheet ) {
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
#ifdef DEBUG
	CheckSet( attribute_type );
#endif
	( *m_attributes_ptr )[ attribute_type ].is_set = true;
}

void Style::Set( const attribute_type_t attribute_type, const float value ) {
#ifdef DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	( *m_attributes_ptr )[ attribute_type ].value.scalar = value;
}

void Style::SetColor( const attribute_type_t attribute_type, const types::Color& value ) {
#ifdef DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	( *m_attributes_ptr )[ attribute_type ].value.color = value;
}

void Style::SetObject( const attribute_type_t attribute_type, const void* ptr ) {
#ifdef DEBUG
	CheckSet( attribute_type );
#endif
	Set( attribute_type );
	( *m_attributes_ptr )[ attribute_type ].value.ptr = ptr;
}

void Style::SetTexture( const attribute_type_t attribute_type, const resource::resource_t res ) {
	SetObject( attribute_type, g_engine->GetTextureLoader()->LoadTexture( res ) );
}

void Style::SetTexture( const attribute_type_t attribute_type, const resource::resource_t res, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags, const float value ) {
	SetObject( attribute_type, g_engine->GetTextureLoader()->LoadTexture( res, x1, y1, x2, y2, flags, value ) );
}

void Style::SetColorTexture( const attribute_type_t attribute_type, const types::Color& color ) {
	SetObject( attribute_type, g_engine->GetTextureLoader()->GetColorTexture( color ) );
}

void Style::SetFont( const attribute_type_t attribute_type, const resource::resource_t res, const unsigned char size ) {
	SetObject( attribute_type, g_engine->GetFontLoader()->LoadFont( res, size ) );
}

void Style::SetSound( const attribute_type_t attribute_type, const resource::resource_t res ) {
	SetObject( attribute_type, g_engine->GetSoundLoader()->LoadSound( res ) );
}

bool Style::Has( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	return m_attributes[ modifiers ][ attribute_type ].is_set;
}

const float Style::Get( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#ifdef DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.scalar;
}

const types::Color Style::GetColor( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#ifdef DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.color;
}

const void* Style::GetObject( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
#ifdef DEBUG
	CheckGet( attribute_type, modifiers );
#endif
	return m_attributes[ modifiers ][ attribute_type ].value.ptr;
}

const std::string& Style::GetStyleName() const {
	return m_style_name;
}

void Style::SetStyle() {
	ASSERT( m_style_handler, "style handler not set and SetStyle not overridden" );
	m_style_handler( this );
}

void Style::Include( const std::string& style_name ) {
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

const modifier_t Style::GetModifiers() const {
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
		for ( auto& include : m_includes ) {
			auto* style = m_stylesheet->GetStylePtr( include );
			style->SetStyleForInclude( m_attributes_ptr, m_modifiers );
		}
	}
	SetStyle();
	UnsetAttributesPtr();
}

#ifdef DEBUG

void Style::CheckSet( const attribute_type_t attribute_type ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	ASSERT( m_attributes_ptr, "attributes ptr not set" );
	ASSERT( !( *m_attributes_ptr )[ attribute_type ].is_set, "style attribute '" + std::to_string( attribute_type ) + "' in style '" + GetStyleName() + "' already set" );
}

void Style::CheckGet( const attribute_type_t attribute_type, const modifier_t modifiers ) const {
	ASSERT( m_is_initialized, "style '" + GetStyleName() + "' not initialized" );
	ASSERT( m_attributes[ modifiers ][ attribute_type ].is_set, "style attribute not set" );
}

#endif

}
}