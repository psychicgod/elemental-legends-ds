# Build and test Elemental Legends DS

## Requirements
Install the **devkitPro** toolchain and its `nds-dev` package group, which supplies devkitARM, libnds, libfat and ndstool. Follow the current instructions at https://devkitpro.org/wiki/Getting_Started .

In a shell configured for devkitPro:

```sh
cd elemental_legends_nds
./build.sh
```

If successful, this generates `elemental_legends.nds`. To run it, open the `.nds` file in a Nintendo DS emulator such as melonDS. FAT save support requires a writable FAT device exposed to the homebrew program; an emulator's ordinary cartridge save emulation is not necessarily sufficient.

## Manual emulator test checklist

1. Start new game in Lotus Village, move using D-pad and talk to Elder Sora with A.
2. Accept the Grove quest. Travel to Spirit Grove and trigger three encounters; win battles and check quest progress.
3. Weaken an enemy below 40% HP and use R to capture; check the party menu (START) and change the leader.
4. Speak to Ranger Kai, accept the capture quest, and verify capture progress.
5. Speak to Healer Mira and confirm HP/party recovery.
6. Save with X, restart, load with Y and check position, quest progress, and party. Requires FAT storage.
7. After completing the Grove quest, enter the Ancient Temple from the north of the village and fight the Void Warden.
8. Exercise bottom touchscreen buttons for combat, party, quests, save and load.

## Verification performed in this environment

- Host GCC C11 syntax-only check using minimal substitute DS headers: **PASS**, with `-Wall -Wextra -Werror`.
- Actual ARM compilation/linking/ROM generation: **NOT RUN** (devkitPro not installed).
- Emulator runtime testing: **NOT RUN** (no compiled ROM).

The substitute headers verify C syntax only, not libnds API compatibility or ROM runtime behavior.

## Changes from previous RPG source

- Made `LIBFAT` path explicit in Makefile when not already defined.
- Added a build script that checks required devkitPro components.
- Fixed misleading indentation warnings in C source.
- Corrected Grove quest progression so wild battle victories also count when a spirit is captured.
- Added stricter save data validation for progression values.
