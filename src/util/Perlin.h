#pragma once

// based on https://github.com/sol-prog/Perlin_Noise

#include <vector>

#include "Util.h"

namespace util {

CLASS( Perlin, Util )

	Perlin();
	Perlin( unsigned int seed );

	// Get a noise value, for 2D images z can have any value
	float Noise( float x, float y, float z );

	// multi-level noise
	float Noise( float x, float y, float z, size_t passes );

private:

	// The permutation vector
	std::vector< int > p;

	float Fade( float t );
	float Lerp( float t, float a, float b );
	float Grad( int hash, float x, float y, float z );
};

}
