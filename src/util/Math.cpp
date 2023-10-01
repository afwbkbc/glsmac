#include "Math.h"

namespace util {

Vec3 Math::Cross( const Vec3& a, const Vec3& b ) {
	Vec3 res;
	res.x = a.y * b.z - a.z * b.y;
	res.y = a.z * b.x - a.x * b.z;
	res.z = a.x * b.y - a.y * b.x;
	return res;
}

float Math::Dot( const Vec3& a, const Vec3& b ) {
	return
		a.x * b.x +
			a.y * b.y +
			a.z * b.z;
}

Vec3 Math::Normalize( const Vec3& v ) {
	float l = sqrt( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) );
	return {
		v.x / l,
		v.y / l,
		v.z / l
	};
}

}
