# GSE vs JS mindset — cheatsheet

`.gls.js` is GSE (GLSMAC's embedded script language) — JS-*shaped* syntax, not JS semantics.
(This document was LLM-generated and isn't final; the actual CPP code is.)


| | JS | GSE (`.gls.js`) |
|---|---|---|
| Keywords | ~40 reserved words | Only: `return break continue throw let const true false null undefined if else while for try catch switch case default`. No `var`, `function`, `class`, `new`, `import`, `export`, `async`, `await`. (`src/gse/parser/JS.h:98-148`) |
| Functions | `function`/arrow both | Arrow only: `let f = (a, b) => { ... };`. No hoisting — declare before use. |
| Modules | `import`/`export` | `#include('path')` returns whatever that file's top-level `return` produced; call the result yourself. (`GLSMAC_data/default/main.gls.js:1-4`) |
| `+` coercion | Anything stringifies (`1+'2'==='12'`) | Strict — both sides must match type (int+int, float+float, string+string). Array `+` **concatenates**; object `+` **merges** (throws on duplicate key). Anything else throws `operation_not_supported`. (`src/gse/Interpreter.cpp:522-543`) |
| Truthy/falsy | `0`, `''`, `null`, `[]` (no) all coerce | **None.** `if`/`while`/ternary require an actual `Bool` — `if (arr)` or `if (str)` throws `TYPE_ERROR`. Always compare explicitly. (`src/gse/Interpreter.cpp:1076-1077`) |
| `this` | dynamic, many footguns | Context-bound inside callables, same idea as JS. Nothing special found here — don't over-solve it. (`src/gse/context/Context.cpp:13,48`) |
| async | promises / event loop / microtasks | No promises. `#async(ms, callback)` = repeating timer: callback returning `true` reschedules after `ms`, `false` stops. Nested `#async` calls create child scopes tied to the parent's lifetime (cooperative, not microtask-queued) — see the parent/child example at `GLSMAC_data/tests/async.gls.js:33-73`. Different mental model, don't reach for `await`-shaped intuitions. |
| Array push | `.push(x)` | `arr :+ x` or `arr[] = x`. No method-call form seen. |
| `null`/`undefined` | both exist, both loose | Both exist as distinct types here too — not a JS difference, confirmed same shape (`src/gse/value/Null.h`, `src/gse/value/Undefined.h`, `#typeof()` returns the type name string). |
| `#` prefix | private class fields only | Namespace marker for **engine builtins** (`#main`, `#include`, `#typeof`, `#sizeof`, `#to_string`, `#is_defined`, `#uppercase`, ...) — every `Context::CreateBuiltin` call auto-prepends it. Nothing to do with privacy. |
| Testing | describe/it frameworks | `test.assert(cond)` — only exists inside the GSE test harness (`GLSMAC_data/tests/`, run via `--gse-tests`), not available in real game/`default/` scripts. |


## Not yet confirmed

Other builtin-side array/collection operators exist beyond push
(`OT_POP`, `OT_ERASE`, `OT_RANGE`, `OT_AT`, `OT_CHILD` —
`src/gse/program/Types.h:42-47`) but their surface syntax wasn't traced
against real usage — only `:+` / `[]=` push are confirmed
(`src/gse/parser/JS.cpp:912`, `GLSMAC_data/tests/async.gls.js`). Grep
`GLSMAC_data/tests/*.gls.js` for real usage before relying on the rest.
