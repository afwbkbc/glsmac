#include "Math.h"

namespace util {

types::Vec3 Math::Cross( const types::Vec3& a, const types::Vec3& b ) {
	types::Vec3 res;
	res.x = a.y * b.z - a.z * b.y;
	res.y = a.z * b.x - a.x * b.z;
	res.z = a.x * b.y - a.y * b.x;
	return res;
}

float Math::Dot( const types::Vec3& a, const types::Vec3& b ) {
	return
		a.x * b.x +
			a.y * b.y +
			a.z * b.z;
}

types::Vec3 Math::Normalize( const types::Vec3& v ) {
	float l = sqrt( ( v.x * v.x ) + ( v.y * v.y ) + ( v.z * v.z ) );
	return {
		v.x / l,
		v.y / l,
		v.z / l
	};
}

}
