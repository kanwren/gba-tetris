{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "57f38afc31ea99b8b68bca7e61c370c234f172ee";
          sha256 = "1vrb024k2ark6w265ync6y921s41plkk0gb09y0zqjk698wyhgk0";
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
    name = "Tetris";
    cfgFile = ../mednafen-09x.cfg;
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.mit;
    };
  };
}

