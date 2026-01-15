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

BasePreview::BasePreview( const Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_BASE_PREVIEW, "base-preview", {
		{ "base", { gse::VT_OBJECT, ::game::backend::base::Base::WRAP_CLASS } },
	}
) {}

void BasePreview::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			const auto* const u = ::game::backend::base::Base::Unwrap( data->value.at( "base" ) );
			ASSERT( u, "invalid base ptr" );
			const auto* const base = m_game->GetBM()->GetBaseById( u->m_id );
			ASSERT( base, "invalid base" );
			Update( widget, base );
		}
	);
}

void BasePreview::Update( ui::dom::Widget* const widget, const base::Base* const base ) {
	widget->Clear();

	const auto& render = base->GetRenderData();

	size_t index = 0;

	ASSERT( render.base.mesh->GetType() == types::mesh::Mesh::MT_RECTANGLE, "base mesh not rectangle" ); // TODO: 3d bases

	AddMeshAndTexture( widget, index++, render.base.mesh, render.base.texture, true );
	AddMeshAndTexture( widget, index++, render.badge.mesh, render.badge.texture, true, { 0.16f, 0.27f }, { 0.04f, 0.04f } );
}

}
}
}
