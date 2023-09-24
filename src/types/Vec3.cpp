#include <cmath>
#include <cfloat>

#include "Vec3.h"
#include "Vec4.h"

namespace types {

#define DATATYPE float

void Vec3::Rotate( const DATATYPE angle, const Vec3 axe ) {
	const float SinHalfAngle = sin( angle / 2.0f );
	const float CosHalfAngle = cos( angle / 2.0f );

	Vec4 rq( axe.x * SinHalfAngle, axe.y * SinHalfAngle, axe.z * SinHalfAngle, CosHalfAngle );
	Vec4 cq = rq;
	cq.Conjugate();
	cq.Normalize();
	rq = rq * ( *this ) * cq;
	Set( rq.x, rq.y, rq.z );
};

const std::string Vec3::ToString() const {
	return "[ " + std::to_string( x ) + " " + std::to_string( y ) + " " + std::to_string( z ) + " ]";
}

#undef DATATYPE

} /* namespace types */
