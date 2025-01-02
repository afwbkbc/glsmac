#pragma once

#include <unordered_set>
#include <vector>
#include <string>

#include "gse/Wrappable.h"
#include "gse/type/Object.h"

#include "Types.h"

namespace ui {

class UI;

namespace dom {
class Object;
}

class Class : public gse::Wrappable {
public:
	Class( const UI* const ui );

	const properties_t& GetProperties() const;

	void AddObject( GSE_CALLABLE, dom::Object* object );
	void RemoveObject( GSE_CALLABLE, dom::Object* object );

	virtual const gse::Value Wrap( const bool dynamic = false ) override;
	virtual void WrapSet( const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si );
	static void WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, const gse::Value& value, gse::context::Context* ctx, const gse::si_t& call_si );

private:
	const UI* const m_ui;

	properties_t m_local_properties = {};
	properties_t m_properties = {};

	void SetProperty( GSE_CALLABLE, const std::string& name, const gse::Value& value );
	void SetPropertyFromParent( GSE_CALLABLE, const std::string& name, const gse::Value& value );
	void SetPropertiesFromParent( GSE_CALLABLE );
	void UnsetProperty( GSE_CALLABLE, const std::string& name );
	void UnsetPropertyFromParent( GSE_CALLABLE, const std::string& name );
	void UnsetProperties( GSE_CALLABLE, const std::vector< std::string >& properties );
	void UnsetPropertiesFromParent( GSE_CALLABLE );

	std::unordered_set< dom::Object* > m_objects = {};
	void UpdateObject( GSE_CALLABLE, dom::Object* const object );

	ui::Class* m_parent_class = nullptr;
	std::unordered_set< ui::Class* > m_child_classes = {};
	void AddChildClass( GSE_CALLABLE, ui::Class* const cls );
	void RemoveChildClass( GSE_CALLABLE, ui::Class* const cls );

private:
	friend class UI;
	void SetProperties( GSE_CALLABLE, const properties_t& properties ); // can be set in constructor too
	void SetParentClass( GSE_CALLABLE, const std::string& name );

};

}
