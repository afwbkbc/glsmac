const validator = #include('../default/content/validator');
const technologies = #include('../default/technologies');
const facilities = #include('../default/facilities');
const units = #include('../default/units');
const factions = #include('../default/factions');

const make_catalog = () => {
	return {
		technologies: {
			definitions: #clone(technologies.definitions),
			order: #clone(technologies.order),
		},
		facilities: #clone(facilities.definitions),
		facility_manifest: #clone(facilities.manifest),
		facility_coverage: #clone(facilities.coverage),
		project_coverage: #clone(facilities.project_coverage),
		units: #clone(units.definitions),
		unit_manifest: #clone(units.manifest),
		moralesets: #clone(units.moralesets),
		factions: #clone(factions.definitions),
	};
};

const get_facility = (catalog, id) => {
	for (entry of catalog.facilities) {
		if (entry.id == id) {
			return entry;
		}
	}
	return null;
};

const get_unit_by_chassis = (catalog, chassis) => {
	for (entry of catalog.units) {
		if (entry.data.chassis == chassis) {
			return entry;
		}
	}
	return null;
};

const set_manifest_required_project = (catalog, id, required_project) => {
	for (let i = 0; i < #sizeof(catalog.facility_manifest); i++) {
		const entry = catalog.facility_manifest[i];
		if (entry.id == id) {
			catalog.facility_manifest[i] = {
				id: entry.id,
				name: entry.name,
				kind: entry.kind,
				mineral_cost: entry.mineral_cost,
				energy_maintenance: entry.energy_maintenance,
				required_technology: entry.required_technology,
				required_project: required_project,
				obsolete_technology: entry.obsolete_technology,
				effect: entry.effect,
			};
			return;
		}
	}
};

return {
	validator: validator,
	technologies: technologies,
	facilities: facilities,
	units: units,
	factions: factions,
	make_catalog: make_catalog,
	get_facility: get_facility,
	get_unit_by_chassis: get_unit_by_chassis,
	set_manifest_required_project: set_manifest_required_project,
};
