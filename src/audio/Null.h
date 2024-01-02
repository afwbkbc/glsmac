#pragma once

#include "Audio.h"

#include "scene/actor/Sound.h"

namespace audio {

CLASS( Null, Audio )

	void AddActor( scene::actor::Sound* actor ) override {}
	void RemoveActor( scene::actor::Sound* actor ) override {}

};

}
