{
  pkgs ? import <nixpkgs> {},
  stdenv ? pkgs.stdenv,
  php ? pkgs.php,
  buildPecl ? pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php stdenv;
  },
  gitignoreSource ? (import (pkgs.fetchFromGitHub {
    owner = "hercules-ci";
    repo = "gitignore";
    rev = "00b237fb1813c48e20ee2021deb6f3f03843e9e4";
    sha256 = "sha256:186pvp1y5fid8mm8c7ycjzwzhv7i6s3hh33rbi05ggrs7r3as3yy";
  }) { inherit (pkgs) lib; }).gitignoreSource,

  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = "https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz";
    sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
  }))) {},

  libmustache ? pkgs.callPackage (import ((fetchTarball {
    url = "https://github.com/jbboehr/libmustache/archive/1761e260bbd2247891aa49cb04318b40206b9a4b.tar.gz";
    sha256 = "0pch9i6yywx2p7d5j3rz8lwgrg69hfs8bagdjm4whcmvrpx8cvi4";
  }))) { inherit mustache_spec stdenv; },

  phpMustacheVersion ? null,
  phpMustacheSha256 ? null,
  phpMustacheSrc ? pkgs.lib.cleanSourceWith {
    filter = (path: type: (builtins.all (x: x != baseNameOf path) [".idea" ".git" "ci.nix" ".travis.sh" ".travis.yml" ".ci" ".github"]));
    src = gitignoreSource ./.;
  }
}:

pkgs.callPackage ./nix/derivation.nix {
  inherit buildPecl php libmustache mustache_spec phpMustacheVersion phpMustacheSrc phpMustacheSha256;
}

