#pragma once

#include <string>

#include "base/ObjectLink.h"

namespace types {

class Texture {
public:
	Texture( const std::string & name ) : m_name( name ) {

	}
	~Texture() {
		if ( m_bitmap )
			delete m_bitmap;
		if ( m_renderer_object )
			m_renderer_object->Remove();
	}
	const std::string m_name;
	size_t m_width = 0;
	size_t m_height = 0;
	unsigned char m_bpp = 0;
	unsigned char *m_bitmap = NULL;
	size_t m_bitmap_size = 0;

	base::ObjectLink *m_renderer_object = NULL;
};

} /* namespace types */
