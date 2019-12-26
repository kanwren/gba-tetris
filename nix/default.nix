{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "4f800ffbdeb4d0dc4b327a0e6b80bd30e396ef84";
          sha256 = "0lbl04s7p6gfinspxcmmsfr5yxyl8p85lxpgv8lji29abgi56aka";
        };
      in import cs2110src {};
  };

  pkgs = import nixpkgs {
    overlays = [ cs2110overlay ];
    config = {
      allowUnfree = true;
      allowBroken = false;
    };
  };
};

{
  env = pkgs.mkShell {
    buildInputs = with pkgs; [
      cmake
      gcc
      cs2110.nin10kit
      cs2110.cs2110-vbam-sdl
    ];
  };

  gba-tetris = pkgs.cs2110.makeGBA {
    src = ../.;
    executableName = "Tetris";
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.gpl3;
    };
  };
}

