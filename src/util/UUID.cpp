#include "UUID.h"

#include <mutex>

#ifdef _WIN32
#include <random>
#include <sstream>
#endif

namespace util {

static std::mutex s_uuid_state_mutex;

#ifndef _WIN32// TODO: make ossp-uuid buildable on windows

struct uuid_state_t {
	uuid_state_t() {
		uuid_rc_t rc;
		if ( ( rc = uuid_create( &ctx ) ) != UUID_RC_OK ) {
			THROW( "unable to initialize UUID generator: " + std::to_string( rc ) );
		}
	}
	~uuid_state_t() {
		uuid_destroy( ctx );
	}
	uuid_t* ctx = nullptr;
};
static uuid_state_t s_uuid_state;

#else

static std::random_device rd;
static std::mt19937 gen( rd() );
static std::uniform_int_distribution<> dis( 0, 15 );
static std::uniform_int_distribution<> dis2( 8, 11 );

#endif

const std::string UUID::Generate( unsigned int mode ) {
	std::lock_guard guard( s_uuid_state_mutex );

#ifndef _WIN32// TODO: make ossp-uuid buildable on windows
	uuid_rc_t rc;
	if ( ( rc = uuid_make( s_uuid_state.ctx, mode ) ) != UUID_RC_OK ) {
		THROW( "failed to generate UUID (error code: " + std::to_string( rc ) + " )" );
	}
	char* str = NULL;
	if ( ( rc = uuid_export( s_uuid_state.ctx, UUID_FMT_STR, (void**)&str, NULL ) ) != UUID_RC_OK ) {
		THROW( "failed to format UUID (error code: " + std::to_string( rc ) + " )" );
	}
	std::string result( str );
	free( str );
	return result;
#else
	// this algorithm is bad, used here only until ossp-uuid build is fixed
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
