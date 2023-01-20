#include "Sound.h"

namespace types {

Sound::~Sound() {
	if ( m_buffer ) {
		free( m_buffer );
	}
}

}
