#include "Color.h"

namespace types {

Color::Color() {
	//
};

Color::Color( const channel_t red, const channel_t green, const channel_t blue, const channel_t alpha ) {
	Set( red, green, blue, alpha );
};

Color::Color( const channel_t red, const channel_t green, const channel_t blue ) {
	Set( red, green, blue, 1.0 );
};

Color::Color( const color_t& color ) {
	memcpy( &value, &color, sizeof(color) );
}

void Color::Set( channel_t red, channel_t green, channel_t blue, channel_t alpha ) {
	value.red = red;
	value.green = green;
	value.blue = blue;
	value.alpha =alpha;
};

void Color::operator= ( const color_t& color ) {
	memcpy( &value, &color, sizeof(color) );
}

bool Color::operator== ( Color& other ) const {
	return
		( value.red == other.value.red ) &&
		( value.green == other.value.green ) &&
		( value.blue == other.value.blue ) &&
		( value.alpha == other.value.alpha )
	;
};

bool Color::operator!= ( Color& other) const {
	return
		( value.red != other.value.red ) ||
		( value.green != other.value.green) ||
		( value.blue != other.value.blue) ||
		( value.alpha != other.value.alpha)
	;
};

bool Color::operator!= ( const Color& other) {
	return
		( value.red != other.value.red ) ||
		( value.green != other.value.green) ||
		( value.blue != other.value.blue) ||
		( value.alpha != other.value.alpha)
	;
};

const Color::rgba_t Color::GetRGBA() const {
	return RGBA( value.red * 255, value.green * 255, value.blue * 255, value.alpha * 255 );
};

Color Color::FromRGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha ) {
	return {
		(channel_t) red / 255,
		(channel_t) green / 255,
		(channel_t) blue / 255,
		(channel_t) alpha / 255
	};
}

Color::rgba_t Color::RGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha ) {
	return red | ( green << 8 ) | ( blue << 16) | ( alpha << 24 );
}



}