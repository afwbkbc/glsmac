# Change Log

## [v0.2] - 2023-04-03

### Added
- Entering game mode via main menu:

    Start Game -> Make Random Map or Customize Random Map -> ... -> Play With Standard Rules
    
    Quickstart
    
    Start Game -> Load Map File

- Simple perlin-based map generator

    default map sizes have ratio of 2:1 instead of original 1:1 because 2:1 looks better on wide screens

    total number of tiles for every size are approximately same as in original

- Map rendering (tiles, water, coasts, sprites, minimap)
- Most of in-game UI
- Map editor

    activated by default once you're in game

    works same as in original SMAC - select tool and brush and draw with them (left mouse - draw, right mouse - undraw)

    second tool ('Cover Map') replaced with 'Moisture' tool because it makes more sense

- Map saving ( Left Menu -> EDIT MAP -> Save Map... )
- Map loading ( Left Menu -> EDIT MAP -> Load Map... )
- MSVS+Clang support
- FreeBSD+Clang support
- New command-line options ( add --help to see them, there are more in debug build )
- Small 'GLSMAC' label with version in top right corner of main menu to avoid confusion

### Changed
- Alien Crossfire background replaced with Alpha Centauri background in main menu
- Improved command-line syntax
- GLSMAC.exe can now be placed in SMAC directory and launched without arguments
- Disabled memory debug and gdb by default in debug builds

### Fixed
- Windows builds
- Loading resources from case-insensitive filesystems
- Excessive redraws of UI
- Slow texts rendering
- Some crashes

### Known bugs
- Small line artifacts between tiles on map zoomout
- Crash when loading large maps with very low free RAM

## [v0.1] - 2023-01-24

### Added
- Basic engine functions (window initialization, sounds, textures, netcode, fonts, etc)
- Intro logo
- Main menu
- Some demo popups, semi-functional multiplayer lobby
 
