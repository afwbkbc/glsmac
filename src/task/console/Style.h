#pragma once

#include "ui/theme/StyleSheet.h"

namespace task {
namespace console {

CLASS( Style, ::ui::theme::StyleSheet )

	void AddStyles() override;
};

}
}
