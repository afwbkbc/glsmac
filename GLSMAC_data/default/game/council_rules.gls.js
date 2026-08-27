const MINIMUM_SESSION_INTERVAL = 20;
const VOTE_PENDING = -2;
const VOTE_ABSTAIN = -1;
const VOTE_NO = 0;
const VOTE_YES = 1;
const MAX_ENERGY_CREDITS = 1000000000;
const UNITY_CORE_ENERGY = 500;
const COUNCIL_SEA_CHANGE = 300;
const MIN_CLIMATE_CHANGE = -3500;
const MAX_CLIMATE_CHANGE = 3500;

const is_policy_proposal = (proposal) => {
	return
		proposal == 'trade_pact' || proposal == 'repeal_trade_pact' ||
		proposal == 'salvage_unity_core' || proposal == 'repeal_un_charter' ||
		proposal == 'reinstate_un_charter' || proposal == 'launch_solar_shade' ||
		proposal == 'melt_polar_caps';
};

const get_proposal_name = (proposal) => {
	if (proposal == 'trade_pact') { return 'Global Trade Pact'; }
	if (proposal == 'repeal_trade_pact') { return 'Repeal Global Trade Pact'; }
	if (proposal == 'salvage_unity_core') { return 'Salvage Unity Fusion Core'; }
	if (proposal == 'repeal_un_charter') { return 'Repeal U.N. Charter'; }
	if (proposal == 'reinstate_un_charter') { return 'Reinstate U.N. Charter'; }
	if (proposal == 'launch_solar_shade') { return 'Launch Solar Shade'; }
	if (proposal == 'melt_polar_caps') { return 'Melt Polar Caps'; }
	if (proposal == 'supreme') { return 'Supreme Leader of Planet'; }
	return 'Planetary Governor';
};

