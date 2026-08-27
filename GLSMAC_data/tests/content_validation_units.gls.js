const content = #include('./_content_validation_common');

let invalid = content.make_catalog();
invalid.units[0].data.morale = 'MISSING';
test.assert(content.validator.validate(invalid).errors == [
	'units.ScoutPatrol.morale: references missing morale set MISSING',
]);

invalid = content.make_catalog();
invalid.units[0].data.movement_per_turn = 1.5;
test.assert(content.validator.validate(invalid).errors == [
	'units.ScoutPatrol.movement_per_turn: must be an integer from 0 through 1000',
]);

invalid = content.make_catalog();
let invalid_air_unit = content.get_unit_by_chassis(invalid, 'Needlejet');
invalid_air_unit.data.operational_range = 1;
test.assert(content.validator.validate(invalid).errors == [
	'units.' + invalid_air_unit.id + '.operational_range: does not match chassis range 2',
]);

invalid = content.make_catalog();
invalid_air_unit = content.get_unit_by_chassis(invalid, 'Needlejet');
invalid_air_unit.data.is_missile = true;
test.assert(content.validator.validate(invalid).errors == [
	'units.' + invalid_air_unit.id + '.is_missile: does not match chassis missile flag',
]);

invalid = content.make_catalog();
invalid.units[0].data.required_technology = 'MissingTechnology';
test.assert(content.validator.validate(invalid).errors == [
	'units.ScoutPatrol.required_technology: references missing technology MissingTechnology',
]);

invalid = content.make_catalog();
invalid.units[0].data.weapon = 'MissingWeapon';
test.assert(content.validator.validate(invalid).errors == [
	'units.ScoutPatrol.weapon: references missing weapon MissingWeapon',
]);

invalid = content.make_catalog();
invalid.units[0].data.abilities = ['HeavyArtillery', 'HeavyArtillery'];
test.assert(content.validator.validate(invalid).errors == [
	'units.ScoutPatrol.abilities[1]: duplicates unit ability HeavyArtillery',
]);
