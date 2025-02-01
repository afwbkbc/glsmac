#pragma once

#include <cstdint>
#include <cstring>
#include <unordered_set>
#include <functional>

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
		ALIGN_LEFT_TOP = ALIGN_LEFT | ALIGN_TOP,
		ALIGN_LEFT_BOTTOM = ALIGN_LEFT | ALIGN_BOTTOM,
		ALIGN_LEFT_CENTER = ALIGN_LEFT | ALIGN_VCENTER,
		ALIGN_RIGHT_TOP = ALIGN_RIGHT | ALIGN_TOP,
		ALIGN_RIGHT_BOTTOM = ALIGN_RIGHT | ALIGN_BOTTOM,
		ALIGN_RIGHT_CENTER = ALIGN_RIGHT | ALIGN_VCENTER,
		ALIGN_TOP_CENTER = ALIGN_TOP | ALIGN_HCENTER,
		ALIGN_BOTTOM_CENTER = ALIGN_BOTTOM | ALIGN_HCENTER,
	};

	Rectangle* AsRectangle() const;
	Text* AsText() const;

	void SetLeft( const coord_t px );
	void SetTop( const coord_t px );
	void SetRight( const coord_t px );
	void SetBottom( const coord_t px );
	void SetWidth( const coord_t px );
	void SetHeight( const coord_t px );
	void SetPadding( const coord_t px );
	void SetAlign( const align_t align );
	void SetZIndex( const coord_t zindex );
	void SetOverflowAllowed( const bool is_overflow_allowed );

	void NeedUpdate();

	struct area_t {
		coord_t left;
		coord_t right;
		coord_t top;
		coord_t bottom;
		coord_t width;
		coord_t height;
		coord_t zindex;

		bool operator!=( const area_t& other ) const {
			return memcmp( this, &other, sizeof( other ) ) != 0;
		}

		const bool EnlargeTo( const area_t& other ) {
			bool changed = false;
			if ( top > other.top ) {
				top = other.top;
				changed = true;
			}
			if ( left > other.left ) {
				left = other.left;
				changed = true;
			}
			if ( bottom < other.bottom ) {
				bottom = other.bottom;
				changed = true;
			}
			if ( right < other.right ) {
				right = other.right;
				changed = true;
			}
			if ( changed ) {
				width = right - left;
				height = bottom - top;
			}
			return changed;
		}
	};

	area_t m_area = {};

	// this includes children than may get outside of geometry's area (i.e. with negative coordinates)
	const area_t& GetEffectiveArea() const;

	void OnChildUpdate();

	const geometry_handler_id_t AddHandler( const geometry_handler_type_t type, const std::function< void() >& f );
	void RemoveHandler( const geometry_handler_id_t id );

	const bool Contains( const types::Vec2< ptrdiff_t >& position ) const;

protected:

	virtual void UpdateImpl() = 0;

	const UI* const m_ui;

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
	coord_t m_padding = 0;
	uint8_t m_align = ALIGN_LEFT | ALIGN_TOP;
	coord_t m_zindex = 0.5f;
	bool m_is_overflow_allowed = true;

	void UpdateArea();
	void UpdateEffectiveArea();

	area_t m_effective_area = {};

	geometry_handler_id_t m_next_handler_id = 0;
	std::unordered_map< geometry_handler_type_t, std::unordered_map< geometry_handler_id_t, std::function< void() > > > m_handlers = {};
	std::unordered_map< geometry_handler_id_t, geometry_handler_type_t > m_handler_types = {};
	void RunHandlers( const geometry_handler_type_t type ) const;
};

}
}
