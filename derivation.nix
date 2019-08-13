{ php, stdenv, autoreconfHook, fetchurl, mustache_spec, libmustache, pkgconfig,
  buildPecl ? import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    # re2c is required for nixpkgs master, must not be specified for <= 19.03
    inherit php stdenv autoreconfHook fetchurl;
  },
  phpMustacheVersion ? null,
  phpMustacheSrc ? null,
  phpMustacheSha256 ? null
}:

let
  orDefault = x: y: (if (!isNull x) then x else y);
in

buildPecl rec {
  pname = "mustache";
  name = "mustache-${version}";
  version = orDefault phpMustacheVersion "v0.7.4";
  src = orDefault phpMustacheSrc (fetchurl {
    url = "https://github.com/jbboehr/php-mustache/archive/${version}.tar.gz";
    sha256 = orDefault phpMustacheSha256 "163lrr2869zpzyxiag6af1jvxhg4ivqbljrfyqgbflcq2c9vn975";
  });

  buildInputs = [ libmustache ];
  nativeBuildInputs = [ mustache_spec pkgconfig ];

  # needed or the tests will fail below
  postBuild = ''
      PREV=$(patchelf --print-rpath modules/mustache.so)
      patchelf --set-rpath ${libmustache}/lib:$PREV modules/mustache.so
    '';

  doCheck = true;
  checkTarget = "test";
  checkFlagsArray = ["REPORT_EXIT_STATUS=1" "NO_INTERACTION=1" "TEST_PHP_DETAILED=1"];
}

