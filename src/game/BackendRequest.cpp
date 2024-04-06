#include "BackendRequest.h"

#include <cstring>

#include "base/Base.h"

namespace game {

BackendRequest::BackendRequest( const request_type_t type )
	: type( type ) {
	memset( &data, 0, sizeof( data ) );
}

BackendRequest::BackendRequest( const BackendRequest& other )
	: type( other.type ) {

	data = other.data;

	switch ( type ) {
		default: {
			//
		}
	}
}

BackendRequest::~BackendRequest() {
	switch ( type ) {
		default: {
			//
		}
	}
}

}
