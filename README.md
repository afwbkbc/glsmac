# GLSMAC

Unofficial open-source OpenGL/SDL2 reimplementation of Sid Meier's Alpha Centauri ( + Alien Crossfire ).

![github-preview-min](https://user-images.githubusercontent.com/7736265/227072592-0acc91e6-d3b4-46d2-98f6-0ba4e430328f.png)

Note: you will still need to own the original game and expansion - GLSMAC will load its resources like images or sounds.

Check [Screenshots](https://github.com/afwbkbc/glsmac/wiki/Screenshots) for current state of project.

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

Mod compatibility - it should work with asset mods that replace .PCX, .WAV or other asset files in SMAC directory (later it will also accept upscaled versions). It won't work with mods that modify or link to terranx.exe. Mods that change .txt files - probably not, maybe some settings and rules will be read from original SMAC's .txt but not all of them.

### Other improvements?

Once I replicate original game, I'll focus on other things, such as: larger maps, larger maximum number of players, custom mod support, online services such as game browser or hosting long-turns games (where people can login and make 1 turn per day or so), alternative art packs (used instead of original, this may mean better graphics)

### Current Version:

# v0.2+

### Roadmap

##### Versions 0.x+: replicating original SMAC gameplay as closely as possible (but without bugs and some technical limitations)

- v0.1: fundamental engine functions, such as: rendering, UI, netcode, sound system

- v0.2: basic map functions, such as: tiles, mapgen, creating/saving/loading games, map rendering

- v0.3: basic multiplayer: ability to host and join games, lobby, save/load games with same players. multiplayer support will be maintained during further steps from now on

- v0.4: game processing, such as: game turns, units (only scout patrols, formers and colony pods), buildings (only recycling tanks), technologies (only centauri ecology), building of bases (only land bases), terraforming (only farms, mines and solar arrays), unit combat, victory by conquest

- v0.5: all remaining SMAC units, buildings, terraformings, technologies, etc

- v0.6: all remaining victory types

- v0.7: AI (computer opponents, cities automation, units automation, etc) and diplomacy

- v0.8: alien lifeforms, ecology

- v0.9: all remaining in-game UI, including game settings

- v0.10: non-crucial things like movies, interludes, dynamic music

- v0.11: final polishing and bugfixing

##### Versions 1.x+: fixing bugs, adding optional small features that weren't present in original SMAC but won't change it's gameplay drastically. See 'GLSMAC v1.x+ features' section at the end

##### Versions 2.x+: adding all kinds of features, as long as they are worth the time. See 'GLSMAC v2.x+ features' section at the end

##### Versions 3.x+: not sure yet, probably something cool.

### Hardware requirements

Minimum recommended (for at least 60 FPS):

- GPU: Intel HD 4400, AMD Radeon R7, Geforce GT710 or similar
- RAM: 2-4GB
- HDD: 50MB

Requirements may go up slightly as more features are added to game. But I'll try to keep FPS above 60 on low-end cards as above. Amount of RAM will limit maximum size of map you can play on (until some kind of disk caching is implemented, but that will cost performance). Disk space is needed for GLSMAC binary, config files, saves, maybe map files and replays.

### Build instructions

Supported platforms:

- GNU/Linux with GCC compiler
- FreeBSD with Clang compiler
- Windows ( Visual Studio + Clang )
- Windows ( MinGW )

Other platforms/toolchains weren't tested but you can try and please report results if you do (or pull request if you manage to get it running on more OSes).

### Linux / FreeBSD

You will need CMake and working compiler (gcc or clang).

You will also need following libraries (unless you build with `-DVENDORED_DEPENDENCIES=YES`):

- FreeType
- SDL2
- SDL2_image
- GL
- GLU
- GLEW
- uuid-ossp
- yaml-cpp

Gentoo: `emerge cmake libsdl2 sdl2-image freetype glu glew ossp-uuid yaml-cpp`

Ubuntu: `apt install cmake build-essential libfreetype-dev libsdl2-dev libsdl2-image-dev libglu-dev libglew-dev libossp-uuid-dev libyaml-cpp-dev`

ArchLinux: `pacman -Syu cmake base-devel freetype2 sdl2 sdl2_image glew yaml-cpp` (you'll need to install `ossp-uuid` manually because it's not in repos)

FreeBSD: `pkg install pkgconf cmake sdl2 sdl2_image glew ossp-uuid yaml-cpp`

Fedora: `dnf install cmake make automake gcc gcc-c++ freetype-devel SDL2-devel SDL2_image-devel glew-devel uuid-devel yaml-cpp-devel`

OpenSUSE: `zypper install -t pattern devel_basis && zypper install cmake-full freetype2-devel SDL2-devel SDL2_image-devel glew-devel uuid-devel yaml-cpp-devel`

It is highly recommended to build project using CMake and make (ninja works too) instead of adding .cpp and .h files manually to IDE (don't report bugs if you try the latter).

It is recommended to build in separate directory. For example: `cmake -S . -B build` (remove build directory when you'll want a clean build for some reason).

For release build (default): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && make -C build`

For debug build: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug && make -C build`

For portable build (binary that can be executed on different machines): `cmake -S . -B build -DCMAKE_BUILD_TYPE=Portable64 && make -C build` ( or `Portable32` for 32-bit )

For same build as before (or Release if it's first build): `cmake -S . -B build && make -C build`

Add -j parameter to make to speed up if you have multiple cores, i.e. `make -C build -j9` for 8 cores.

Optionally, add `-DVENDORED_DEPENDENCIES=YES` to cmake parameters to download and build all required libraries, instead of using system-installed ones. By default this is enabled on Windows and disabled on other OSes. You can't disable it on Windows.

Optionally, use `VERBOSE=1 make -C build` to see actual compiling/linking commands (useful when build fails)

You can also just download binary releases from github, they are built for ubuntu but will run on most linux distros (only 64-bit for now). Windows and other binaries coming soon :)

### Windows

#### Visual Studio

Building with Visual Studio is recommended for windows. You will need **git** (for example, Git SCM) and **Visual Studio 2022** installed. Use the Visual Studio Installer, select **Desktop development with C++**. Then click **modify**->**individual components** on Visual Studio 2022.

You will need to install the following `C++ CMake tools for windows`, `C++ Clang Compiler for windows`, `C++ Clang-cl`, `Windows 11 SDK (10.2.22621.0)`, `Windows Universal CRT SDK`, `Windows Universal C Runtime`.

Once that is done you should clone the git, then open the folder. Press the top button **Project**->**Configure**, then the output should open. Wait for it to say "CMake generation finished.". Once that is done select the build target `GLSMAC.exe` and press the debug button.

You are going to want to add your path to the start argument when running, to do that open the **Solution Explorer** and press **Show all files**. Then go to `out\build\x64-debug-win-clang\bin`, right click GLSMAC.exe and select **Add Debug Configuration**->**Default**.

Then in the file that just open paste

`"args": [ "--smacpath", "\"C:\\GOG Games\\Sid Meier's Alpha Centauri\"" ]`

below name in the configuration. You need to replace the path with your own path to the game. Then you are done and can run the game in Visual Studio.

Alternatively, set working directory to that of your SMAC installation, it will work without arguments then.

#### MinGW

Building on Windows with MinGW is possible, but tricky. You need to install all those libraries manually, and also FreeGLUT. Double-check that you download libraries compiled for MinGW (and not MSVC). GLEW may need to be compiled from source (because they only ship MSVC version). Make sure to have threads-enabled version of MinGW. Maybe this guide will help you - https://github.com/induktio/mingw-glsmac

#### MSVC

Wasn't tested. Feel free to test, but I have feeling it will require many changes to code to be compatible.

### Launch

Just run ./bin/GLSMAC --smacpath <smac_directory>

Or copy GLSMAC into your SMAC directory and run without arguments from there.

Run ./bin/GLSMAC --help to see more options. Debug builds have extra options that aren't available for release builds.

### Reporting problems

If you encountered problem, first thing to try is to update to newest version, maybe it was already fixed.

Next step is to try Stable branch. Main branch contains latest code that may be unfinished or partially finished, and/or contain bugs aswell. Stable branch is supposed to be consistent and bug-free.

Until GLSMAC is v1.0 please do not comment on tiny glitches, sometimes I postpone them bit. Only important game-breaking bugs.

Check existing issues, maybe this bug is already known. Then it would be nice if you comment on existing one with your system specs and maybe other details.

If this feature was working correctly earlier - maybe you can find latest commit where it still worked, it will make fixing faster.

If it fails to build: create issue with errors from build log.

If it crashes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, type "r", make it crash, then type "bt" and create issue with this backtrace and steps to reproduce.

If it freezes: 1) use debug build 2) run gdb --args ./bin/GLSMAC <smac_directory>, get to where it freezes, then press Ctrl+C, type "bt" and create issue with backtrace and steps to reproduce.

If it behaves incorrectly: create issue and specify: 1) steps to reproduce 2) expected behavior 3) actual behavior ; attach screenshots if needed.

