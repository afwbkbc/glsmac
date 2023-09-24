#pragma once

#include <vector>

#include "base/Module.h"

#include "scene/actor/Sound.h"

namespace audio {

CLASS( Audio, base::Module )

	virtual ~Audio();

	// TODO: scenes?
	virtual void AddActor( scene::actor::Sound* actor ) = 0;
	virtual void RemoveActor( scene::actor::Sound* actor ) = 0;

	void Iterate() override;

	// if sound is still playing - don't delete it until it finishes
	void RemoveAndDeleteActor( scene::actor::Sound* actor );

protected:
	std::vector< scene::actor::Sound* > m_removal_queue = {};

};

} /* namespace audio */
