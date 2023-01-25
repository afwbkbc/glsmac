# GLSMAC
Open-source OpenGL/SDL2 client for Sid Meier's Alpha Centauri ( + Alien Crossfire )

Note: you will still need to own original game and expansion - this client uses it's assets.

GLSMAC discord : https://discord.gg/fWG3xK7cUx

### Why?

SMAC was a great game, possibly the best in 4x genre, but it's outdated so much it barely runs on modern systems. Especially multiplayer (which is based on ancient directplay).

### What will be same between GLSMAC and original game?

Models, images, movies, texts, lore, factions, game rules (such as tech trees, types of armor/weapons etc), UI

### What will be completely different?

Graphics engine, network code are fully rewritten to allow it run as smoothly as possible.

### What may differ slightly?

Random maps generation, AI, dynamic music, some UI parts. While I'll try to get them as close as possible to original logic, they will still be recreated from scratch.

### Compatibility?

Network won't be compatible to original client (so you can't play together with somebody running it).

Maybe I'll make saves and maps compatible, but it's very low priority.

### Other improvements?

Once I replicate original game, I'll focus on other things, such as: larger maps, larger maximum number of players, custom mod support, online services such as game browser or hosting long-turns games (where people can login and make 1 turn per day or so), alternative art packs (used instead of original, this may mean better graphics)

### Roadmap

##### Versions 0.x: replicating original SMAC gameplay as closely as possible (but without bugs)

- 0.1: fundamental engine functions, such as: rendering, UI, netcode, sound system

- 0.2: basic map functions, such as: tiles, mapgen, creating/saving/loading games, map rendering

- 0.3: basic multiplayer: ability to host and join games, lobby, save/load games with same players. multiplayer support will be maintained during further steps from now on

- 0.4: game processing, such as: game turns, units, buildings, technologies, building of bases, terraforming, basic combat, victory by conquest

- 0.5: all remaining SMAC units, buildings, terraformings, tech, etc

- 0.6: all remaining victory types

- 0.7: AI (computer opponents, cities automation, units automation, etc) and diplomacy

- 0.8: alien lifeforms, ecology

- 0.9: all remaining in-game UI, including game settings

- 0.10: non-crucial things like movies, interludes

- 0.11: final polishing and bugfixing

##### Versions 1.x: fixing bugs, adding optional small features that weren't present in original SMAC but won't change it's gameplay drastically. See 'GLSMAC 1.x features' section

##### Versions 2.x: adding all kinds of features, as long as they are worth the time. See 'GLSMAC 2.x features' section

##### Versions 3.x: not sure yet, probably something cool

### Build instructions

Target platform is GNU/Linux with GCC compiler, it should also be possible to build it with MinGW (if you're on Windows). Other compilers/platforms weren't tested but you can try.

You will need following libraries (-dev versions): FreeType2, SDL2, SDL_image, GL, GLU, GLEW

Ubuntu/Debian/Mint: sudo apt-get install libfreetype-dev libsdl2-dev libsdl2-image-dev libglu-dev libglew-dev

Gentoo: sudo emerge libsdl2 sdl2-image freetype glu glew

Building on Windows with MinGW is possible, but tricky. You need to install all those libraries manually, and also FreeGLUT. Double-check that you download libraries compiled for MinGW (and not MSVC). GLEW may need to be compiled from source (because they only ship MSVC version). Make sure to have threads-enabled version of MinGW. MSYS and TDM-GCC are recommended for more consistent development environment. Further MinGW configuration instructions are beyond the scope of this project, but there are plenty of guides on internet.

Building on Windows with MSVC is not supported and will probably require many changes to code to be compatible.

It is recommended to build project using cmake and make instead of adding .cpp and .h files manually to IDE.

For release build: cmake . -DCMAKE_BUILD_TYPE=Release && make

For debug build: cmake -DCMAKE_BUILD_TYPE=Debug . && make

For portable build (binary that can be executed on different machines): cmake . -DCMAKE_BUILD_TYPE=Portable64 && make ( use Portable32 for 32-bit )

For same build as before (or Release if it's first build): cmake . && make

Add -j parameter to make to speed up if you have multiple cores, i.e. "make -j9" for 8

You may also just download binary releases from github, they are built for ubuntu but will run on most linux distros (only 64-bit for now).

### Launch

Just run ./bin/GLSMAC <smac_directory>

### Reporting problems

If you encountered problem, first thing to try is to update to newest version, maybe it was already fixed.

If it fails to build: create issue with errors from build log

If it crashes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, type "r", make it crash, then type "bt" and create issue with this backtrace and steps to reproduce

If it freezes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, get to where it freezes, then press Ctrl+C, type "bt" and create issue with backtrace and steps to reproduce

If it behaves incorrectly: create issue and specify: 1) steps to reproduce 2) expected behavior 3) actual behavior ; attach screenshots if needed

### Copyright disclaimer

GLSMAC does not include any copyrighted assets from original SMAC, it is designed to use assets of existing SMAC installation on user's computer. It will also work with any third-party assets, as long as they are in same format and in same directory structure as original SMAC.

GLSMAC code is created from scratch and tries to imitate original SMAC logic while not actually containing any of it. It does not disassemble original SMAC binaries and does not link to any of SMAC libraries.

GLSMAC requires user to have original SMAC (+expansion) installed in order to operate in a way similar to original SMAC.

All rights for SMAC/SMACX assets belong to their respective owners. AGPL license you see here only applies to GLSMAC code (and maybe custom-made assets in the future if I decide to make them).

### More information

Primary source of information is git log :)

Then there's discord where updates are frequent, sometimes I'll explain technical details or plans and also put screenshots of newly added stuff. You can also ask me questions and coordinate development if you decide to participate.

Then there's this README but I'll only put generic stuff here and won't update often.

Finally, there are multiple threads on various forums and reddits but I don't have enough time to update them often and may be slow to reply.