const get_climate_state = (game) => {
	if (#typeof(game.get_tm) != 'Callable') { return null; }
	const tm = game.get_tm();
	if (#typeof(tm.get_climate_state) != 'Callable') { return null; }
	return tm.get_climate_state();
};

const queue_climate_change = (game, amount) => {
	const tm = game.get_tm();
	const previous = tm.get_climate_state();
	tm.set_climate_state(
		previous.level,
		#min(MAX_CLIMATE_CHANGE, #max(MIN_CLIMATE_CHANGE, previous.future_change + amount)),
		previous.progress
	);
	return previous;
};

const restore_climate_state = (game, state) => {
	game.get_tm().set_climate_state(state.level, state.future_change, state.progress);
};

const get_population = (game, player) => {
	let population = 0;
	for (base of game.get_bm().get_bases()) {
		if (base.get_owner().id == player.id) {
			population += base.get_size();
		}
	}
	return population;
};

const owns_project = (game, player, project_id) => {
	const base = game.get_bm().get_project_base(project_id);
	return #is_defined(base) && base != null && base.get_owner().id == player.id;
};

const get_votes = (game, player) => {
	if (player.get_faction().is_progenitor) {
		return 0;
	}
	let votes = get_population(game, player);
	if (owns_project(game, player, 'TheEmpathGuild')) {
		votes += #floor(#to_float(votes) / 2.0);
	}
	if (owns_project(game, player, 'ClinicalImmortality')) {
		votes *= 2;
	}
	if (player.get_faction().id == 'PEACEKEEPERS') {
		votes *= 2;
	}
	return votes;
};

const get_voters = (game) => {
	let voters = [];
	for (player of game.get_players()) {
		if (get_population(game, player) > 0 && !player.get_faction().is_progenitor) {
			voters :+player;
		}
	}
	return voters;
};

const outranks = (left, right) => {
	return right == null || left.votes > right.votes ||
		(left.votes == right.votes && left.player.id < right.player.id);
};

const get_rankings = (game) => {
	let first = null;
	let second = null;
	for (player of get_voters(game)) {
		const entry = {player: player, votes: get_votes(game, player)};
		if (outranks(entry, first)) {
			second = first;
			first = entry;
		} else if (outranks(entry, second)) {
			second = entry;
		}
	}
	let result = [];
	if (first != null) { result :+first; }
	if (second != null) { result :+second; }
	return result;
};

const get_total_votes = (game) => {
	let total = 0;
	for (player of get_voters(game)) {
		total += get_votes(game, player);
	}
	return total;
};

const get_governor = (game) => {
	for (player of get_voters(game)) {
		if (player.get_council_state().is_governor) {
			return player;
		}
	}
	return null;
};

const same_session = (left, right) => {
	return
		left.proposal == right.proposal &&
		left.last_session_turn == right.last_session_turn &&
		left.caller_id == right.caller_id &&
		left.candidate_a_id == right.candidate_a_id &&
		left.candidate_b_id == right.candidate_b_id &&
		left.global_trade_pact == right.global_trade_pact &&
		left.unity_core_salvaged == right.unity_core_salvaged &&
		left.un_charter_repealed == right.un_charter_repealed;
};

const normalize_policy_state = (state) => {
	return {
		global_trade_pact: #is_defined(state.global_trade_pact)
			? state.global_trade_pact : false,
		unity_core_salvaged: #is_defined(state.unity_core_salvaged)
			? state.unity_core_salvaged : false,
		un_charter_repealed: #is_defined(state.un_charter_repealed)
			? state.un_charter_repealed : false,
	};
};

const get_policy_state = (game) => {
	let result = null;
	for (player of game.get_players()) {
		const state = normalize_policy_state(player.get_council_state());
		if (result == null) {
			result = state;
		} else if (
			result.global_trade_pact != state.global_trade_pact ||
			result.unity_core_salvaged != state.unity_core_salvaged ||
			result.un_charter_repealed != state.un_charter_repealed
		) {
			return null;
		}
	}
	return result;
};

const has_global_trade_pact = (game) => {
	const state = get_policy_state(game);
	return state != null && state.global_trade_pact;
};

const has_salvaged_unity_core = (game) => {
	const state = get_policy_state(game);
	return state != null && state.unity_core_salvaged;
};

const is_un_charter_repealed = (game) => {
	const state = get_policy_state(game);
	return state != null && state.un_charter_repealed;
};

const has_active_session = (game) => {
	for (player of game.get_players()) {
		if (player.get_council_state().proposal != '') {
			return true;
		}
	}
	return false;
};

const get_session = (game) => {
	let session = null;
	for (player of game.get_players()) {
		const state = player.get_council_state();
		if (state.proposal != '' && session == null) {
			session = state;
		}
	}
	if (session == null) { return null; }
	for (player of game.get_players()) {
		const state = player.get_council_state();
		if (state.proposal == '' || !same_session(session, state)) {
			return null;
		}
	}
	return session;
};

const get_last_session_turn = (game) => {
	let result = 0;
	for (player of game.get_players()) {
		result = #max(result, player.get_council_state().last_session_turn);
	}
	return result;
};

const is_voter = (game, player) => {
	for (voter of get_voters(game)) {
		if (voter.id == player.id) { return true; }
	}
	return false;
};

const validate_call = (game, player, proposal) => {
	if (game.is_game_over()) {
		return 'Game already has a winner';
	}
	if (
		proposal != 'governor' && proposal != 'supreme' &&
		!is_policy_proposal(proposal)
	) {
		return 'Unsupported Planetary Council proposal';
	}
	if (!is_voter(game, player)) {
		return 'Only a living human faction may convene the Planetary Council';
	}
	if (game.get_turn() <= 0) {
		return 'The Planetary Council cannot convene before the first mission year';
	}
	if (has_active_session(game)) {
		return 'A Planetary Council session is already active';
	}
	if (#sizeof(get_rankings(game)) < 2) {
		return 'At least two eligible factions are required for a Council session';
	}
	const policy_state = get_policy_state(game);
	if (policy_state == null) {
		return 'Planetary Council policy state is inconsistent';
	}
	const last_turn = get_last_session_turn(game);
	if (last_turn > 0 && game.get_turn() - last_turn < MINIMUM_SESSION_INTERVAL) {
		return 'The Planetary Council may only meet once every 20 turns';
	}
	if (proposal == 'supreme') {
		if (!player.has_technology('MindMachineInterface')) {
			return 'Mind/Machine Interface is required to propose Supreme Leader';
		}
		if (get_votes(game, player) * 2 < get_total_votes(game)) {
			return 'A Supreme Leader proposal requires at least half of all Council votes';
		}
	} else if (proposal == 'trade_pact' || proposal == 'repeal_trade_pact') {
		if (!player.has_technology('PlanetaryEconomics')) {
			return 'Planetary Economics is required for a Global Trade Pact proposal';
		}
		const active = policy_state.global_trade_pact;
		if (proposal == 'trade_pact' && active) {
			return 'The Global Trade Pact is already in effect';
		}
		if (proposal == 'repeal_trade_pact' && !active) {
			return 'The Global Trade Pact is not in effect';
		}
	} else if (proposal == 'salvage_unity_core') {
		if (!player.has_technology('OrbitalSpaceflight')) {
			return 'Orbital Spaceflight is required to salvage the Unity Fusion Core';
		}
		if (policy_state.unity_core_salvaged) {
			return 'The Unity Fusion Core has already been salvaged';
		}
	} else if (proposal == 'repeal_un_charter' || proposal == 'reinstate_un_charter') {
		if (!player.has_technology('AdvancedMilitaryAlgorithms')) {
			return 'Advanced Military Algorithms is required for a U.N. Charter proposal';
		}
		if (proposal == 'repeal_un_charter' && policy_state.un_charter_repealed) {
			return 'The U.N. Charter is already repealed';
		}
		if (proposal == 'reinstate_un_charter' && !policy_state.un_charter_repealed) {
			return 'The U.N. Charter is already in effect';
		}
	} else if (proposal == 'launch_solar_shade' || proposal == 'melt_polar_caps') {
		const required_technology = proposal == 'launch_solar_shade'
			? 'OrbitalSpaceflight'
			: 'AdvancedEcologicalEngineering';
		if (!player.has_technology(required_technology)) {
			return proposal == 'launch_solar_shade'
				? 'Orbital Spaceflight is required to launch a Solar Shade'
				: 'Advanced Ecological Engineering is required to melt the polar caps';
		}
		const climate = get_climate_state(game);
		if (climate == null) {
			return 'Planetary climate controls are unavailable';
		}
		if (proposal == 'launch_solar_shade' && climate.future_change <= MIN_CLIMATE_CHANGE) {
			return 'The Solar Shade is already operating at maximum effect';
		}
		if (proposal == 'melt_polar_caps' && climate.future_change >= MAX_CLIMATE_CHANGE) {
			return 'The polar caps cannot be destabilized further';
		}
	}
};

const get_available_policy_proposals = (game, player) => {
	const state = get_policy_state(game);
	if (state == null) { return []; }
	let candidates = ['salvage_unity_core'];
	candidates :+(state.global_trade_pact ? 'repeal_trade_pact' : 'trade_pact');
	candidates :+(state.un_charter_repealed ? 'reinstate_un_charter' : 'repeal_un_charter');
	candidates :+'launch_solar_shade';
	candidates :+'melt_polar_caps';
	let result = [];
	for (proposal of candidates) {
		if (!#is_defined(validate_call(game, player, proposal))) {
			result :+proposal;
		}
	}
	return result;
};

const validate_vote = (game, player, vote_id) => {
	if (game.is_game_over()) {
		return 'Game already has a winner';
	}
	const session = get_session(game);
	if (session == null) {
		return has_active_session(game)
			? 'Planetary Council session state is inconsistent'
			: 'No Planetary Council vote is active';
	}
	if (!is_voter(game, player)) {
		return 'Faction is not eligible to vote in the Planetary Council';
	}
	const state = player.get_council_state();
	if (state.vote_id != VOTE_PENDING) {
		return 'Faction has already cast its Planetary Council vote';
	}
	if (is_policy_proposal(session.proposal)) {
		if (vote_id != VOTE_ABSTAIN && vote_id != VOTE_YES && vote_id != VOTE_NO) {
			return 'Planetary Council policy vote must be Yes, No, or Abstain';
		}
	} else if (
		vote_id != VOTE_ABSTAIN && vote_id != session.candidate_a_id &&
		vote_id != session.candidate_b_id
	) {
		return 'Planetary Council vote must select a candidate or abstain';
	}
};

const get_required_votes = (proposal, total) => {
	if (proposal == 'supreme') {
		return #ceil(#to_float(total * 3) / 4.0);
	}
	return #floor(#to_float(total) / 2.0) + 1;
};

const get_tally = (game) => {
	const session = get_session(game);
	if (session == null) { return null; }
	let result = {
		proposal: session.proposal,
		candidate_a_id: session.candidate_a_id,
		candidate_b_id: session.candidate_b_id,
		candidate_a_votes: 0,
		candidate_b_votes: 0,
		total_votes: 0,
		required_votes: 0,
		all_voted: true,
	};
	for (player of get_voters(game)) {
		const votes = get_votes(game, player);
		result.total_votes = result.total_votes + votes;
		const vote_id = player.get_council_state().vote_id;
		if (vote_id == VOTE_PENDING) {
			result.all_voted = false;
		} else if (vote_id == session.candidate_a_id) {
			result.candidate_a_votes = result.candidate_a_votes + votes;
		} else if (vote_id == session.candidate_b_id) {
			result.candidate_b_votes = result.candidate_b_votes + votes;
		}
	}
	result.required_votes = get_required_votes(session.proposal, result.total_votes);
	return result;
};

const get_result = (game) => {
	const tally = get_tally(game);
	if (tally == null || !tally.all_voted) { return null; }
	let winner_id = -1;
	let winner_votes = 0;
	if (tally.candidate_a_votes > tally.candidate_b_votes) {
		winner_id = tally.candidate_a_id;
		winner_votes = tally.candidate_a_votes;
	} else if (tally.candidate_b_votes > tally.candidate_a_votes) {
		winner_id = tally.candidate_b_id;
		winner_votes = tally.candidate_b_votes;
	}
	return {
		proposal: tally.proposal,
		winner_id: winner_votes >= tally.required_votes ? winner_id : -1,
		winner_votes: winner_votes,
		required_votes: tally.required_votes,
		total_votes: tally.total_votes,
	};
};

const snapshot_states = (game) => {
	let result = [];
	for (player of game.get_players()) {
		result :+{player: player, state: player.get_council_state()};
	}
	return result;
};

const restore_states = (snapshot) => {
	for (entry of snapshot) {
		entry.player.set_council_state(entry.state);
	}
};

const snapshot_energy = (game) => {
	let result = [];
	for (player of game.get_players()) {
		result :+{player: player, energy_credits: player.get_energy_credits()};
	}
	return result;
};

const restore_energy = (snapshot) => {
	for (entry of snapshot) {
		entry.player.set_energy_credits(entry.energy_credits);
	}
};

const award_unity_core_energy = (game) => {
	for (player of game.get_players()) {
		player.set_energy_credits(#min(
			MAX_ENERGY_CREDITS,
			player.get_energy_credits() + UNITY_CORE_ENERGY
		));
	}
};

return {
	minimum_session_interval: MINIMUM_SESSION_INTERVAL,
	vote_pending: VOTE_PENDING,
	vote_abstain: VOTE_ABSTAIN,
	vote_no: VOTE_NO,
	vote_yes: VOTE_YES,
	unity_core_energy: UNITY_CORE_ENERGY,
	council_sea_change: COUNCIL_SEA_CHANGE,
	is_policy_proposal: is_policy_proposal,
	get_proposal_name: get_proposal_name,
	get_population: get_population,
	get_votes: get_votes,
	get_voters: get_voters,
	get_rankings: get_rankings,
	get_total_votes: get_total_votes,
	get_governor: get_governor,
	get_policy_state: get_policy_state,
	has_global_trade_pact: has_global_trade_pact,
	has_salvaged_unity_core: has_salvaged_unity_core,
	is_un_charter_repealed: is_un_charter_repealed,
	has_active_session: has_active_session,
	get_session: get_session,
	get_last_session_turn: get_last_session_turn,
	validate_call: validate_call,
	get_available_policy_proposals: get_available_policy_proposals,
	validate_vote: validate_vote,
	get_required_votes: get_required_votes,
	get_tally: get_tally,
	get_result: get_result,
	snapshot_states: snapshot_states,
	restore_states: restore_states,
	snapshot_energy: snapshot_energy,
	restore_energy: restore_energy,
	award_unity_core_energy: award_unity_core_energy,
	get_climate_state: get_climate_state,
	queue_climate_change: queue_climate_change,
	restore_climate_state: restore_climate_state,
};
