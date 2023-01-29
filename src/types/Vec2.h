#pragma once

namespace types {

template <typename DATATYPE = float>
class Vec2 {
public:
	Vec2() {
		this->Set(0.0f,0.0f);
	}
	Vec2(DATATYPE x,DATATYPE y) {
		this->x=x;
		this->y=y;
	}
	~Vec2() {

	}
	void Set(DATATYPE x,DATATYPE y) {
		this->x=x;
		this->y=y;
	}
	
	Vec2<DATATYPE> operator- ( Vec2<DATATYPE> other ) {
		return Vec2<DATATYPE>( x - other.x, y - other.y );
	}
	Vec2<DATATYPE> operator+ ( Vec2<DATATYPE> other ) {
		return Vec2<DATATYPE>( x + other.x, y + other.y );
	}
	Vec2<DATATYPE> operator* ( DATATYPE other ) {
		return Vec2<DATATYPE>( x * other, y * other );
	}
	Vec2<DATATYPE> operator/ ( DATATYPE other ) {
		return Vec2<DATATYPE>( x / other, y / other );
	}
	
	DATATYPE x;
	DATATYPE y;
};

} /* namespace types */
