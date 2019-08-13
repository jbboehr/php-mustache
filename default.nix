{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,
  buildPecl ? pkgs.callPackage <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php;
  },

  phpMustacheVersion ? null,
  phpMustacheSrc ? ./.,
  phpMustacheSha256 ? null,

  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
    sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
  }))) {},

  libmustache ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/libmustache/archive/bebc4573a849fcfb9054462752b75bdb181d9607.tar.gz;
    sha256 = "1zngfl794vrchxwz5qs80j5421dhqxsmr0lnz1cwxjv06s8zv8iz";
  }))) { inherit mustache_spec; }
}:

pkgs.callPackage ./derivation.nix {
  inherit buildPecl php libmustache mustache_spec phpMustacheVersion phpMustacheSrc phpMustacheSha256;
}

