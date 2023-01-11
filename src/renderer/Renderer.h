#pragma once

#include <vector>

#include "base/Module.h"
#include "base/Error.h"

#include "scene/Scene.h"
#include "types/Texture.h"

namespace renderer {

MAJOR_CLASS( Renderer, base::Module )
	virtual void AddScene( scene::Scene *scene ) = 0;
	virtual void RemoveScene( scene::Scene *scene ) = 0;
	virtual const unsigned short GetWindowWidth() const = 0;
	virtual const unsigned short GetWindowHeight() const = 0;
	const float GetAspectRatio() const {
		return (float) GetWindowWidth() / GetWindowHeight();
	}
	
	virtual void LoadTexture( const types::Texture* texture ) = 0;
	//virtual void UnloadTexture( const types::Texture* texture ) = 0;
	virtual void EnableTexture( const types::Texture* texture ) = 0;
	virtual void DisableTexture() = 0;

protected:
};

} /* namespace renderer */
