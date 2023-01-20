#pragma once

#include "base/Base.h"

#include <string>

//#include "base/ObjectLink.h"

namespace types {

MAJOR_CLASS( Sound, base::Base )

	~Sound();
	
	unsigned char* m_buffer = nullptr;
	size_t m_buffer_size = 0;
	
	// based on SDL_AudioSpec so some adapting maybe needed for other sound loaders / audio modules
	struct {
		size_t freq; // samples per sec
		uint16_t format; // SDL2 audio data format (TODO: untie from SDL2)
		uint8_t channels; // number of channels
		uint8_t silence; // silence value
		uint16_t samples;  // samples / channels
		uint16_t padding; // necessary for some compile environment
		uint32_t size; // audio buffer size (bytes)
	} m_spec;
	
	string m_name = "";	
};

}
