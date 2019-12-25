#! /usr/bin/env nix-shell
#! nix-shell shell.nix -i bash
for f in src/images/*.png; do
  nin10kit --mode=3 ${f%.*} $f
done
