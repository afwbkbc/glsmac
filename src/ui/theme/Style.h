#pragma once

#include <vector>
#include <unordered_map>
#include <functional>

#include "base/Base.h"

#include "types/Color.h"
#include "loader/texture/TextureLoader.h"
#include "loader/font/FontLoader.h"

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
	
#define SH(...) [__VA_ARGS__] ( ::ui::theme::Style* s ) -> void
	
CLASS( Style, base::Base )

	typedef std::function<void( Style* s )> style_handler_t;
	
	Style( const std::string& style_name, const StyleSheet* stylesheet );
	
	void Initialize();

	// TODO: attribute grouping/inheritance
	enum attribute_type_t : uint8_t {
		A_NONE,
		
		// positioning
		A_ALIGN,
		A_WIDTH,
		A_HEIGHT,
		A_LEFT,
		A_TOP,
		A_RIGHT,
		A_BOTTOM,
		A_PADDING,
		A_Z_INDEX,
		
		// textures
		A_TEXTURE,
		A_SIZE_FROM_TEXTURE,
		A_STRETCH_TEXTURE,
		A_KEEP_TEXTURE_ASPECT_RATIO,

		// TODO: combine into multitexture?
		A_TEXTURE_BACK, // for things like panel background, scrollbar background
		A_TEXTURE_CENTER, // for things like scrollbar slider
		
		// borders and things like slider up button, slider down button
		A_BORDER_SIZE,
		A_TEXTURE_LEFT,
		A_TEXTURE_TOP,
		A_TEXTURE_RIGHT,
		A_TEXTURE_BOTTOM,
		A_TEXTURE_LEFTTOP,
		A_TEXTURE_RIGHTTOP,
		A_TEXTURE_RIGHTBOTTOM,
		A_TEXTURE_LEFTBOTTOM,
		
		// text
		A_FONT,
		A_TEXT_ALIGN,
		A_TEXT_LEFT,
		A_TEXT_COLOR,
		A_HINT_COLOR,
		
		// sounds
		A_SOUND,
		A_SOUND_VOLUME,
		A_SOUND_AUTOPLAY, // start playing when element is created
		A_SOUND_AUTOSTOP, // stop playing when element is destroyed
		A_SOUND_REPEATABLE,
		A_SOUND_START_DELAY,
		A_BUTTON_CLICK_SOUND,
		A_BUTTON_MOVE_SOUND,
		
		// headers
		A_HEADER_HEIGHT,
		A_HEADER_TEXTALIGN,
		A_HEADER_TEXTCOLOR,
		A_HEADER_FONT,
		// header borders
		A_HEADER_TEXTURE_BACK,
		A_HEADER_TEXTURE_LEFT,
		A_HEADER_TEXTURE_TOP,
		A_HEADER_TEXTURE_RIGHT,
		A_HEADER_TEXTURE_BOTTOM,
		A_HEADER_TEXTURE_LEFTTOP,
		A_HEADER_TEXTURE_RIGHTTOP,
		A_HEADER_TEXTURE_RIGHTBOTTOM,
		A_HEADER_TEXTURE_LEFTBOTTOM,
		
		// lists etc
		A_ITEM_WIDTH,
		A_ITEM_HEIGHT,
		A_ITEM_MARGIN,
		A_ITEM_TEXTURE,
		A_ITEM_ICON_WIDTH,
		A_ITEM_ICON_TEXTURE,
		
		// must be last
		ATTRIBUTE_TYPE_MAX
	};
	
	// bit flags
	typedef uint8_t modifier_t;
	static constexpr modifier_t M_NONE = 0;
	static constexpr modifier_t M_HOVER = 1;
	static constexpr modifier_t M_ACTIVE = 2;
	static constexpr modifier_t M_SELECTED = 4; // also means "focused" (for inputs)
	static constexpr modifier_t MODIFIER_MAX = 8;
	
	void SetStyleHandler( const style_handler_t style_handler );
	
	void Set( const attribute_type_t attribute_type ); // for bool flags without value
	void Set( const attribute_type_t attribute_type, const float value );
	void SetColor( const attribute_type_t attribute_type, const Color& value );
	
	// raw setter
	void SetObject( const attribute_type_t attribute_type, const void* value );
	// convenience setters // TODO: improve SetTexture* API
	void SetTexture( const attribute_type_t attribute_type, const std::string& name );
	void SetTextureTC( const attribute_type_t attribute_type, const std::string& name, const Color::rgba_t transparent_color );
	void SetTexture( const attribute_type_t attribute_type, const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const uint8_t flags = TextureLoader::LT_NONE, const float value = 1.0 );
	void SetTextureTC( const attribute_type_t attribute_type, const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const Color::rgba_t transparent_color, const uint8_t flags = TextureLoader::LT_NONE, const float value = 1.0 );
	void SetTextureTCs( const attribute_type_t attribute_type, const std::string& name, const size_t x1, const size_t y1, const size_t x2, const size_t y2, const TextureLoader::transparent_colors_t& transparent_colors, const uint8_t flags = TextureLoader::LT_NONE, const float value = 1.0 );
	void SetColorTexture( const attribute_type_t attribute_type, const Color& color );
	void SetFont( const attribute_type_t attribute_type, const std::string &name, const unsigned char size );
	void SetSound( const attribute_type_t attribute_type, const std::string& name );

	bool Has( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const float Get( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const Color GetColor( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	const void* GetObject( const attribute_type_t attribute_type, const modifier_t modifiers ) const;
	
	const std::string& GetStyleName() const;
	
	void Include( const std::string& style_name );
	
	bool Is( const modifier_t modifier ) const;
	
	struct attribute_t {
		bool is_set = false;
		union {
			float scalar;
			Color::color_t color;
			const void *ptr;
		} value;
	};
	typedef attribute_t attributes_t[ ATTRIBUTE_TYPE_MAX ];
	
	typedef std::vector< std::string > includes_t;
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
	
	attributes_t m_attributes[ MODIFIER_MAX ];
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

#include "StyleSheet.h"
