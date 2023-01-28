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

### Current Version:

# v0.1+

### Roadmap

##### Versions 0.x: replicating original SMAC gameplay as closely as possible (but without bugs and some technical limitations)

- v0.1: fundamental engine functions, such as: rendering, UI, netcode, sound system

- v0.2: basic map functions, such as: tiles, mapgen, creating/saving/loading games, map rendering

- v0.3: basic multiplayer: ability to host and join games, lobby, save/load games with same players. multiplayer support will be maintained during further steps from now on

- v0.4: game processing, such as: game turns, units, buildings, technologies, building of bases, terraforming, basic combat, victory by conquest

- v0.5: all remaining SMAC units, buildings, terraformings, tech, etc

- v0.6: all remaining victory types

- v0.7: AI (computer opponents, cities automation, units automation, etc) and diplomacy

- v0.8: alien lifeforms, ecology

- v0.9: all remaining in-game UI, including game settings

- v0.10: non-crucial things like movies, interludes

- v0.11: final polishing and bugfixing

##### Versions 1.x: fixing bugs, adding optional small features that weren't present in original SMAC but won't change it's gameplay drastically. See 'GLSMAC v1.x+ features' section at the end

##### Versions 2.x: adding all kinds of features, as long as they are worth the time. See 'GLSMAC v2.x+ features' section at the end

##### Versions 3.x: not sure yet, probably something cool.

### Build instructions

Target platform is GNU/Linux with GCC compiler, it should also be possible to build it with MinGW (if you're on Windows). Other compilers/platforms weren't tested but you can try.

You will need following libraries (-dev versions): FreeType2, SDL2, SDL_image, GL, GLU, GLEW

Ubuntu/Debian/Mint: sudo apt-get install libfreetype-dev libsdl2-dev libsdl2-image-dev libglu-dev libglew-dev

Gentoo: sudo emerge libsdl2 sdl2-image freetype glu glew

Building on Windows with MinGW is possible, but tricky. You need to install all those libraries manually, and also FreeGLUT. Double-check that you download libraries compiled for MinGW (and not MSVC). GLEW may need to be compiled from source (because they only ship MSVC version). Make sure to have threads-enabled version of MinGW. MSYS and TDM-GCC are recommended for more consistent development environment. Maybe this guide will help you - https://github.com/induktio/mingw-glsmac

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

About features - they aren't promises with guarantees, they are ideas that I'm thinking of implementing once base game is finished. No specific priorities or deadlines (most complex to implement will usually come last). They are also up for discussing on discord, you can also propose your own ideas and I'll add them if they are good.

### GLSMAC v1.x+ features

##### Art upscaling

SMAC art is 256-color and sounds are of 22khz mono quality. Maybe it's possible to upscale them and improve quality in runtime.

##### replays

Easy recording of replays (maybe even 'auto-record replay' checkbox in options), then they can be saved, sent to others and watched, alone or as multiplayer game (host game in replay mode). Replays will play in usual way, like in dota2 or stacraft2.

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
- portrait will be generated by some 'AI face generator' tool, there are some available already.  They generate realistic photos of people that don't actually exist.
-  images of buildings will be generated too in some way

He will see generated faction, if doesn't like it - he can press Randomize again and new one will be made. He can save it if he liked it. He can also select 'random' for one or several (or all) AIs. In multiplayer games host will have option 'Allow random races' so he disallow them if he doesn't like it.

##### Voice chat

If enabled in settings, during multiplayer games players will be able to talk in voice. It will be either always-on or integrated with diplomacy (i.e. voice will work while diplomacy window is open to another player), or maybe voice channels will be always-on but only for those with Pact status (so if it's 2v2 then each team will have separate voice channel). Most likely it will be configurable per-game. 

##### Game browser

Unlike in original SMAC, after pressing "JOIN" in Multiplayer player will see game browser with games hosted by others. There will be new ones (not started yet) and those he played before. So he can just click and join, also he can add specific players to friends to see their games before others, and other stuff like this. Of course there will be 'join by IP' button too just in case (i.e. if master server is down). 

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

###### 1) server-clients : default mode and the only one available before 1.x - one player hosts game on his PC and others connect.

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

###### 2) master-server : player "host" will configure game, but it will be physically hosted on my server(s). Host won't have special permissions once game is started.

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

###### 3) servers-servers : every player is game server, game processing is split between evenly and with some redundancy

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

###### 4) cross-hosting : every player online will partially host some other game of other players. in turn, his game will also be hosted by someone else. coordination will be p2p similar to torrents, master server only as backup when p2p fails for some reason

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
        - the most complex system to implement, will certainly be the last if at all
        - rare sudden lags if hosts change too often (for example game was hosted on 3 PCs but they all went offline at same time)

### GLSMAC v2.x+ features

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

