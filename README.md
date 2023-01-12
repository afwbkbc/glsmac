# GLSMAC
Open-source OpenGL/SDL client for Sid Meier's Alpha Centauri ( + Alien Crossfire )

Note: you will still need to own original game and expansion - this client uses it's assets.

GLSMAC discord : https://discord.gg/fWG3xK7cUx

- Why?

SMAC was a great game, possibly the best in 4x genre, but it's outdated so much it barely runs on modern systems. Especially multiplayer (which is based on ancient directplay).

- What will be same between GLSMAC and original game?

Models, images, movies, texts, lore, factions, game rules (such as tech trees, types of armor/weapons etc), UI

- What will be completely different?

Graphics engine, network code are fully rewritten to allow it run as smoothly as possible.

- What may differ slightly?

Random maps generation, AI, dynamic music, some UI parts. While I'll try to get them as close as possible to original logic, they will still be recreated from scratch.

- Compatibility?

Network won't be compatible to original client (so you can't play together with somebody running it).

Maybe I'll make saves and maps compatible, but it's very low priority.

- Other improvements?

Once I replicate original game, I'll focus on other things, such as: larger maps, larger maximum number of players, custom mod support, online services such as game browser or hosting long-turns games (where people can login and make 1 turn per day or so), alternative art packs (used instead of original, this may mean better graphics)

- Build instructions

You will need following libraries (-dev versions): FreeType2, SDL2, SDL2_image, GL, GLEW

Target platform is GNU/Linux with GCC compiler, it should also be possible to build it with MinGW (if you're on Windows). Other compilers/platforms weren't tested but you can try.

For release build: cmake . -DCMAKE_BUILD_TYPE=Release && make

For debug build: cmake -DCMAKE_BUILD_TYPE=Debug . && make

For same build as before (or Release if it's first build): cmake . && make

Add -j parameter to make to speed up if you have multiple cores, i.e. "make -j9" for 8

- Launch

Just run ./bin/GLSMAC <smac_directory>

- Reporting problems

If you encountered problem, first thing to try is to update to newest version, maybe it was already fixed.

If it fails to build: create issue with errors from build log

If it crashes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, type "r", make it crash, then type "bt" and create issue with this backtrace and steps to reproduce

If it freezes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, get to where it freezes, then press Ctrl+C, type "bt" and create issue with backtrace and steps to reproduce

If it behaves incorrectly: create issue and specify: 1) steps to reproduce 2) expected behavior 3) actual behavior ; attach screenshots if needed
