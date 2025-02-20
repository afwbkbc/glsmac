#include "Color.h"

#include "gse/value/Float.h"

namespace types {

Color::color_t::color_t( const color_t& color ) {
	*this = color;
}
Color::color_t::color_t( const Color& color ) {
	*this = color.value;
}
void Color::color_t::operator=( const Color& color ) {
	*this = color.value;
}
Color::color_t::color_t( const channel_t red, const channel_t green, const channel_t blue, const channel_t alpha ) {
	this->red = red;
	this->green = green;
	this->blue = blue;
	this->alpha = alpha;
}

Color::color_t::color_t( const channel_t red, const channel_t green, const channel_t blue ) {
	this->red = red;
	this->green = green;
	this->blue = blue;
}

Color::Color()
	: gse::Wrappable() {
	//
};

Color::Color( const channel_t red, const channel_t green, const channel_t blue, const channel_t alpha )
	: gse::Wrappable() {
	Set( red, green, blue, alpha );
};

Color::Color( const channel_t red, const channel_t green, const channel_t blue )
	: gse::Wrappable() {
	Set( red, green, blue, 1.0 );
};

Color::Color( const color_t& color )
	: gse::Wrappable() {
	value = color;
}

void Color::Set( channel_t red, channel_t green, channel_t blue, channel_t alpha ) {
	value.red = red;
	value.green = green;
	value.blue = blue;
	value.alpha = alpha;
};

void Color::operator=( const color_t& color ) {
	value = color;
}

bool Color::operator==( Color& other ) const {
	return
		( value.red == other.value.red ) &&
			( value.green == other.value.green ) &&
			( value.blue == other.value.blue ) &&
			( value.alpha == other.value.alpha );
};

bool Color::operator!=( const Color& other ) const {
	return
		( value.red != other.value.red ) ||
			( value.green != other.value.green ) ||
			( value.blue != other.value.blue ) ||
			( value.alpha != other.value.alpha );
};

const Color Color::operator*( float operand ) const {
	return {
		value.red * operand,
		value.green * operand,
		value.blue * operand,
		value.alpha // alpha shouldn't change from multiplication?
	};
}

const Color Color::operator/( float operand ) const {
	return {
		value.red / operand,
		value.green / operand,
		value.blue / operand,
		value.alpha // alpha shouldn't change from division?
	};
}

const Color Color::operator*( const Color& other ) const {
	return {
		value.red * other.value.red,
		value.green * other.value.green,
		value.blue * other.value.blue,
		value.alpha * other.value.alpha,
	};
}

const Color::rgba_t Color::GetRGBA() const {
	return RGBA( value.red * 255, value.green * 255, value.blue * 255, value.alpha * 255 );
};

Color Color::FromRGBA( const rgba_t rgba ) {
	return {
		(channel_t)( ( rgba ) & 0xff ) / 256,
		(channel_t)( ( rgba >> 8 ) & 0xff ) / 256,
		(channel_t)( ( rgba >> 16 ) & 0xff ) / 256,
		(channel_t)( ( rgba >> 24 ) & 0xff ) / 256
	};
}

Color Color::FromRGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha ) {
	return {
		(channel_t)red / 255,
		(channel_t)green / 255,
		(channel_t)blue / 255,
		(channel_t)alpha / 255
	};
}

Color Color::FromRGB( const uint8_t red, const uint8_t green, const uint8_t blue ) {
	return FromRGBA( red, green, blue );
}

Color::rgba_t Color::RGBA( const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha ) {
	return red | ( green << 8 ) | ( blue << 16 ) | ( alpha << 24 );
}

Color::rgba_t Color::RGB( const uint8_t red, const uint8_t green, const uint8_t blue ) {
	return RGBA( red, green, blue, 255 );
}

void Color::Convert( const color_t& in, rgba_t& out ) {
	out =
		( (uint8_t)( std::min( in.red, 1.0f ) * 255) ) |
		( (uint8_t)( std::min( in.green, 1.0f ) * 255 ) << 8 ) |
		( (uint8_t)( std::min( in.blue, 1.0f ) * 255 ) << 16 ) |
		( (uint8_t)( std::min( in.alpha, 1.0f ) * 255 ) << 24 )
	;
}

void Color::Convert( const rgba_t& in, color_t& out ) {
	out = color_t{
		(channel_t)( ( in ) & 0xff ) / 255,
		(channel_t)( ( in >> 8 ) & 0xff ) / 255,
		(channel_t)( ( in >> 16 ) & 0xff ) / 255,
		(channel_t)( ( in >> 24 ) & 0xff ) / 255
	};
}

const std::string Color::ToString() const {
	return "{" + std::to_string( value.red ) + ":" + std::to_string( value.green ) + ":" + std::to_string( value.blue ) + ":" + std::to_string( value.alpha ) + "}";
}

WRAPIMPL_BEGIN( Color )
	WRAPIMPL_PROPS
		{
			"r",
			VALUE( gse::value::Float,, value.red )
		},
		{
			"g",
			VALUE( gse::value::Float,, value.green )
		},
		{
			"b",
			VALUE( gse::value::Float,, value.blue )
		},
		{
			"a",
			VALUE( gse::value::Float,, value.alpha )
		},
	};
WRAPIMPL_END_NOPTR( Color )

UNWRAPIMPL_NOPTR_BEGIN( Color )
	return {
		( (gse::value::Float*)properties.at( "r" ) )->value,
		( (gse::value::Float*)properties.at( "g" ) )->value,
		( (gse::value::Float*)properties.at( "b" ) )->value,
		( (gse::value::Float*)properties.at( "a" ) )->value
	};
UNWRAPIMPL_NOPTR_END()

}