Update: on Linux you can use --gdb argument for debug build to start inside gdb automatically

If you're using debug build - it will create files tmp/lastmap.seed, tmp/lastmap.gsm (map file) and tmp/lastmap.dump (full dump of initialized map), attach them too if your issue is related to map. Make sure to gzip lastmap.dump, it's huge otherwise.

### Copyright disclaimer

GLSMAC does not include any copyrighted assets from original SMAC, it is designed to use assets of existing SMAC installation on user's computer. It will also work with any third-party assets, as long as they are in same format and in same directory structure as original SMAC.

GLSMAC code is created from scratch and tries to imitate original SMAC logic while not actually containing any of it. It does not disassemble original SMAC binaries and does not link to any of SMAC libraries.

GLSMAC requires user to have original SMAC (+expansion) installed in order to operate in a way similar to original SMAC.

All rights for SMAC/SMACX assets belong to their respective owners, same for game mechanics, lore and other content (except for new GLSMAC features which didn't exist in original SMAC). AGPL license you see here only applies to GLSMAC code (and maybe custom-made assets in the future if I decide to make them).

### More information

Primary source of information is git log.

Then there's discord where updates are frequent, sometimes I'll explain technical details or plans and also put screenshots of newly added features. There you can also ask me questions and coordinate development if you decide to participate.

Then there's this README but I'll only put generic stuff here and won't update often.

Finally, there are multiple threads on various forums and reddits but I don't have enough time to update them often and may be slow to reply.

About features - they aren't promises with guarantees, they are ideas that I'm thinking of implementing once base game is finished. No specific priorities or deadlines (most complex to implement will usually come last). They are also up for discussing on discord, you can also propose your own ideas and I'll add them if they are good.

All features that change SMAC gameplay will be optional and can be enabled/disabled per game, there will also be "Force Classic Mode" checkbox in settings that will conveniently disable all game-altering additions, for classic SMAC experience.

Emphasis on graphics is average, that means I try to make it look modern and not ugly, but will postpone time-consuming improvements to after v1.0 at least. That means graphics you see now is not final version, it will be better, but now gameplay is higher priority.

### GLSMAC v1.x+ features

##### Art upscaling

SMAC art is 256-color and sounds are of 22khz mono quality. Maybe it's possible to upscale them and improve quality in runtime. Voice can be generated with something like https://beta.elevenlabs.io/

##### Replays

Easy recording of replays (maybe even 'auto-record replay' checkbox in options), then they can be saved, sent to others and watched, alone or as multiplayer game (host game in replay mode). Replays will play in usual way, like in dota2 or stacraft2. Maybe also add replay-to-video and streaming functions.

##### Javascript-based modding support

JavaScript wrappers for easy creation and modification of game logic. Upon starting new game player will be able to choose which mod he wants for this game. If he makes his own mod - there will also be some UI where he can share it, or browse mods of others. If somebody hosts game with mod he just made - others can join it and it will download for them automatically. 'Mod' package will include js files, art (textures, models, sounds), maybe css for custom UI, all bundled into single file for easy sharing.

Maybe other languages too, I just like JS the most 🙂 (as scripting language).

JavaScript engine needs to be built in a way so that it won't block main game, even if he puts things like while (true) {} or fork bombs - then game will just show message that script is frozen and will propose to kill it (if script freezes inside game - game will end and user is back to main menu, so buggy mods won't be very playable but won't cause harm). Special care will need to be taken to prevent any possible buffer overflows and other exploits.

##### Custom factions

Faction editor will be built into game - at faction choosing screen there will be 'Create New Faction' button where user can customize it as he wants. He will also have list of previously created factions, so he can choose any. So similar to Stellaris.

He will be able to use either default art, some new premade arts or upload his own custom art for faction.

However I think there should be some moderation on his art, just to make sure it's not something NSFW or just random pixels in place of bases, or just full-transparent images. So reasonable way would be like this - he uploads new art to his faction, tests it (maybe there will be some 'preview' where he's put into actual game and can play it a bit), then once he's done he saves it and there's status like 'pending approval'. Faction will be immediately available for single player, but multiplayer only after me or some moderator checks it and approves if it looks okay (and game host will also need to enable 'allow custom factions' in rules).

