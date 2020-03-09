{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "f9dd686e7ffa79c944f5fbc3d0fc4ccfacc82199";
          sha256 = "01wj36yahzww7zb3vd630dm7palb826k5pvd3ifdwhz9bmwcmp5j";
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
    cfgFiles = [ ../mednafen-09x.cfg ];
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.gpl3;
    };
  };
}

