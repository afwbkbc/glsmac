#pragma once

#include <cstdint>
#include <cstring>
#include <unordered_set>

#include "ui/Types.h"
#include "types/mesh/Types.h"

#include "types/Vec2.h"

namespace ui {

class UI;

namespace geometry {

class Rectangle;
class Text;

class Geometry {
public:

	enum geometry_type_t {
		GT_RECTANGLE,
		GT_TEXT,
	};

	Geometry( const UI* const ui, Geometry* const parent, const geometry_type_t type );
	virtual ~Geometry();

	enum align_t : uint8_t {
		ALIGN_NONE = 0,
		ALIGN_LEFT = 1 << 0,
		ALIGN_RIGHT = 1 << 1,
		ALIGN_HCENTER = ALIGN_LEFT | ALIGN_RIGHT,
		ALIGN_TOP = 1 << 2,
		ALIGN_BOTTOM = 1 << 3,
		ALIGN_VCENTER = ALIGN_TOP | ALIGN_BOTTOM,
		ALIGN_CENTER = ALIGN_HCENTER | ALIGN_VCENTER,
	};

	Rectangle* AsRectangle() const;
	Text* AsText() const;

	void SetLeft( const coord_t px );
	void SetTop( const coord_t px );
	void SetRight( const coord_t px );
	void SetBottom( const coord_t px );
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	void SetAlign( const align_t align );

	void NeedUpdate();

protected:

	virtual void UpdateImpl() = 0;

	const UI* const m_ui;

	struct area_t {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;
		bool operator!=( const area_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}
	};
	area_t m_area = {};

private:
	const geometry_type_t m_type;

	void Update();

	Geometry* const m_parent;
	std::unordered_set< Geometry* > m_children = {};

	enum stick_bits_t : uint8_t {
		STICK_NONE = 0,
		STICK_LEFT = 1 << 0,
		STICK_RIGHT = 1 << 1,
		STICK_WIDTH = 1 << 2,
		STICK_TOP = 1 << 3,
		STICK_BOTTOM = 1 << 4,
		STICK_HEIGHT = 1 << 5,
	};
	uint8_t m_stick_bits = STICK_NONE;

	coord_t m_left = 0;
	coord_t m_top = 0;
	coord_t m_right = 0;
	coord_t m_bottom = 0;
	coord_t m_width = 0;
	coord_t m_height = 0;
	uint8_t m_align = ALIGN_LEFT | ALIGN_TOP;

	void UpdateArea();

};

}
}
