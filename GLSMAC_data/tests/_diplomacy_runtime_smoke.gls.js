#main((glsmac) => {

	#include('../default/game/game')(glsmac);
	#include('../default/ui/ui')(glsmac);

	let finished = false;
	const fail = (message) => {
		if (!finished) {
			finished = true;
			#print('DIPLOMACY_RUNTIME_FAIL: ' + message);
			glsmac.exit();
		}
	};
	const wait_for = (condition, failure, done) => {
		let ticks = 0;
		#async(25, () => {
			ticks++;
			if (condition()) {
				done();
				return false;
			}
			if (ticks >= 200) {
				fail(failure);
				return false;
			}
			return true;
		});
	};

	glsmac.on('configure_game', (e) => {
		const game = e.game;
		game.register_event('diplomacy_runtime_seed_energy', {
			validate: (e) => {
				if (e.caller != 0 && e.caller != e.data.player.id) {
					return 'Only the runtime test player may seed loan balances';
				}
			},
			apply: (e) => {
				const snapshot = {
					player_energy: e.data.player.energy_credits,
					target_energy: e.data.target.energy_credits,
				};
				e.data.player.set_energy_credits(200);
				e.data.target.set_energy_credits(20);
				e.game.trigger('economy_updated', {player: e.data.player});
				e.game.trigger('economy_updated', {player: e.data.target});
				return snapshot;
			},
			rollback: (e) => {
				e.data.player.set_energy_credits(e.applied.player_energy);
				e.data.target.set_energy_credits(e.applied.target_energy);
				e.game.trigger('economy_updated', {player: e.data.player});
				e.game.trigger('economy_updated', {player: e.data.target});
			},
		});
		game.register_event('diplomacy_runtime_process_loan', {
			validate: (e) => {
				if (e.caller != 0 && e.caller != e.data.player.id) {
					return 'Only the runtime test player may request loan processing';
				}
			},
			apply: (e) => {
				if (e.game.is_master()) {
					e.game.event('process_diplomatic_loan_payment', {
						borrower: e.data.borrower,
						lender: e.data.lender,
					});
				}
			},
			rollback: (e) => {},
		});
		game.on('start_ui', (e) => {
			let player = game.get_player();
			const get_owned_base = (owner) => {
				for (base of game.get_bm().get_bases()) {
					if (base.get_owner().id == owner.id) {
						return base;
					}
				}
				return null;
			};
			let other = null;
			for (candidate of game.get_players()) {
				if (candidate.id != player.id) {
					other = candidate;
					break;
				}
			}
			if (other == null) {
				fail('quickstart did not create an opponent');
				return;
			}
			if (
				player.get_diplomatic_relation(other) != 'neutral' ||
				player.get_diplomatic_offer(other) != '' ||
				player.get_diplomatic_trade(other) != null ||
				player.get_diplomatic_loan_offer(other) != null ||
				player.get_diplomatic_loan(other) != null ||
				player.get_integrity_blemishes() != 0 ||
				other.get_integrity_blemishes() != 0
			) {
				fail('initial diplomatic state is invalid');
				return;
			}
			const exercise_vendetta = () => {
				game.event('declare_vendetta', {player: player, target: other});
				wait_for(
					() => {
						return (
							player.get_diplomatic_relation(other) == 'vendetta' &&
							other.get_diplomatic_relation(player) == 'vendetta' &&
							player.get_integrity_blemishes() == 1 &&
							other.get_integrity_blemishes() == 0
						);
					},
					'vendetta did not become bilateral',
					() => {
						const ended_commerce = game.get('f_economy_get_base_commerce')(
							game,
							get_owned_base(player)
						);
						const debt = other.get_diplomatic_loan(player);
						if (
							ended_commerce.total != 0 ||
							#sizeof(ended_commerce.partners) != 0 ||
							debt == null || debt.balance != 114
						) {
							fail('vendetta did not end commerce while preserving debt');
							return;
						}
						game.event('diplomacy_runtime_process_loan', {
							player: player,
							borrower: other,
							lender: player,
						});
						wait_for(
							() => {
								const wartime_debt = other.get_diplomatic_loan(player);
								return wartime_debt != null && wartime_debt.balance == 120;
							},
							'wartime missed payment did not increase loan balance',
							() => {
								finished = true;
								#print(
									'DIPLOMACY_RUNTIME_PASS: treaty commerce, reciprocal trade, loan repayment, betrayal integrity, and vendetta debt'
								);
								glsmac.exit();
							}
						);
					}
				);
			};
			const exercise_loan = () => {
				game.event('diplomacy_runtime_seed_energy', {
					player: player,
					target: other,
				});
				wait_for(
					() => {
						return (
							game.get_player(player.id).energy_credits == 200 &&
							game.get_player(other.id).energy_credits == 20
						);
					},
					'could not seed deterministic loan balances',
					() => {
						player = game.get_player(player.id);
						other = game.get_player(other.id);
						game.event('propose_diplomatic_loan', {
							player: player,
							target: other,
							terms: {
								proposer_is_lender: true,
								principal: 100,
								payment: 6,
								turns: 20,
							},
						});
						wait_for(
							() => { return other.get_diplomatic_loan_offer(player) != null; },
							'loan proposal was not stored',
							() => {
								game.event_as(other.id, 'respond_diplomatic_loan', {
									player: other,
									proposer: player,
									accept: true,
								});
								wait_for(
									() => {
										const current_player = game.get_player(player.id);
										const current_other = game.get_player(other.id);
										const debt = current_other.get_diplomatic_loan(current_player);
										return (
											debt != null && debt.balance == 120 && debt.payment == 6 &&
											current_player.energy_credits == 100 &&
											current_other.energy_credits == 120
										);
									},
									'accepted loan did not transfer principal and create debt',
									() => {
										player = game.get_player(player.id);
										other = game.get_player(other.id);
										game.event('diplomacy_runtime_process_loan', {
											player: player,
											borrower: other,
											lender: player,
										});
										wait_for(
											() => {
												const current_player = game.get_player(player.id);
												const current_other = game.get_player(other.id);
												const debt = current_other.get_diplomatic_loan(current_player);
												return (
													debt != null && debt.balance == 114 &&
													current_player.energy_credits == 106 &&
													current_other.energy_credits == 114
												);
											},
											'peaceful loan payment did not transfer and reduce debt',
											() => {
												player = game.get_player(player.id);
												other = game.get_player(other.id);
												exercise_vendetta();
											}
										);
									}
								);
							}
						);
					}
				);
			};

			game.event('propose_diplomatic_relation', {
				player: player,
				target: other,
				relation: 'treaty',
			});
			wait_for(
				() => { return other.get_diplomatic_offer(player) == 'treaty'; },
				'treaty proposal was not stored',
				() => {
					game.event_as(other.id, 'respond_diplomatic_proposal', {
						player: other,
						proposer: player,
						accept: true,
					});
					wait_for(
						() => {
							return (
								player.get_diplomatic_relation(other) == 'treaty' &&
								other.get_diplomatic_relation(player) == 'treaty'
							);
						},
						'accepted treaty did not become bilateral',
						() => {
							const commerce_base = get_owned_base(player);
							const commerce = commerce_base == null
								? null
								: game.get('f_economy_get_base_commerce')(game, commerce_base);
							if (
								commerce == null || #sizeof(commerce.partners) != 1 ||
								commerce.partners[0].player_id != other.id ||
								commerce.partners[0].relation != 'treaty'
							) {
								fail('accepted treaty did not establish base commerce');
								return;
							}
							let offered_technology = '';
							for (id of player.get_research_state().technologies) {
								if (!other.has_technology(id)) {
									offered_technology = id;
									break;
								}
							}
							let requested_technology = '';
							for (id of other.get_research_state().technologies) {
								if (!player.has_technology(id)) {
									requested_technology = id;
									break;
								}
							}
							if (offered_technology == '' || requested_technology == '') {
								fail('players did not start with tradeable technologies');
								return;
							}
							game.event('propose_diplomatic_trade', {
								player: player,
								target: other,
								terms: {
									offer_energy: 0,
									offer_technology: offered_technology,
									request_energy: 0,
									request_technology: requested_technology,
								},
							});
							wait_for(
								() => { return other.get_diplomatic_trade(player) != null; },
								'trade proposal was not stored',
								() => {
									game.event_as(other.id, 'respond_diplomatic_trade', {
										player: other,
										proposer: player,
										accept: true,
									});
									wait_for(
										() => {
											return (
												other.get_diplomatic_trade(player) == null &&
												player.has_technology(requested_technology) &&
												other.has_technology(offered_technology)
											);
										},
										'accepted trade did not transfer its technologies',
									() => { exercise_loan(); }
									);
								}
							);
						}
					);
				}
			);
		});
	});

	glsmac.run();

});
