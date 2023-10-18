#include "Event.h"

#include <cstring>

#include "base/Base.h"

namespace game {

Event::Event( const Event::event_type_t type )
	: type( type ) {
	memset( &data, 0, sizeof( data ) );
}

Event::Event( const Event& other )
	: type( other.type ) {
	switch ( type ) {
		case ET_QUIT: {
			NEW( data.quit.reason, std::string, *other.data.quit.reason );
			break;
		}
		default: {
			//
		}
	}
}

Event::~Event() {
	switch ( type ) {
		case ET_QUIT: {
			if ( data.quit.reason ) {
				DELETE( data.quit.reason );
			}
			break;
		}
		default: {
			//
		}
	}
}

}
