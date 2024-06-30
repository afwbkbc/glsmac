#include <numeric>
#include <random>
#include <algorithm>

#include "Perlin.h"

// based on https://github.com/sol-prog/Perlin_Noise

namespace util {

// Initialize with the reference values for the permutation vector
Perlin::Perlin() {

	// Initialize the permutation vector with the reference values
	p = {
		151,
		160,
		137,
		91,
		90,
		15,
		131,
		13,
		201,
		95,
		96,
		53,
		194,
		233,
		7,
		225,
		140,
		36,
		103,
		30,
		69,
		142,
		8,
		99,
		37,
		240,
		21,
		10,
		23,
		190,
		6,
		148,
		247,
		120,
		234,
		75,
		0,
		26,
		197,
		62,
		94,
		252,
		219,
		203,
		117,
		35,
		11,
		32,
		57,
		177,
		33,
		88,
		237,
		149,
		56,
		87,
		174,
		20,
		125,
		136,
		171,
		168,
		68,
		175,
		74,
		165,
		71,
		134,
		139,
		48,
		27,
		166,
		77,
		146,
		158,
		231,
		83,
		111,
		229,
		122,
		60,
		211,
		133,
		230,
		220,
		105,
		92,
		41,
		55,
		46,
		245,
		40,
		244,
		102,
		143,
		54,
		65,
		25,
		63,
		161,
		1,
		216,
		80,
		73,
		209,
		76,
		132,
		187,
		208,
		89,
		18,
		169,
		200,
		196,
		135,
		130,
		116,
		188,
		159,
		86,
		164,
		100,
		109,
		198,
		173,
		186,
		3,
		64,
		52,
		217,
		226,
		250,
		124,
		123,
		5,
		202,
		38,
		147,
		118,
		126,
		255,
		82,
		85,
		212,
		207,
		206,
		59,
		227,
		47,
		16,
		58,
		17,
		182,
		189,
		28,
		42,
		223,
		183,
		170,
		213,
		119,
		248,
		152,
		2,
		44,
		154,
		163,
		70,
		221,
		153,
		101,
		155,
		167,
		43,
		172,
		9,
		129,
		22,
		39,
		253,
		19,
		98,
		108,
		110,
		79,
		113,
		224,
		232,
		178,
		185,
		112,
		104,
		218,
		246,
		97,
		228,
		251,
		34,
		242,
		193,
		238,
		210,
		144,
		12,
		191,
		179,
		162,
		241,
		81,
		51,
		145,
		235,
		249,
		14,
		239,
		107,
		49,
		192,
		214,
		31,
		181,
		199,
		106,
		157,
		184,
		84,
		204,
		176,
		115,
		121,
		50,
		45,
		127,
		4,
		150,
		254,
		138,
		236,
		205,
		93,
		222,
		114,
		67,
		29,
		24,
		72,
		243,
		141,
		128,
		195,
		78,
		66,
		215,
		61,
		156,
		180
	};
	// Duplicate the permutation vector
	p.insert( p.end(), p.begin(), p.end() );
}

// Generate a new permutation vector based on the value of seed
Perlin::Perlin( unsigned int seed ) {
	p.resize( 256 );

	// Fill p with values from 0 to 255
	std::iota( p.begin(), p.end(), 0 );

	// Initialize a random engine with seed
	std::default_random_engine engine( seed );

	// Suffle  using the above random engine
	std::shuffle( p.begin(), p.end(), engine );

	// Duplicate the permutation vector
	p.insert( p.end(), p.begin(), p.end() );
}

float Perlin::Noise( float x, float y, float z ) {

	// Find the unit cube that contains the point
	int X = (int)floor( x ) & 255;
	int Y = (int)floor( y ) & 255;
	int Z = (int)floor( z ) & 255;

	// Find relative x, y,z of point in cube
	x -= floor( x );
	y -= floor( y );
	z -= floor( z );

	// Compute Fade curves for each of x, y, z
	float u = Fade( x );
	float v = Fade( y );
	float w = Fade( z );

	// Hash coordinates of the 8 cube corners
	int A = p[ X ] + Y;
	int AA = p[ A ] + Z;
	int AB = p[ A + 1 ] + Z;
	int B = p[ X + 1 ] + Y;
	int BA = p[ B ] + Z;
	int BB = p[ B + 1 ] + Z;

	// Add blended results from 8 corners of cube
	float res = Lerp( w, Lerp( v, Lerp( u, Grad( p[ AA ], x, y, z ), Grad( p[ BA ], x - 1, y, z ) ), Lerp( u, Grad( p[ AB ], x, y - 1, z ), Grad( p[ BB ], x - 1, y - 1, z ) ) ), Lerp( v, Lerp( u, Grad( p[ AA + 1 ], x, y, z - 1 ), Grad( p[ BA + 1 ], x - 1, y, z - 1 ) ), Lerp( u, Grad( p[ AB + 1 ], x, y - 1, z - 1 ), Grad( p[ BB + 1 ], x - 1, y - 1, z - 1 ) ) ) );

	//res = (res + 1.0)/2.0;

	ASSERT( res >= -1, "perlin res overflow ( " + std::to_string( res ) + " < -1 )" );
	ASSERT( res <= 1, "perlin res overflow ( " + std::to_string( res ) + " > 1 )" );
	//Log( "Noise( " + to_string( x ) + "x" + to_string( y ) + " ) = " + to_string( res ) );
	return res;
}

float Perlin::Noise( float x, float y, float z, size_t passes ) {
	float res = 0;
	float scale = 1.0f;
	for ( size_t i = 0 ; i < passes ; i++ ) {
		res += Noise( x * scale, y * scale, i );
		scale /= 2;
	}

	res = std::max( -1.0f, std::min( 1.0f, res ) );

	//Log( "Noise( " + to_string( x ) + "x" + to_string( y ) + ":" + to_string( passes ) + " ) = " + to_string( res ) );
	return res;
}

float Perlin::Fade( float t ) {
	return t * t * t * ( t * ( t * 6 - 15 ) + 10 );
}

float Perlin::Lerp( float t, float a, float b ) {
	return a + t * ( b - a );
}

float Perlin::Grad( int hash, float x, float y, float z ) {
	int h = hash & 15;
	// Convert lower 4 bits of hash into 12 Gradient directions
	float u = h < 8
		? x
		: y,
		v = h < 4
		? y
		: h == 12 || h == 14
			? x
			: z;
	return ( ( h & 1 ) == 0
		? u
		: -u
	) + ( ( h & 2 ) == 0
		? v
		: -v
	);
}

}
