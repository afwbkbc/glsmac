#pragma once

#include "base/Base.h"

#include <vector>
#include <unordered_map>

#include "types/Color.h"

using namespace std;
using namespace types;

namespace ui {
namespace theme {

MAJOR_CLASS( Style, base::Base )

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
		A_COLOR,
		A_TEXTURE,
		A_FONT,
		A_SIZE_FROM_TEXTURE,
			
		ATTRIBUTE_TYPE_MAX
	};

	typedef uint8_t modifier_t;
	static const modifier_t M_NONE = 0;
	static const modifier_t M_HOVER = 1;
	static const modifier_t MODIFIER_MAX = 2;
	
	void Set( const attribute_type_t attribute_type ); // for bool flags without value
	void Set( const attribute_type_t attribute_type, const ssize_t value );
	void SetColor( const attribute_type_t attribute_type, const Color& value );
	void SetObject( const attribute_type_t attribute_type, const void* value );

	bool Has( const attribute_type_t attribute_type, const modifier_t modifier ) const;
	const ssize_t Get( const attribute_type_t attribute_type, const modifier_t modifier ) const;
	const Color GetColor( const attribute_type_t attribute_type, const modifier_t modifier ) const;
	const void* GetObject( const attribute_type_t attribute_type, const modifier_t modifier ) const;
	
protected:
	
	virtual void SetStyle() = 0;
	
	typedef struct {
		bool is_set = false;
		union {
			ssize_t scalar;
			Color::color_t color;
			const void *ptr;
		} value;
	} attribute_t;
	typedef attribute_t attributes_t[ ATTRIBUTE_TYPE_MAX ];
	
	attributes_t m_attributes[ MODIFIER_MAX ];
	attributes_t* m_attributes_ptr = nullptr;
	modifier_t m_modifier = M_NONE;
	void SetAttributesPtr( attributes_t* attributes );
	void UnsetAttributesPtr();
	
private:
	bool m_is_initialized = false;
	
	void PrepareAttributes( const modifier_t modifier );
	
#if DEBUG
	void CheckSet( const attribute_type_t attribute_type ) const;
	void CheckGet( const attribute_type_t attribute_type, const modifier_t modifier ) const;
#endif
};

}
}
