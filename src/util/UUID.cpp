#include "UUID.h"

#ifndef _WIN32 // TODO: fix ossp-uuid dependency
#include <uuid++.hh>
#else
#include <random>
#include <sstream>
static std::random_device rd;
static std::mt19937 gen( rd() );
static std::uniform_int_distribution<> dis( 0, 15 );
static std::uniform_int_distribution<> dis2( 8, 11 );
#endif

namespace util {

const std::string UUID::GenerateRandom() {
#ifndef _WIN32 // TODO: fix ossp-uuid dependency
	uuid id;
	id.make( UUID_MAKE_V1 );
	const char* myId = id.string();
	std::string result( id.string() );
	delete myId;
	return result;
#else
	std::stringstream ss;
	int i;
	ss << std::hex;
	for ( i = 0; i < 8; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	for ( i = 0; i < 4; i++ ) {
		ss << dis( gen );
	}
	ss << "-4";
	for ( i = 0; i < 3; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	ss << dis2( gen );
	for ( i = 0; i < 3; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	for ( i = 0; i < 12; i++ ) {
		ss << dis( gen );
	};
	return ss.str();
#endif
}

}
