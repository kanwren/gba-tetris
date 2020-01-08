{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "75949c31e0a9cda14ae5e655b33bcd7dbe98f9c9";
          sha256 = "0kjxng2idjv3hi9wyl56k0rxi6n3l2bqvrhgizcsy60i71sklbmc";
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

