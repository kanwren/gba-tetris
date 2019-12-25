```
████████╗███████╗████████╗██████╗ ██╗███████╗
╚══██╔══╝██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝
   ██║   █████╗     ██║   ██████╔╝██║███████╗
   ██║   ██╔══╝     ██║   ██╔══██╗██║╚════██║
   ██║   ███████╗   ██║   ██║  ██║██║███████║
   ╚═╝   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝
```

This is a very basic re-implementation of the classic NES game "Tetris" as a
simple example of how to write GBA games in C.


# Building

There are two options:

- Run `nix-build`, if using Nix. This takes care of all of
  the necessary dependencies and patching.
- Use the GT CS 2110 toolchain:
  * Use the Docker container distributed by CS 2110, or install the following
    toolchain dependencies using `apt`:
    - [GBA linker script](http://ppa.launchpad.net/tricksterguy87/ppa-gt-cs2110/ubuntu/pool/main/c/cs2110-gba-linker-script/)
    - `gcc-arm-embedded`
  * Run `make med`

If not using `apt` or `nix`, you need to set `LINKSCRIPT_DIR` to the directory
containing `GBAVariables.mak` from the linker script package. You also need to
patch the `GBAVariables.mak` Makefile manually in order to properly find
`ARMINC`, `ARMLIB`, and `GCCLIB` from `gcc-arm-embedded`. They should be set to
the following paths:

- `ARMINC ${gcc-arm-embedded}/arm-none-eabi/include`
- `ARMLIB ${gcc-arm-embedded}/arm-none-eabi/lib`
- `GCCLIB ${gcc-arm-embedded}/lib/gcc/arm-none-eabi/$(arm-none-eabi-gcc -dumpversion)`

Where `${gcc-arm-embedded}` is the path to `gcc-arm-embedded` and
`$(arm-none-eabi-gcc -dumpversion)` is the result of running the inner command.


# Instructions

- Your goal is to fill and clear rows using the random pieces dropped from the
  top of the board. The more rows you clear at once, the more points you earn.
  Try to get the highest score you can!

- Unlike newer versions of Tetris, the piece generation in NES Tetris is
  random; it's possible to suffer from long droughts without certain pieces, as
  the generator will not compensate to be nicer to the player.

- You can shift pieces side-to-side or rotate them into position.

- Clearing 4 lines at once is called a "tetris," and will score the most points.

- There are 7 kinds of pieces, called "tetrominoes," each made of four blocks.
  Only the line pieces is able to score a tetris.

- Try to keep the board low; if the pieces reach the top of the board, you lose!

- Every 4 lines you clear advances the game by one level, and the pieces speed
  up slightly.

- If you reach level 10, you win! (It's kept pretty short to make it more
  playable, but this is configurable via `MAX_LEVELS` in `game.h`)


# Controls

- Use LEFT and RIGHT to move the piece in either direction as it falls. Holding
  LEFT or RIGHT triggers DAS, which causes the piece to move to the side more
  quickly after a small delay.

- Use DOWN to drop the piece quickly.

- Use A to rotate the piece clockwise.

- Use B to rotate the piece counterclockwise.

- Use SELECT to return to the title screen at any time.

