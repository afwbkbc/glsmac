#pragma once

namespace types {

class Color {
public:
	float red = 0.0;
	float green = 0.0;
	float blue = 0.0;
	float alpha = 1.0;
	
	typedef uint32_t rgba_t;
	
	Color() {};
	Color(float red,float green,float blue,float alpha) { this->Set(red,green,blue,alpha); };
	Color(float red,float green,float blue) { this->Set(red,green,blue,1.0); };
	void Set(float red,float green,float blue,float alpha) {
		this->red=red;
		this->green=green;
		this->blue=blue;
		this->alpha=alpha;
	};
	bool operator == (const Color operand) { return (this->red==operand.red)&&(this->green==operand.green)&&(this->blue==operand.blue)&&(this->alpha==operand.alpha); };
	bool operator != (const Color operand) { return (this->red!=operand.red)||(this->green!=operand.green)||(this->blue!=operand.blue)||(this->alpha!=operand.alpha); };

	const rgba_t GetRGBA() const {
		return ( (uint8_t)(red * 255) ) | ( (uint8_t)(green * 255) << 8 ) | ( (uint8_t)(blue * 255) << 16) | ( (uint8_t)(alpha * 255) << 24 );
	};
	
	static Color TRANSPARENT() { return Color( 0.0, 0.0, 0.0, 0.0 ); }
	static Color WHITE() { return Color( 1.0, 1.0, 1.0, 1.0 ); }
	static Color RED() { return Color( 1.0, 0.0, 0.0, 1.0 ); }
	static Color GREEN() { return Color( 0.0, 1.0, 0.0, 1.0 ); }
	static Color BLUE() { return Color( 0.0, 0.0, 1.0, 1.0 ); }
};

} /* namespace types */
