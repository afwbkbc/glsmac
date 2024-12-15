#pragma once

#include <vector>

#include "BBSection.h"

namespace ui_legacy::object {
class Section;
class Label;
class Mesh;
}

namespace game {
namespace frontend {

class TileObject;
namespace ui_legacy {

CLASS( ObjectPreview, BBSection )

	ObjectPreview( Game* game );

	void Create() override;
	void Destroy() override;

	void PreviewObject( const TileObject* object );
	void HideObjectPreview();

private:
	const TileObject* m_object = nullptr;

	void* m_ui_state = nullptr;

	std::vector< ::ui_legacy::object::Label* > m_labels = {};

};

}
}
}
