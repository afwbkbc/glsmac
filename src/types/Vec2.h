#pragma once

#include <string>

namespace types {

template< typename DATATYPE = float >
class Vec2 {
public:
	Vec2() {
		this->Set( 0.0f, 0.0f );
	}

	Vec2( DATATYPE x, DATATYPE y ) {
		this->x = x;
		this->y = y;
	}

	~Vec2() {

	}

	void Set( DATATYPE x, DATATYPE y ) {
		this->x = x;
		this->y = y;
	}

	const Vec2< DATATYPE > operator-( Vec2< DATATYPE > other ) const {
		return Vec2< DATATYPE >( x - other.x, y - other.y );
	}

	const Vec2< DATATYPE > operator+( Vec2< DATATYPE > other ) const {
		return Vec2< DATATYPE >( x + other.x, y + other.y );
	}

	const Vec2< DATATYPE > operator*( DATATYPE other ) const {
		return Vec2< DATATYPE >( x * other, y * other );
	}

	const Vec2< DATATYPE > operator/( DATATYPE other ) const {
		return Vec2< DATATYPE >( x / other, y / other );
	}

	bool operator==( const Vec2< DATATYPE >& other ) const {
		return x == other.x && y == other.y;
	}

	bool operator!=( const Vec2< DATATYPE >& other ) const {
		return x != other.x || y != other.y;
	}

	operator bool() const {
		return x || y;
	}

	DATATYPE x;
	DATATYPE y;

	const std::string ToString() const {
		return "[ " + std::to_string( x ) + " " + std::to_string( y ) + " ]";
	}
};

}
