#pragma once

#include "Production.h"

#include "game/backend/base/RenderInfo.h"

namespace game {
namespace backend {
namespace base {
namespace production {

class Custom : public Production {
public:
	Custom( const RenderInfo& render_info, const std::string& name, const std::string& hint );
	virtual ~Custom() = default;

	const uint8_t GetGroupOrder() const override;
	gse::value::Object* const ToValue( GSE_CALLABLE, const std::string& id ) const override;

private:
	friend class Production;
	static const Production* const DeserializeImpl( types::Buffer& buf, const std::string& hint );

protected:
	void SerializeImpl( types::Buffer& buf ) const override;

private:
	const std::string m_name;
	const RenderInfo m_render_info;

};

}
}
}
}
