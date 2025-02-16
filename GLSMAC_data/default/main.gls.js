#main((glsmac) => {

    // backwards compatibility
    if (#classof(glsmac) != 'GLSMAC') {
        (#include('main_legacy'))(glsmac);
        return;
    }

    #include('game/game')(glsmac);
    #include('ui/ui')(glsmac);

    glsmac.run();

});
