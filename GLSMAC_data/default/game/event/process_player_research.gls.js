return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to process research';
		}
		if (#typeof(e.data.technology) != 'Object') {
			return 'Research technology must be an object';
		}
		if (#typeof(e.data.labs) != 'Int' || e.data.labs < 0) {
			return 'Research labs must be a non-negative whole number';
		}
		if (#typeof(e.data.technology.id) != 'String' || e.data.technology.id == '') {
			return 'Technology ID must be a non-empty string';
		}
		if (#typeof(e.data.technology.name) != 'String' || e.data.technology.name == '') {
			return 'Technology name must be a non-empty string';
		}
		if (#typeof(e.data.technology.cost) != 'Int' || e.data.technology.cost <= 0) {
			return 'Technology cost must be a positive whole number';
		}
		const state = e.data.player.get_research_state();
		if (state.target != e.data.technology.id) {
			return 'Technology is not the player\'s current research target';
		}
		if (state.progress >= e.data.technology.cost) {
			return 'Research progress must remain below the technology cost';
		}
	},

	apply: (e) => {
		const previous = e.data.player.get_research_state();
		let technologies = [];
		for (id of previous.technologies) {
			technologies :+id;
		}
		let target = previous.target;
		let progress = previous.progress + e.data.labs;
		let technology = e.data.technology;
		let completed_names = [];
		while (target != '' && progress >= technology.cost) {
			progress -= technology.cost;
			technologies :+technology.id;
			completed_names :+technology.name;
			target = e.game.get('f_technology_get_next_target')(technologies, e.data.player);
			if (target != '') {
				technology = e.game.get('f_technology_get_definition')(target);
				if (technology == null) {
					throw Error('Unknown research target: ' + target);
				}
			}
		}
		if (target == '') {
			progress = 0;
		}
		e.data.player.set_research_state({
			technologies: technologies,
			target: target,
			progress: progress,
		});
		e.game.trigger('research_updated', {
			player: e.data.player,
		});
		for (name of completed_names) {
			e.game.message(
				e.data.player.name + ' has discovered ' + name + '.'
			);
		}
		if (#sizeof(completed_names) > 0) {
			const queue_datalinks = e.game.get('f_project_queue_planetary_datalinks');
			if (#is_defined(queue_datalinks)) {
				queue_datalinks();
			}
		}
		return {
			state: previous,
			completed: #sizeof(completed_names) > 0,
			completed_count: #sizeof(completed_names),
		};
	},

	rollback: (e) => {
		e.data.player.set_research_state(e.applied.state);
		e.game.trigger('research_updated', {
			player: e.data.player,
		});
	},

};
