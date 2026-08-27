const factions = #include('../default/factions');

let configured = {};
let configured_count = 0;
const fm = {
	add: (id, data) => {
		configured[id] = data;
		configured_count++;
	},
	import_colors: (file) => { return file; },
	import_base_names: (file) => { return [file]; },
};

factions.configure(fm);

test.assert(configured_count == 15);
test.assert(configured.GAIANS.starting_technologies == ['CentauriEcology']);
test.assert(configured.HIVE.starting_technologies == ['DoctrineLoyalty']);
test.assert(configured.UNIVERSITY.starting_technologies == ['InformationNetworks']);
test.assert(configured.MORGANITES.starting_technologies == ['IndustrialBase']);
test.assert(configured.SPARTANS.starting_technologies == ['DoctrineMobility']);
test.assert(configured.BELIEVERS.starting_technologies == ['SocialPsych']);
test.assert(configured.PEACEKEEPERS.starting_technologies == ['Biogenetics']);
test.assert(configured.CONSCIOUSNESS.starting_technologies == ['AppliedPhysics', 'InformationNetworks']);
test.assert(configured.PIRATES.starting_technologies == ['DoctrineMobility']);
test.assert(configured.PIRATES.is_naval);
test.assert(configured.DRONES.starting_technologies == ['IndustrialBase']);
test.assert(configured.ANGELS.starting_technologies == ['InformationNetworks', 'PlanetaryNetworks']);
test.assert(configured.PLANETCULT.starting_technologies == ['CentauriEcology', 'SocialPsych']);
test.assert(configured.CARETAKERS.starting_technologies == ['Biogenetics', 'CentauriEcology', 'InformationNetworks']);
test.assert(configured.CARETAKERS.is_progenitor);
test.assert(configured.USURPERS.starting_technologies == ['AppliedPhysics', 'Biogenetics', 'CentauriEcology']);
test.assert(configured.USURPERS.is_progenitor);
