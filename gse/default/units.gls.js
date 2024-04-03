const defs = #include('units/defs');
const movement = #include('units/movement');
const combat = #include('units/combat');
const turns = #include('units/turns');

const result = {

    init: () => {
        movement.init();
        combat.init();
        turns.init();
    },

    define: () => {
        defs.define();
        combat.define();
    },

};

return result;
