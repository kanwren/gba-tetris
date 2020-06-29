{ nixpkgs ? import ./nixpkgs.nix
}:

with rec {
  cs2110overlay = self: super: {
    cs2110 =
      let
        cs2110src = super.fetchFromGitHub {
          owner = "nprindle";
          repo = "cs2110-nix";
          rev = "e25774440dd3cfd19fa2f9d62bbdb82cbe8bfa83";
          sha256 = "02p39jri9ka99c1k6wx5b72p89lbpl72slhcl3y53wajsmvp3j5r";
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
      gnumake
      mednafen
      gcc-arm-embedded
      cs2110.nin10kit
      cs2110.cs2110-vbam-sdl
    ];
    shellHook = with pkgs; ''
      export LINKSCRIPT_DIR="${cs2110.cs2110-gba-linker-script}"
    '';
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

