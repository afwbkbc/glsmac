#pragma once

namespace ui {

class UI;

namespace dom {

class Focusable {
public:
	Focusable();
	~Focusable();

	virtual void OnFocus() = 0;
	virtual void OnDefocus() = 0;

};

}
}
