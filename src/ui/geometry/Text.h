#pragma once

#include "Geometry.h"

namespace scene::actor {
class Text;
}

namespace ui {
namespace geometry {

class Text : public Geometry {
public:
	Text( const UI* const ui, Geometry* const parent );

	void SetActor( scene::actor::Text* const actor );

protected:
	void UpdateImpl() override;

private:
	scene::actor::Text* m_actor = nullptr;

};

}
}
