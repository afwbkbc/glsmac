#pragma once

#include "SimpleButton.h"

#include "Label.h"

namespace ui {
namespace object {

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

} /* namespace object */
} /* namespace ui */
