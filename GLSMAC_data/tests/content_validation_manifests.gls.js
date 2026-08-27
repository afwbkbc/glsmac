const content = #include('./_content_validation_common');

let invalid = content.make_catalog();
invalid.unit_manifest.weapons[1].required_technology = 'MissingTechnology';
test.assert(content.validator.validate(invalid).errors == [
	'unit_manifest.weapons.Laser.required_technology: references missing technology MissingTechnology',
]);

invalid = content.make_catalog();
invalid.unit_manifest.predefined_units[0].chassis = 'MissingChassis';
test.assert(content.validator.validate(invalid).errors == [
	'unit_manifest.predefined_units.ColonyPod.chassis: references missing chassis MissingChassis',
]);

invalid = content.make_catalog();
invalid.factions[0].data.starting_technologies :+'CentauriEcology';
test.assert(content.validator.validate(invalid).errors == [
	'factions.GAIANS.starting_technologies[1]: duplicates starting technology CentauriEcology',
]);

const cyclic = content.validator.validate({
	technologies: {
		definitions: {
			Alpha: {id: 'Alpha', name: 'Alpha', cost: 10, commerce_bonus: 0, prerequisites: ['Beta']},
			Beta: {id: 'Beta', name: 'Beta', cost: 10, commerce_bonus: 0, prerequisites: ['Alpha']},
		},
		order: ['Alpha', 'Beta'],
	},
	facilities: [],
	facility_manifest: [],
	facility_coverage: {complete: 0, partial: 0, status: {}},
	project_coverage: {complete: 0, partial: 0, status: {}},
	units: [],
	unit_manifest: {
		chassis: [],
		reactors: [],
		weapons: [],
		armors: [],
		abilities: [],
		predefined_units: [],
	},
	moralesets: [],
	factions: [],
});
test.assert(cyclic.errors == [
	'technologies: dependency cycle prevents resolution of: Alpha, Beta',
]);
