const fungal_bloom = #include('../default/game/event/fungal_bloom');

let ecological_damage_events = 2;
const owner = {
	get_ecological_damage_events: () => { return ecological_damage_events; },
	set_ecological_damage_events: (value) => { ecological_damage_events = value; },
};
const terraforming = {
	road: true,
	mag_tube: true,
	forest: true,
	farm: true,
	soil_enricher: true,
	mine: true,
	solar: true,
	condenser: true,
	mirror: true,
	borehole: true,
};
const features = {monolith: false, xenofungus: false};
let tile = null;
tile = {
	terraforming: terraforming,
	features: features,
	get_base: () => { return null; },
	update_terraforming: (changes) => {
		for (id in changes) {
			terraforming[id] = changes[id];
		}
	},
	update_features: (changes) => {
		for (id in changes) {
			features[id] = changes[id];
		}
	},
};
const base = {
	name: 'Gaia\'s Landing',
	get_owner: () => { return owner; },
	get_workable_tiles: () => { return [tile]; },
};
let messages = [];
let triggers = [];
const event = {
	caller: 0,
	data: {base: base, tile: tile, damage: 17},
	game: {
		trigger: (name, data) => { triggers :+name; },
		message: (message) => { messages :+message; },
	},
};

test.assert(!#is_defined(fungal_bloom.validate(event)));
event.caller = 1;
test.assert(#is_defined(fungal_bloom.validate(event)));
event.caller = 0;
event.data.damage = 1.5;
test.assert(#is_defined(fungal_bloom.validate(event)));
event.data.damage = 17;
event.data.tile = {
	get_base: () => { return null; },
	update_features: (changes) => {},
	update_terraforming: (changes) => {},
};
test.assert(#is_defined(fungal_bloom.validate(event)));
event.data.tile = tile;

event.applied = fungal_bloom.apply(event);
test.assert(features.xenofungus);
for (id of ['forest', 'farm', 'soil_enricher', 'mine', 'solar', 'condenser', 'mirror', 'borehole']) {
	test.assert(!terraforming[id]);
}
test.assert(terraforming.road);
test.assert(terraforming.mag_tube);
test.assert(ecological_damage_events == 3);
test.assert(#sizeof(messages) == 1);
test.assert(#sizeof(triggers) == 2);

fungal_bloom.rollback(event);
test.assert(!features.xenofungus);
for (id of ['forest', 'farm', 'soil_enricher', 'mine', 'solar', 'condenser', 'mirror', 'borehole']) {
	test.assert(terraforming[id]);
}
test.assert(ecological_damage_events == 2);
