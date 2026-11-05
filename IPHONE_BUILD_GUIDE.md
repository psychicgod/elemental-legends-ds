# Build Elemental Legends DS from an iPhone

## Upload the project to GitHub
1. Download `elemental_legends_ds_github_ready.zip` to the iPhone Files app and unzip it.
2. In Safari, create a new **private** GitHub repository. Do not add a README during creation.
3. The easiest mobile upload is to use a Git client app that supports committing an entire folder (including hidden `.github` files). Upload the *contents* of the `elemental_legends_nds` folder at the repository root.
4. Confirm GitHub shows `Makefile`, `build.sh`, `source/main.c`, and `.github/workflows/build-nds.yml` at those exact paths.
   Note: Safari's GitHub web upload may not preserve a complete folder tree conveniently on iPhone. A Git client is recommended.
5. On GitHub, open **Actions**. If asked, enable Actions for the repository.
6. Choose **Build Elemental Legends DS**, tap **Run workflow**, select `main`, then run it.
7. Wait for the job to finish. If successful, open the run and download **Elemental-Legends-DS-ROM** under Artifacts.
8. In Files, unzip the downloaded artifact. It should contain `elemental_legends.nds`.
9. Import that `.nds` file into a Nintendo DS emulator such as Delta on iPhone.

## If the workflow fails
Open the red workflow run -> **Compile Nintendo DS ROM** -> failed step -> copy the error log.
The source has not been compiled or emulator-tested in this environment; this workflow
is a build attempt, not a guarantee that the game works. Share the error log for a fix.

## Save note
The game's FAT-based save path may not be writable in all emulators.
Use emulator save states until in-game save compatibility has been verified.
