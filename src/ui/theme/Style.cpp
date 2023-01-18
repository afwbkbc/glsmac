#include "Style.h"

using namespace types;

namespace ui {
namespace theme {

void Style::Initialize() {
	ASSERT( !m_is_initialized, "style already initialized" );
	m_is_initialized = true;
	for ( modifier_t modifier = M_NONE ; modifier < MODIFIER_MAX ; modifier++ ) {
		PrepareAttributes( modifier );
	}
}

void Style::Set( const attribute_type_t attribute_type ) {
#if DEBUG
	CheckSet( attribute_type );
#endif
	(*m_attributes_ptr)[ attribute_type ].is_set = true;
}

void Style::Set( const attribute_type_t attribute_type, const ssize_t value ) {
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

bool Style::Has( const attribute_type_t attribute_type, const modifier_t modifier ) const {
	ASSERT( m_is_initialized, "style not initialized" );
	return m_attributes[ modifier ][ attribute_type ].is_set;
}

const ssize_t Style::Get( const attribute_type_t attribute_type, const modifier_t modifier ) const {
#if DEBUG
	CheckGet( attribute_type, modifier );
#endif
	return m_attributes[ modifier ][ attribute_type ].value.scalar;
}

const Color Style::GetColor( const attribute_type_t attribute_type, const modifier_t modifier ) const {
#if DEBUG
	CheckGet( attribute_type, modifier );
#endif
	return m_attributes[ modifier ][ attribute_type ].value.color;
}

const void* Style::GetObject( const attribute_type_t attribute_type, const modifier_t modifier ) const {
#if DEBUG
	CheckGet( attribute_type, modifier );
#endif
	return m_attributes[ modifier ][ attribute_type ].value.ptr;
}

void Style::SetAttributesPtr( attributes_t* attributes ) {
	ASSERT( !m_attributes_ptr, "attributes ptr already set" );
	m_attributes_ptr = attributes;
}

void Style::UnsetAttributesPtr() {
	ASSERT( m_attributes_ptr, "attributes ptr already unset" );
	m_attributes_ptr = nullptr;
}

void Style::PrepareAttributes( const modifier_t modifier ) {
	m_modifier = modifier;
	SetAttributesPtr( &m_attributes[ modifier ] );
	SetStyle();
	UnsetAttributesPtr();
}

#if DEBUG
void Style::CheckSet( const attribute_type_t attribute_type ) const {
	ASSERT( m_is_initialized, "style not initialized" );
	ASSERT( m_attributes_ptr, "attributes ptr not set" );
	ASSERT( !(*m_attributes_ptr)[ attribute_type ].is_set, "style attribute already set" );
}

void Style::CheckGet( const attribute_type_t attribute_type, const modifier_t modifier ) const {
	ASSERT( m_is_initialized, "style not initialized" );
	ASSERT( m_attributes[ modifier ][ attribute_type ].is_set, "style attribute not set" );
}
#endif


}
}