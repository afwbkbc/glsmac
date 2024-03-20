const movement = #include('units/movement');
const defs = #include('units/defs');

return {

    init: () => {
        movement.init();
    },

    define: () => {
        defs.define();
    },

};
