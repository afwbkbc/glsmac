#pragma once

#include "base/Base.h"

#include <vector>
#include <unordered_map>
#include <functional>

#include "types/Color.h"
#include "loader/texture/TextureLoader.h"
#include "loader/font/FontLoader.h"

using namespace std;
using namespace types;

using namespace loader;
namespace loader {
using namespace texture;
using namespace font;
}

namespace ui {
namespace object {
	class UIObject;
}
using namespace object;
namespace theme {
	class Theme;
}
using namespace theme;
}
using namespace ui;

namespace ui {
namespace theme {

class StyleSheet;
	
#define SH(...) [__VA_ARGS__] ( Style* s ) -> void
	
MAJOR_CLASS( Style, base::Base )

	typedef function<void( Style* s )> style_handler_t;
	
	Style( const string& style_name, const StyleSheet* stylesheet );
	
	void Initialize();

	enum attribute_type_t : uint8_t {
		A_NONE,
		A_WIDTH,
		A_HEIGHT,
		A_LEFT,
		A_TOP,
		A_RIGHT,
		A_BOTTOM,
		A_ALIGN,
		A_TEXTALIGN,
		A_TEXTCOLOR,
		A_TEXTURE,
		A_TEXTURE_BACK,
		A_TEXTURE_BORDER_LEFT,
		A_TEXTURE_BORDER_TOP,
		A_TEXTURE_BORDER_RIGHT,
		A_TEXTURE_BORDER_BOTTOM,
		A_TEXTURE_BORDER_CORNER_LT,
		A_TEXTURE_BORDER_CORNER_RT,
		A_TEXTURE_BORDER_CORNER_RB,
		A_TEXTURE_BORDER_CORNER_LB,
		A_FONT,
		A_SIZE_FROM_TEXTURE,
		A_STRETCH_TEXTURE, // will also force aspect ratio of texture
			
		ATTRIBUTE_TYPE_MAX
	};

	// bit flags
	typedef uint8_t modifier_t;
	static const modifier_t M_NONE = 0;
	static const modifier_t M_HOVER = 1;
	static const modifier_t M_ACTIVE = 2;
	static const modifier_t M_SELECTED = 4;
	static const modifier_t MODIFIER_MAX = 8;
	
	void SetStyleHandler( const style_handler_t style_handler );
	
	void Set( const attribute_type_t attribute_type ); // for bool flags without value
	void Set( const attribute_type_t attribute_type, const ssize_t value );
	void SetColor( const attribute_type_t attribute_type, const Color& value );
	
	// raw setter
	void SetObject( const attribute_type_t attribute_type, const void* value );
	// convenience setters
	void SetTexture( const attribute_type_t attribute_type, const std::string& name );
	void SetTexture( const attribute_type_t attribute_type, const string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = TextureLoader::LT_NONE, const float value = 1.0 );
	void SetFont( const attribute_type_t attribute_type, const std::string &name, const unsigned char size );

	bool Has( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const ssize_t Get( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const Color GetColor( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const void* GetObject( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	
	const string& GetStyleName() const;
	
	void Include( const string& style_name );
	
	bool Is( const modifier_t modifier ) const;
	
	typedef struct {
		bool is_set = false;
		union {
			ssize_t scalar;
			Color::color_t color;
			const void *ptr;
		} value;
	} attribute_t;
	typedef attribute_t attributes_t[ ATTRIBUTE_TYPE_MAX ];
	
	typedef vector< string > includes_t;
	void SetIncludes( const includes_t& includes );
	
	void SetStyleForInclude( attributes_t* attributes_ptr, const modifier_t modifiers );
	const modifier_t GetModifiers() const;
	
protected:
	
	virtual void SetStyle();
	style_handler_t m_style_handler = nullptr;
	
private:
	
	const string m_style_name = "";
	
	includes_t m_includes = {};
	
	const StyleSheet* m_stylesheet = nullptr;
	
	attributes_t m_attributes[ MODIFIER_MAX ];
	attributes_t* m_attributes_ptr = nullptr;
	modifier_t m_modifiers = M_NONE;
	void SetAttributesPtr( attributes_t* attributes );
	void UnsetAttributesPtr();
	
	bool m_is_initialized = false;
	
	void PrepareAttributes( const modifier_t modifiers );
	
#if DEBUG
	void CheckSet( const attribute_type_t attribute_type ) const;
	void CheckGet( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
#endif
};

}
}

#include "StyleSheet.h"
