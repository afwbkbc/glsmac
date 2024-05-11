#pragma once

#include "Util.h"

#include "types/Vec3.h"

namespace util {

CLASS( Math, Util )

	static types::Vec3 Cross( const types::Vec3& a, const types::Vec3& b );
	static float Dot( const types::Vec3& a, const types::Vec3& b );
	static types::Vec3 Normalize( const types::Vec3& v );

};

}
