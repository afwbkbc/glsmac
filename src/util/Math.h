#pragma once

#include "Util.h"

#include "types/Vec3.h"

using namespace types;

namespace util {

CLASS( Math, Util )

	static Vec3 Cross( const Vec3& a, const Vec3& b );
	static float Dot( const Vec3& a, const Vec3& b );
	static Vec3 Normalize( const Vec3& v );

};

}
