const defs = #include('units/defs');
const movement = #include('units/movement');
const attack = #include('units/attack');
const turns = #include('units/turns');

const result = {

    init: () => {
        movement.init();
        attack.init();
        turns.init();
    },

    define: () => {
        defs.define();
    },

};

return result;
