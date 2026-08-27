const content = #include('./_content_validation_common');

let invalid = content.make_catalog();
invalid.technologies.definitions.Biogenetics.cost_typo = 30;
test.assert(content.validator.validate(invalid).errors == [
	'technologies.Biogenetics.cost_typo: is not a supported field',
]);

invalid = content.make_catalog();
content.get_facility(invalid, 'RecyclingTanks').data.nutrient_typo = 1;
test.assert(content.validator.validate(invalid).errors == [
	'facilities.RecyclingTanks.nutrient_typo: is not a supported field',
]);

invalid = content.make_catalog();
invalid.facility_coverage.status.RecyclingTanks = 'unknown';
test.assert(content.validator.validate(invalid).errors == [
	'facility_coverage.RecyclingTanks: must be complete or partial',
	'facility_coverage.complete: reports 37 but contains 36',
]);

invalid = content.make_catalog();
content.get_facility(invalid, 'RecyclingTanks').data.required_technology =
	'MissingTechnology';
test.assert(content.validator.validate(invalid).errors == [
	'facilities.RecyclingTanks.required_technology: references missing technology MissingTechnology',
	'facilities.RecyclingTanks.required_technology: does not match base-game manifest value Biogenetics',
]);
