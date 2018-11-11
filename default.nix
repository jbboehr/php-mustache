{
  pkgs ? import <nixpkgs> {},
  php ? pkgs.php,

  phpMustacheVersion ? null,
  phpMustacheSrc ? null,
  phpMustacheSha256 ? null,

  mustacheSpecVersion ? null,
  mustacheSpecSrc ? null,
  mustacheSpecSha256 ? null,
  mustache_spec ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/mustache-spec/archive/210109d9ccd05171233d8d7a8ceb97f3bddc790a.tar.gz;
    sha256 = "0ss51lraznpbixahfdj7j7wg41l1gnwq9qbm9aw2lkc9vvsc3h3c";
  }) + "/derivation.nix")) { inherit mustacheSpecVersion mustacheSpecSrc mustacheSpecSha256; },

  libmustacheVersion ? null,
  libmustacheSrc ? null,
  libmustacheSha256 ? null,
  libmustache ? pkgs.callPackage (import ((fetchTarball {
    url = https://github.com/jbboehr/libmustache/archive/ddf7eb217e04fc42624911913f30009739c9ead4.tar.gz;
    sha256 = "19bn5kgihf5b2y6lpp259ar616j2mznskzmh57icdyr6wpk2zw90";
  }) + "/derivation.nix")) { inherit libmustacheVersion libmustacheSrc libmustacheSha256 mustache_spec; }
}:

pkgs.callPackage ./derivation.nix {
  inherit php libmustache mustache_spec phpMustacheVersion phpMustacheSrc phpMustacheSha256;
}

