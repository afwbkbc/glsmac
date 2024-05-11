#pragma once

#include "base/Base.h"

#include "Types.h"

#include "types/Color.h"

namespace ui {
namespace object {
class UIObject;
}
namespace theme {
class Theme;
}
}

namespace ui {
namespace theme {

class StyleSheet;

#define SH( ... ) [__VA_ARGS__] ( ::ui::theme::Style* s ) -> void

CLASS( Style, base::Base )

	Style( const std::string& style_name, const StyleSheet* stylesheet );

	void Initialize();

	void SetStyleHandler( const style_handler_t style_handler );

	void Set( const attribute_type_t attribute_type ); // for bool flags without value
	void Set( const attribute_type_t attribute_type, const float value );
	void SetColor( const attribute_type_t attribute_type, const types::Color& value );

	// raw setter
	void SetObject( const attribute_type_t attribute_type, const void* value );
	// convenience setters // TODO: improve SetTexture* API
	void SetTexture( const attribute_type_t attribute_type, const std::string& name );
	void SetTexture( const attribute_type_t attribute_type, const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = 0, const float value = 1.0 );
	void SetColorTexture( const attribute_type_t attribute_type, const types::Color& color );
	void SetFont( const attribute_type_t attribute_type, const std::string& name, const unsigned char size );
	void SetSound( const attribute_type_t attribute_type, const std::string& name );

	bool Has( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const float Get( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const types::Color GetColor( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const void* GetObject( const attribute_type_t attribute_type, const modifier_t modifiers ) const;

	const std::string& GetStyleName() const;

	void Include( const std::string& style_name );

	bool Is( const modifier_t modifier ) const;

	void SetIncludes( const includes_t& includes );

	void SetStyleForInclude( attributes_t* attributes_ptr, const modifier_t modifiers );
	const modifier_t GetModifiers() const;

protected:

	virtual void SetStyle();
	style_handler_t m_style_handler = nullptr;

private:

	const std::string m_style_name = "";

	includes_t m_includes = {};

	const StyleSheet* m_stylesheet = nullptr;

	attributes_t m_attributes[MODIFIER_MAX];
	attributes_t* m_attributes_ptr = nullptr;
	modifier_t m_modifiers = M_NONE;
	void SetAttributesPtr( attributes_t* attributes );
	void UnsetAttributesPtr();

	bool m_is_initialized = false;

	void PrepareAttributes( const modifier_t modifiers );

#ifdef DEBUG

	void CheckSet( const attribute_type_t attribute_type ) const;
	void CheckGet( const attribute_type_t attribute_type, const modifier_t modifiers ) const;

#endif
};

}
}
