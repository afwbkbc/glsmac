#pragma once

namespace types {

class Color {
public:
	float red = 0.0;
	float green = 0.0;
	float blue = 0.0;
	float alpha = 1.0;
	Color() {};
	Color(float red,float green,float blue) { this->Set(red,green,blue,1.0); };
	Color(float red,float green,float blue,float alpha) { this->Set(red,green,blue,alpha); };
	void Set(float red,float green,float blue,float alpha) {
		this->red=red;
		this->green=green;
		this->blue=blue;
		this->alpha=alpha;
	};
	bool operator == (const Color operand) { return (this->red==operand.red)&&(this->green==operand.green)&&(this->blue==operand.blue)&&(this->alpha==operand.alpha); };
	bool operator != (const Color operand) { return (this->red!=operand.red)||(this->green!=operand.green)||(this->blue!=operand.blue)||(this->alpha!=operand.alpha); };

	static Color WHITE() { return Color( 1.0, 1.0, 1.0 ); }
	static Color RED() { return Color( 1.0, 0.0, 0.0 ); }
	static Color GREEN() { return Color( 0.0, 1.0, 0.0 ); }
	static Color BLUE() { return Color( 0.0, 0.0, 1.0 ); }
};

} /* namespace types */
