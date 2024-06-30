#pragma once

#include <vector>

#include "common/Module.h"

namespace scene::actor {
class Sound;
}

namespace audio {

CLASS( Audio, common::Module )

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

}
