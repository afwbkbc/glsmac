#pragma once

#include "types/Serializable.h"

namespace game {

CLASS( Turn, types::Serializable )
	Turn( const size_t year );
	const size_t GetYear() const;
private:
	const size_t m_year;
};

}