let
  sources = import ./sources.nix;
  pkgs = import sources.nixpkgs {};
  cs2110 = import sources.cs2110-nix {};
in {
  shell = cs2110.gba-shell;

  gba-tetris = cs2110.makeGBA {
    src = ../.;
    name = "Tetris";
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.mit;
    };
  };
}

