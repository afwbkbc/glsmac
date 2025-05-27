return {

	validate: (e) => {
		if (e.caller != 0) {
			return 'Only master is allowed to configure game';
		}
	},

	apply: (e) => {
		#print('CONFIGURE_GAME', e);
	},

	rollback: (e) => {
		#print('CONFIGURE_GAME ROLLBACK', e);
	},

};
