#pragma once

#include <vector>

#include "ObjectsListBase.h"

namespace game {
namespace frontend {
namespace ui_legacy {

class ObjectPreview;

CLASS( ObjectsList, ObjectsListBase )

	ObjectsList( Game* game, ObjectPreview* object_preview );

protected:
	bool OnObjectMouseOver( TileObject* const object ) override;
	bool OnObjectMouseOut( TileObject* const object ) override;

private:
	void PreviewObject( TileObject* object );
	void HideObjectPreview( TileObject* object );

	ObjectPreview* m_object_preview;
	TileObject* m_previewing_object = nullptr;
};

}
}
}
