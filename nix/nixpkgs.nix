let
  fetchNixpkgs = { owner, repo, rev, sha256 }:
    builtins.fetchTarball {
      url = "https://github.com/${owner}/${repo}/archive/${rev}.tar.gz";
      inherit sha256;
    };
in

fetchNixpkgs {
  owner = "nixos";
  repo = "nixpkgs";
  rev = "330e54ee51fde8da97088aa12fb93a88fe91f94b";
  sha256 = "1szm6pfnwxfhcg9va5scxdw6f1b4s3bkq04qwxgd3shi0x85v42g";
}
