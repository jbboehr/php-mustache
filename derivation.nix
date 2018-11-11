{ php, stdenv, autoreconfHook, fetchurl, mustache_spec, libmustache, pkgconfig,
  phpMustacheVersion ? null,
  phpMustacheSrc ? null,
  phpMustacheSha256 ? null
}:

let
  orDefault = x: y: (if (!isNull x) then x else y);
  buildPecl = import <nixpkgs/pkgs/build-support/build-pecl.nix> {
    inherit php stdenv autoreconfHook fetchurl;
  };
in

buildPecl rec {
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

