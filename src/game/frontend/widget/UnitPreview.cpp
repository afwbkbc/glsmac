#include "UnitPreview.h"

#include "ui/UI.h"
#include "ui/dom/Widget.h"
#include "game/frontend/unit/Unit.h"
#include "types/mesh/Rectangle.h"
#include "ui/geometry/Rectangle.h"
#include "scene/actor/Mesh.h"
#include "game/frontend/Game.h"
#include "game/backend/unit/Unit.h"
#include "game/frontend/unit/UnitManager.h"

namespace game {
namespace frontend {
namespace widget {

UnitPreview::UnitPreview( const Game* const game, ui::UI* const ui )
	: Widget(
	game, ui, ui::WT_UNIT_PREVIEW, "unit-preview", {
		{ "unit", { gse::VT_OBJECT, ::game::backend::unit::Unit::WRAP_CLASS } },
	}
) {}

void UnitPreview::Register( ui::dom::Widget* const widget ) {
	widget->OnUpdate(
		F_WIDGET_UPDATE( this ) {
			const auto* const u = ::game::backend::unit::Unit::Unwrap( data->value.at( "unit" ) );
			ASSERT( u, "invalid unit ptr" );
			const auto* const unit = m_game->GetUM()->GetUnitById( u->m_id );
			if ( unit ) {
				Update( widget, unit );
			}
		}
	);
}

void UnitPreview::Update( ui::dom::Widget* const widget, const unit::Unit* const unit ) {
	widget->Clear();

	const auto& render = unit->GetRenderData();

	size_t index = 0;

	ASSERT( render.unit.mesh->GetType() == types::mesh::Mesh::MT_RENDER, "unit mesh not render" );

	AddMeshAndTexture( widget, index++, render.unit.mesh, render.unit.texture, true );
	AddMeshAndTexture( widget, index++, render.badge.mesh, render.badge.texture, true, { 0.23f, 0.4f }, { 0.06f, 0.08f } );
	AddMeshAndTexture( widget, index++, render.healthbar.mesh, render.healthbar.texture, true, { 0.03f, 0.2933f }, { 0.12f, 0.16f } );
}

}
}
}
