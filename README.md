# Elemental Legends DS — RPG expansion

Original homebrew inspired by elemental fantasy and creature collecting. No proprietary graphics, music, or game assets are included.

## Features
- Three 32×24 areas: Lotus Village, Spirit Grove, Ancient Temple.
- NPC conversations with Elder Sora, Healer Mira, Ranger Kai.
- Three quests: win three wild battles, capture two creatures, defeat Void Warden. Quest rewards and temple unlock.
- Four-slot creature party with selection, leader changes, XP, leveling and companion attacks.
- Turn-based elemental bending, Avatar State, healing, and weakened-creature capture.
- Top-screen 2D pixel graphics and bottom-screen touch menus.
- Save/load using libfat and `fat:/elemental_legends.sav`, with versioning and checksum.

## Build
Install devkitPro's Nintendo DS development packages (`nds-dev`, including devkitARM, libnds, libfat) and run `make` in this directory. The output should be `elemental_legends.nds`. The Makefile expects `DEVKITARM` and the devkitPro environment to be configured. Run `make clean` before rebuilding if necessary.

This environment does not have devkitARM installed, so the ROM has **not been compiled or tested** here. The project is provided as build-targeted source, not a guaranteed working binary.

## Controls
D-pad: move or choose element; A: talk/interact/strike; B: back/companion; X: save (world)/special (battle); Y: load (world)/Avatar State (battle); START: party; SELECT: quests; L: heal; R: capture. Touchscreen provides party, quest, save/load, travel and battle controls.

## Save compatibility
A writable FAT filesystem must be exposed to libfat by the emulator or homebrew device. Some emulators don't expose a writable FAT volume by default; saving then reports unavailable. Saves are not compatible with the earlier prototype and should not be renamed from unrelated formats.

## Gameplay notes
Talk to Elder Sora and Ranger Kai in Lotus Village to accept quests. The Grove exit is east of the village; the village exit is west of the Grove. After completing Elder Sora's quest, the temple entrance unlocks at the north edge of Lotus Village. In the temple, approach the Void Warden and press A.

## Limitations
Sprites are procedurally drawn pixel characters, not external sprite sheets. World movement is tile-based, and exploration uses a centered camera. Touchscreen travel buttons are a convenience shortcut. There is no sound, cinematic animation, or collision with NPCs. This is a compact RPG prototype rather than a full commercial-scale game.
