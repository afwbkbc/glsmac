# CLAUDE.md

## `.gls.js` is not JavaScript

Files matching `*.gls.js` (mostly under `GLSMAC_data/default/`) are GSE
(GLSMAC's embedded script language) — JS-*shaped* syntax, JS-incompatible
semantics. Do not pattern-match JS habits onto them.

Before editing or generating `.gls.js`, read `tools/js-vs-glsjs-cheatsheet.md`
for the mental-model diff. Grammar source of truth: `src/gse/parser/JS.h`.

Test loop: `./build/bin/GLSMAC --gse-tests --gse-tests-script PATH.gls.js --quiet`
(no script arg = full suite).
