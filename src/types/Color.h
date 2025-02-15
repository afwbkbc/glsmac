#pragma once

#include <string>
#include <cstdint>
#include <cstring>

#include "gse/Value.h"
#include "gse/type/Object.h"
#include "gse/Wrappable.h"

// TODO: refactor

namespace types {

class Color : public gse::Wrappable {
public:

	typedef float channel_t;
	typedef uint32_t rgba_t;

	struct color_t {
		channel_t red;
		channel_t green;
		channel_t blue;
		channel_t alpha;
		color_t() = default;
		~color_t() = default;
		color_t( const color_t& color );
		color_t( const Color& color );
		void operator=( const Color& color );
		const color_t operator*( float other ) const {
			return {
				red * other,
				green * other,
				blue * other,
				alpha * other,
			};
		}
		const color_t operator+( const color_t& other ) const {
			return {
				red + other.red,
				green + other.green,
				blue + other.blue,
				alpha + other.alpha,
			};
		}
		color_t( const channel_t red, const channel_t green, const channel_t blue, const channel_t alpha );
		color_t( const channel_t red, const channel_t green, const channel_t blue );
	};

	color_t value = {};

	Color();
	Color( const channel_t red, const channel_t green, const channel_t blue, const channel_t alpha );
	Color( const channel_t red, const channel_t green, const channel_t blue );
	Color( const color_t& color );
	virtual ~Color() = default;

	void Set( channel_t red, channel_t green, channel_t blue, channel_t alpha );

	void operator=( const color_t& color );
	bool operator==( Color& other ) const;
	bool operator!=( const Color& other ) const;
	const Color operator*( const float operand ) const;
	const Color operator/( const float operand ) const;

	const Color operator*( const Color& other ) const;

	const rgba_t GetRGBA() const;
	static Color FromRGBA( const rgba_t rgba );
	static Color FromRGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha = 255 );
	static Color FromRGB( const uint8_t red, const uint8_t green, const uint8_t blue );
	static rgba_t RGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha );
	static rgba_t RGB( const uint8_t red, const uint8_t green, const uint8_t blue );

	static void Convert( const color_t& in, rgba_t& out );
	static void Convert( const rgba_t& in, color_t& out );

	const std::string ToString() const;

	WRAPDEFS_NOPTR( Color );
};

}
