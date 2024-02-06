#pragma once

#include <string>

namespace game {
namespace unit {

class Def {
public:
	Def( const std::string& name );
	const std::string& GetName() const;
private:
	const std::string m_name;
};

}
}
