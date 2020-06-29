let
  sources = import ./sources.nix;
  pkgs = import sources.nixpkgs {};
  cs2110 = import sources.cs2110-nix {};
in {
  shell = pkgs.mkShell {
    buildInputs = with pkgs; [
      gnumake
      mednafen
      gcc-arm-embedded
      cs2110.nin10kit
      cs2110.cs2110-vbam-sdl
    ];
    shellHook = ''
      export LINKSCRIPT_DIR="${cs2110.cs2110-gba-linker-script}"
    '';
  };

  gba-tetris = cs2110.makeGBA {
    src = ../.;
    name = "Tetris";
    cfgFile = ../mednafen-09x.cfg;
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.mit;
    };
  };
}

