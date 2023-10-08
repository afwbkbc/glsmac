#include "UUID.h"

#include <mutex>

namespace util {

struct uuid_state_t {
	uuid_state_t() {
		uuid_rc_t rc;
		if ( ( rc = uuid_create( &ctx ) ) != UUID_RC_OK ) {
			THROW( "unable to initialize UUID generator" );
		}
	}
	~uuid_state_t() {
		uuid_destroy( ctx );
	}
	uuid_t* ctx = nullptr;
};

static std::mutex s_uuid_state_mutex;
static uuid_state_t s_uuid_state;

const std::string UUID::Generate( unsigned int mode ) {
	std::lock_guard< std::mutex > guard( s_uuid_state_mutex );

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
}

}
