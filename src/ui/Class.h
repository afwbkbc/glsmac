#pragma once

#include <unordered_set>
#include <vector>
#include <string>

#include "gse/Wrappable.h"
#include "gse/value/Object.h"

#include "Types.h"

namespace ui {

class UI;

namespace dom {
class Object;
}

class Class : public gse::Wrappable {
public:
	Class( const UI* const ui, const std::string& name, const bool is_master );
	~Class();

	const std::string& GetName() const;
	const std::pair< gse::Value*, class_modifier_t > GetProperty( const std::string& key, const class_modifiers_t& modifiers ) const;
	const properties_t& GetProperties() const;

	void AddObject( GSE_CALLABLE, dom::Object* object, const class_modifiers_t& modifiers );
	void RemoveObject( GSE_CALLABLE, dom::Object* object );
	void UpdateObject( GSE_CALLABLE, dom::Object* const object, const class_modifier_t modifier );
	void AddObjectModifier( GSE_CALLABLE, dom::Object* object, const class_modifier_t modifier );
	void RemoveObjectModifier( GSE_CALLABLE, dom::Object* object, const class_modifier_t modifier );

	virtual gse::Value* const Wrap( GSE_CALLABLE, const bool dynamic = false ) override;
	virtual void WrapSet( const std::string& key, gse::Value* const value, GSE_CALLABLE );
	static void WrapSetStatic( gse::Wrappable* wrapobj, const std::string& key, gse::Value* const value, GSE_CALLABLE );

private:
	const UI* const m_ui;
	const std::string m_name;
	const bool m_is_master;

	properties_t m_local_properties = {};
	properties_t m_properties = {};

	void SetProperty( GSE_CALLABLE, const std::string& name, gse::Value* const value );
	void SetPropertyFromParent( GSE_CALLABLE, const std::string& name, gse::Value* const value );
	void SetPropertiesFromParent( GSE_CALLABLE );
	void UnsetProperty( GSE_CALLABLE, const std::string& name );
	void UnsetPropertyFromParent( GSE_CALLABLE, const std::string& name );
	void UnsetProperties( GSE_CALLABLE, const std::vector< std::string >& properties );
	void UnsetPropertiesFromParent( GSE_CALLABLE );

	std::unordered_map< dom::Object*, class_modifiers_t > m_objects = {};

	Class* m_parent_class = nullptr;
	std::unordered_set< Class* > m_child_classes = {};
	void AddChildClass( GSE_CALLABLE, Class* const cls );
	void RemoveChildClass( GSE_CALLABLE, Class* const cls );

	std::unordered_map< class_modifier_t, Class* > m_subclasses = {};

private:
	friend class UI;
	void SetProperties( GSE_CALLABLE, const properties_t& properties ); // can be set in constructor too
	void SetParentClass( GSE_CALLABLE, const std::string& name );

};

}
