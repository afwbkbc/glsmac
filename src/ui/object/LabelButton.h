#pragma once

#include "SimpleButton.h"

namespace ui {
namespace object {

class Label;

CLASS( LabelButton, SimpleButton )

	LabelButton( const std::string& class_name = "" );

	void Create() override;
	void Destroy() override;

	void SetLabel( const std::string& text );
	const std::string& GetLabel() const;

protected:
	std::string m_label_text = "";
	Label* m_label = nullptr;

};

}
}
