const defs = #include('units/defs');
const movement = #include('units/movement');
const turns = #include('units/turns');

const result = {

    init: () => {
        movement.init();
        turns.init();
    },

    define: () => {
        defs.define();
    },

};

return result;
