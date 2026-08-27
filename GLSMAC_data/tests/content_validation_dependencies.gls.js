const content = #include('./_content_validation_common');

let invalid = content.make_catalog();
content.get_facility(invalid, 'HabitationDome').data.required_facility = 'MissingFacility';
test.assert(content.validator.validate(invalid).errors == [
	'facilities.HabitationDome.required_facility: references missing facility MissingFacility',
]);

invalid = content.make_catalog();
content.get_facility(invalid, 'TheAscentToTranscendence').data.required_project =
	'MissingProject';
test.assert(content.validator.validate(invalid).errors == [
	'facilities.TheAscentToTranscendence.required_project: references missing project MissingProject',
	'facilities.TheAscentToTranscendence.required_project: does not match base-game manifest value TheVoiceOfPlanet',
]);

invalid = content.make_catalog();
content.get_facility(invalid, 'TheAscentToTranscendence').data.required_project =
	'RecyclingTanks';
test.assert(content.validator.validate(invalid).errors == [
	'facilities.TheAscentToTranscendence.required_project: must reference a project',
	'facilities.TheAscentToTranscendence.required_project: does not match base-game manifest value TheVoiceOfPlanet',
]);

invalid = content.make_catalog();
content.set_manifest_required_project(invalid, 'TheVoiceOfPlanet', 'TheAscentToTranscendence');
content.get_facility(invalid, 'TheVoiceOfPlanet').data.required_project =
	'TheAscentToTranscendence';
test.assert(content.validator.validate(invalid).errors == [
	'facility_manifest: project dependency cycle prevents resolution of: TheVoiceOfPlanet, TheAscentToTranscendence',
]);

invalid = content.make_catalog();
content.get_facility(invalid, 'RecyclingTanks').data.mineral_cost = 41;
test.assert(content.validator.validate(invalid).errors == [
	'facilities.RecyclingTanks.mineral_cost: does not match base-game manifest value 40',
]);