##### Random factions

Another cool way to add variety. At faction selection screen there will be 'randomize' button. When he clicks it:

- faction stats, traits etc, are randomized
- faction names, texts and quotes are randomized with random text generators (but should be realistic enough)
- portrait will be generated by some 'AI face generator' tool, there are some available already. They generate realistic photos of people that don't actually exist.
- images of buildings will be generated too in some way

He will see generated faction, if doesn't like it - he can press Randomize again and new one will be made. He can save it if he liked it. He can also select 'random' for one or several (or all) AIs. In multiplayer games host will have option 'Allow random races' so he disallow them if he doesn't like it.

##### Voice chat

If enabled in settings, during multiplayer games players will be able to talk in voice. It will be either always-on or integrated with diplomacy (i.e. voice will work while diplomacy window is open to another player), or maybe voice channels will be always-on but only for those with Pact status (so if it's 2v2 then each team will have separate voice channel). Most likely it will be configurable per-game.

##### Game browser

Unlike in original SMAC, after pressing "JOIN" in Multiplayer player will see game browser with games hosted by others. There will be new ones (not started yet) and those he played before. So he can just click and join, also he can add specific players to friends to see their games before others, and other stuff like this. Of course there will be 'join by IP' button too just in case (i.e. if master server is down).

#### AI vs AI mode

Allow players to code their own AIs (most likely in JavaScript), then host or join games (with special AI-only game mode). Then game will start and AIs will fight each other automatically, players can observe. Turns will go fast so it's a plus (but there will be turn timer just in case some). Then maybe AIs will get ratings, mmr, 'hall of fame' page for best ones.

Actually it's better to just let GLSMAC open network or local port and communicate via it's special AI API with whatever connects - then user can code AI in any language, f.e. in C for speed reasons, maybe even run it on separate powerful server.
AI-vs-AI mode will also be available as 'single player' where player can test his AIs vs each other, either different AIs or same one (maybe even make it learn by playing many games fast).

Also maybe it should be allowed for player to join as AI to any games (unless host disabled 'Allow remote AIs' in options).

##### MMR system

If GLSMAC becomes popular enough - there may be 'Rated' mode added that will keep track of player ratings. It will be separate page in game browser. Also maybe some kind of matchmaking for new games. In rated mode there will be certain limitations, such as having default game rules (or maybe several choices of rules), unlike in normal mode where everything goes (as long as it can sync between players).

##### Open-source art

Remodel and redraw everything under open-source license, then ship with GLSMAC as complete game (support for original SMAC art will stay if somebody needs). Also replace all sprites with 3D models, for example mind worms, bases.

##### Long-turns mode

Master server may host 'long-turns' games (games with very slow turns, like once per day). It will work like login->turn->logout. Not entirely sure it's needed but I know people play some games like that, i.e. freeorion. If demand is there it can be done.

##### Mobile client

Modern smartphones are more than capable of running game like SMAC. In fact, I played it (multiplayer) on Lenovo K900 through wine some day (had some kind of debian installed on top of android, then X and wine inside), and FPS was fine. Making UI will be challenge but it's possible.

##### Network modes

There will be several options on how players can play games (they can select one when they create game):

##### 1) server-clients : default mode and the only one available before 1.x - one player hosts game on his PC and others connect.

Pros:

- low lag if players are near (maybe even LAN)
- independent from master server (game will keep being playable if it's down)
- host will have local saves

Cons:

- requires player to have real IP and ports open in firewall
- exposes host's IP to other players and vice versa
- puts a higher load on host's PC (if his PC is slow - game may lag between turns)
- if host has slow internet connection - game will lag
- host can cheat (maphack, spawning items for himself) and others can't even check it (so not eligible for rated mode)
- host can lose saves

##### 2) master-server : player "host" will configure game, but it will be physically hosted on my server(s). Host won't have special permissions once game is started.

Pros:

- IPs of players are hidden from each other
- anyone can host, having real IP is not required, no need to configure firewalls/routers etc
- no lag from somebody's PC being slow or somebody's internet being slow (only one with slow internet will get lags but not others)
- nobody can cheat (so it's eligible for rated mode)
- automatic backups of saves

Cons:

- depends on server being up (risk of downtime will be low though)
- if player is too far physically from master server - he can have some lag
- if too many games will need to be hosted at same time - there can be queues or server(s) can become overloaded
- players won't have local saves (unless maybe there will be option to switch to server-clients mode, in this case saves will be downloaded to new host)

##### 3) servers-servers : every player is game server, game processing is split between evenly and with some redundancy

Pros:

- low lag if players are near (maybe even LAN)
- independent from master server (game will keep being playable if it's down)
- everybody will have local saves (so harder to lose them)
- cheats that spawn items won't work (because players are validating each other)
- maybe faster turn processing because load will be split

Cons:

- requires at least one player to have real IP and ports open in firewall
- exposes IPs of all players to each other
- any player can use maphack (and others won't know)
- somewhat harder to implement
- may be unstable and prone to bugs, at least at first

##### 4) cross-hosting : every player online will partially host some other game of other players. in turn, his game will also be hosted by someone else. coordination will be p2p similar to torrents, master server only as backup when p2p fails for some reason

Pros:

- (almost) independent from master server (game will keep being playable if it's down)
- nobody in current game can cheat. when hosting game of others - player can use maphack, but it won't give him any benefits
- (almost) decentralized, amount of decentralization will depend on how many players are online and enabled cross-hosting (and probably in same region)
- saves will be kept on computers of multiple players, plus on master server as backup. They will also replicate between players as hosts change

Cons:

- system will work poorly if there's low amount of players online (with cross-hosting enabled), it will fallback to other mode in worst case
- user's PC will process games of other players, store saves of other games, some may not like it even if they get same services from others in return
- redundancy requirement of at least 2x means it's a bit inefficient in results/work ratio (multiple PCs will need to do the exact same processing)
- also 2x or more increased internet traffic, same reason
- rare sudden lags if hosts change too often (for example game was hosted on 3 PCs but they all went offline at same time)
- the most complex system to implement, will certainly be the last if at all (probably not before 2.x)

### GLSMAC v2.x+ features

##### Spherical planet

Planet can be actually spherical. It means that:

- planet will look like a sphere when zoomed out
- north and south poles will also be passable  (maybe only by aircraft)
- map can be rotated infinitely up and down too (or diagonal)
- map rendering will need to be done in perspective projection instead of orthographic, possibly other warping will be needed to display distances between things correctly

##### Tiles manipulation

Add some late game technology branch that enables manipulation of tiles themselves. Some examples:

- merging several tiles into one (so they all can be crossed in 1 move)
- splitting tile into several (so units will have to spend more moves to cross them, or for example to fit more terraforming or even cities in same space)
- connecting two tiles as 'doorway' - when unit steps on it - it teleports to other side, and vice versa
- alter time on tile (for example, turns may happen only once per 5 real turns, or multiple turns per 1 real turn, or variable speeds
- removing tile permanently from map (it will just look as black spot and nobody can enter it anymore). every adjactent tile will be harder to remove (to prevent somebody from building impassable 'wall')

Root technology will add special terraforming module (named maybe Space-Time Manipulator). Visually tiles will be distorted too, for example if somebody merges 5 tiles in straight line - he will get a tile that is elongated. Somewhat later there can be missile module (Void Rift Payload?), when detonated - it will permanently erase tiles in round area around it, then this rift will slowly grow in random directions (either for specific duration or infinitely until stopped by some other technology).

##### RTS mode

A mode without turns but with constant flow of time. It will have added features like key bindings, mass selects, that are present in existing RTS like Starcraft, so that it would be possible to manage all stuff fast enough. Players may also need to resort to automation of cities (and maybe units) once their empires grow big, and manually control only the most critical areas (i.e. areas near enemy). Units' movement won't be based on tiles, it will be smooth (cities and terraforming still based on tiles). Attack range will stay fixed (as distance between adjactent tiles, more only for artilleries). Air units will automatically fly to nearest base once out of fuel.

##### Playable planet

Just had an idea to make Planet a playable side. It would be with completely different gameplay, for example there will be (obviously) no bases, buildings or typical units, instead it will be possible to build fungal towers, spawn worms and more advanced lifeforms, spread fungus. There will be unique resource types which are gained from good ecology, but depleted when ecology worsens (both locally and globally). So there will be incentive to attack the most industry-polluting cities first. Fungus will also need to spread to act as resource transfer aswell so worms will need to be created to slowdown terraforming.

In theory there can be multiple Planet 'factions' aswell, then both players would fight each other for resources and space for fungus (or maybe unite?).

##### Multiple planets

Maybe add technologies and units to detect other planets, then travel and colonize them. At maximum map zoomout it will turn into sphere and player can see and click others (then he'll zoom in and can do same stuff like move units, build bases, etc). Maybe add different kinds of alien life on them. At very late stage add more powerful planet busters that can actually destroy planets, also add some means to defend. Maybe space weapons. Planet may also get stronger late units that may sometimes force player off planet completely. Fungus can spread to other planets aswell at some point of time.

Still, it won't turn game into some Stellaris, number of planets will be fairly small, maybe 10 at most, all at same star (no lanes).

##### Infinite mode

A special, grand scale game mode.

- hosted by master server
- lasts forever
- map is infinite (procedurally generated like in minecraft once somebody reaches it's 'edge')
- number of players is infinite, every new player spawns at some distance from others to give him time to prepare
- if player is defeated - he can reconnect and start a new life
- 'flowering' cycle will intensify and weaken, there will be times where world will be consumed by fungus, but then it will step back for a while, and so on
- number of AIs will be kept at minimum, only to compensate low online when it's low
- turns will need to be fragmented instead of global, which means world will be split into regions where turns will move separately from each other. So time may flow differently in different regions, depending on how fast players there make turns. This will give small boost to new players since turns are very fast at start of the game, then later turns will slowdown if player gets more cities and stuff. There is no way to have global turns because if ~1000 players are playing then 999 may need to wait for 1 player that just went afk or is just very slow (a nightmare even with parallel turns mode). 

