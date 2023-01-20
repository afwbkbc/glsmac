#pragma once

#include "base/Module.h"

#include <vector>

#include "scene/actor/SoundActor.h"

namespace audio {

MAJOR_CLASS( Audio, base::Module )

	virtual ~Audio();

	// TODO: scenes?
	virtual void AddActor( scene::actor::SoundActor *actor ) = 0;
	virtual void RemoveActor( scene::actor::SoundActor *actor ) = 0;
	
	void Iterate();
	
	// if sound is still playing - don't delete it until it finishes
	void RemoveAndDeleteActor( scene::actor::SoundActor *actor );
	
protected:
	vector< scene::actor::SoundActor* > m_removal_queue = {};
	
};

} /* namespace audio */
