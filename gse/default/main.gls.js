const rules = #include('rules');
const units = #include('units');

let i = 0;
while (i < #size(units)) {
    #game.define_unit(units[i][0], units[i][1]);
    i++;
}

#game.on_start(() => {
    #game.spawn_unit('MINDWORMS', 5, 5);
});
