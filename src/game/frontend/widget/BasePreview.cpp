#include "BasePreview.h"

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "game/frontend/base/Base.h"
#include "types/mesh/Rectangle.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "game/frontend/Game.h"
#include "game/backend/base/Base.h"
#include "game/frontend/base/BaseManager.h"

namespace game {
namespace frontend {
namespace widget {

BasePreview::BasePreview( Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_BASE_PREVIEW, "base-preview", {
		{ "base", { gse::VT_OBJECT, ::game::backend::base::Base::WRAP_CLASS } },
	}
) {}

void BasePreview::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			const auto* const b = ::game::backend::base::Base::Unwrap( data->value.at( "base" ) );
			ASSERT( b, "invalid base ptr" );
			m_game->SetWidgetRelation( m_type, b->m_id, widget );
		}
	);
	widget->OnRemove(
		F_WIDGET_REMOVE( this ) {
			m_game->ClearWidgetRelation( m_type, widget );
		}
	);
}

void BasePreview::Update( ui::dom::Widget* const widget, const void* const data ) {
	const auto* const base = (const base::Base*)data;

	widget->Clear();

	if ( !base ) {
		return;
	}

	const auto& render = base->GetRenderData();

	size_t index = 0;

	ASSERT( render.base.mesh->GetType() == types::mesh::Mesh::MT_RENDER, "base mesh not render" );

	AddMeshAndTexture( widget, index++, render.base.mesh, render.base.texture, true );
	AddMeshAndTexture( widget, index++, render.badge.mesh, render.badge.texture, true, { 0.16f, 0.27f }, { 0.04f, 0.04f } );
}

}
}
}
