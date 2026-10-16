#pragma once

#include "Production.h"

#include "game/backend/base/RenderInfo.h"

namespace game {
namespace backend {
namespace base {
namespace production {

class Custom : public Production {
public:
	Custom( const RenderInfo& render_info );
	virtual ~Custom() = default;

private:
	friend class Production;
	static const Production* const DeserializeImpl( types::Buffer& buf );

protected:
	void SerializeImpl( types::Buffer& buf ) const override;

private:
	const RenderInfo m_render_info;

};

}
}
}
}
