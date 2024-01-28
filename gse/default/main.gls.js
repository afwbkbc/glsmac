game.on_turn((turn) => {
    console.log('TURN');
    console.log(turn);
    game.message('Hello, world!');
});

let player = game.get_player();
console.log('PLAYER');
console.log(player);
console.dump(player);

//game.exit();
