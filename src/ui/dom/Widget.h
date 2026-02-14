#pragma once

#include <unordered_map>

#include "Area.h"

#include "ui/Types.h"

namespace game::frontend::widget {
class Widget;
}

namespace types {
namespace texture {
class Texture;
}
}

namespace scene::actor {
class Actor;
}

namespace ui {
namespace dom {

class Widget : public Area {
public:

	Widget( DOM_ARGS );

	void Show() override;
	void Hide() override;
	void Destroy( GSE_CALLABLE ) override;

	void SetTexture( types::texture::Texture* const texture, const size_t index = 0 );
	void SetActor( scene::actor::Actor* const actor, const size_t index = 0 );
	void Clear();

	types::texture::Texture* const GetTexture( const size_t index = 0 ) const;

	void OnUpdate( const f_widget_update_t& on_widget_update );
	void OnRemove( const f_widget_remove_t& on_widget_remove );

private:

	widget_type_t m_type = WT_NONE;
	void Enable( const widget_type_t type );
	void Disable();
	void SetData( gse::value::Object* const data );

	bool m_is_attached = false;
	void Attach();
	void Detach();

	gse::value::Object* m_data = nullptr;
	bool m_data_update_needed = false;

	std::unordered_map< size_t, types::texture::Texture* > m_textures = {};
	std::unordered_map< size_t, scene::actor::Actor* > m_actors = {};

	f_widget_update_t m_on_widget_update = nullptr;
	f_widget_remove_t m_on_widget_remove = nullptr;
	void UpdateWidget();

};

}
}
