const preview = #include('../default/ui/parts/game/bottom_bar/tile_preview');

test.assert(preview.get_terraforming_name('forest') == 'Forest');
test.assert(preview.get_terraforming_name('farm') == 'Farm');
test.assert(preview.get_terraforming_name('mine') == 'Mine');
test.assert(preview.get_terraforming_name('solar') == 'Solar Collector');
test.assert(preview.get_terraforming_name('road') == 'Road');
test.assert(!#is_defined(preview.get_terraforming_name('unknown')));
