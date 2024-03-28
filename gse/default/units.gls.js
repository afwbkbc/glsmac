const defs = #include('units/defs');
const movement = #include('units/movement');
const turns = #include('units/turns');

return {

    init: () => {
        movement.init();
        turns.init();
    },

    define: () => {
        defs.define();
    },

};
