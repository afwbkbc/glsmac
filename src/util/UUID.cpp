#include "UUID.h"

#include "common/Mutex.h"

#include <random>
#include <sstream>

namespace util {

static common::Mutex* s_mutex = nullptr;

void UUID::Init() {
	ASSERT( !s_mutex, "mutex already initialized" );
	s_mutex = new common::Mutex();
}

const std::string UUID::Generate() {
	ASSERT( s_mutex, "mutex not initialized" );

	std::lock_guard guard( *s_mutex );

	static std::random_device rd;
	static std::mt19937 gen( rd() );
	static std::uniform_int_distribution<> dis( 0, 15 );
	static std::uniform_int_distribution<> dis2( 8, 11 );

	std::stringstream ss;
	ss << std::hex;
	for ( int i = 0; i < 8; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	for ( int i = 0; i < 4; i++ ) {
		ss << dis( gen );
	}
	ss << "-4";
	for ( int i = 0; i < 3; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	ss << dis2( gen );
	for ( int i = 0; i < 3; i++ ) {
		ss << dis( gen );
	}
	ss << "-";
	for ( int i = 0; i < 12; i++ ) {
		ss << dis( gen );
	}
	return ss.str();
}

}
