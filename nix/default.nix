{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "62a79ef18bfef24aac2ef22642c2d2f99e4e3242";
          sha256 = "1x0rsspilhkxvl8zcm9p6m52l34xwyhwz4inra2m43vhscmxxv01";
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
    cfgFile = ../mednafen-09x.cfg;
    attrs.meta = with pkgs.stdenv.lib; {
      description = "A basic Tetris game for the GBA";
      license = licenses.gpl3;
    };
  };
}

