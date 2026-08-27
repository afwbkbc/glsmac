const popup = #include('../default/ui/parts/game/popup/probe_operations');
const unit_upgrade = #include('../default/ui/parts/game/popup/unit_upgrade');
const preview = #include('../default/ui/parts/game/bottom_bar/object_preview');

test.assert(#typeof(popup.init) == 'Callable');
test.assert(#typeof(popup.set) == 'Callable');
test.assert(#typeof(unit_upgrade.init) == 'Callable');
test.assert(#typeof(unit_upgrade.set) == 'Callable');
test.assert(#typeof(preview.init) == 'Callable');
