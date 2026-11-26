#pragma once

#include "Production.h"

namespace game {
namespace backend {

namespace unit {
class Def;
}

namespace base {

namespace production {

class Unit : public Production {
public:
	Unit( const std::string& id, const unit::Def* const def );
	virtual ~Unit() = default;

	const uint8_t GetGroupOrder() const override;
	gse::value::Object* const ToValue( GSE_CALLABLE ) const override;

private:
	friend class Production;
	static const Production* const DeserializeImpl( types::Buffer& buf, const std::string& id, const std::string& hint, const std::string& intent );

protected:
	void SerializeImpl( types::Buffer& buf ) const override;

private:
	const unit::Def* const m_def;

};

}
}
}
}
