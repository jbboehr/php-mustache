{
  lib,
  php,
  stdenv,
  pkg-config,
  valgrind,
  autoreconfHook,
  buildPecl,
  src,
  lcov,
  mustache_spec,
  libmustache,
  checkSupport ? false,
  valgrindSupport ? true,
  coverageSupport ? false,
}:
(buildPecl rec {
  pname = "mustache";
  name = "mustache-${version}";
  version = "v0.9.3";

  inherit src;

  buildInputs = [libmustache];
  nativeBuildInputs =
    [php.unwrapped.dev pkg-config mustache_spec]
    ++ lib.optional valgrindSupport valgrind
    ++ lib.optional coverageSupport lcov;

  passthru = {
    inherit php stdenv;
  };

  configureFlags =
    []
    ++ lib.optional coverageSupport ["--enable-mustache-coverage"];

  makeFlags = ["phpincludedir=$(dev)/include"];
  outputs =
    lib.optional (checkSupport && coverageSupport) "coverage"
    ++ ["out" "dev"];

  doCheck = checkSupport;
  theRealFuckingCheckPhase =
    ''
      runHook preCheck
      REPORT_EXIT_STATUS=1 NO_INTERACTION=1 make test TEST_PHP_ARGS="-n -j$(nproc --all)" || (find tests -name '*.log' | xargs cat ; exit 1)
    ''
    + (lib.optionalString valgrindSupport ''
      USE_ZEND_ALLOC=0 REPORT_EXIT_STATUS=1 NO_INTERACTION=1 make test TEST_PHP_ARGS="-n -m -j$(nproc --all)" || (find tests -name '*.mem' | xargs cat ; exit 1)
    '')
    + ''
      runHook postCheck
    '';

  preBuild = lib.optionalString coverageSupport ''
    lcov --directory . --zerocounters
    lcov --directory . --capture --compat-libtool --initial --output-file coverage.info
  '';

  postCheck = lib.optionalString coverageSupport ''
    lcov --no-checksum --directory . --capture --no-markers --compat-libtool --output-file coverage.info
    set -o noglob
    lcov --remove coverage.info '${builtins.storeDir}/*' \
        --compat-libtool \
        --output-file coverage.info
    set +o noglob
    mkdir -p $coverage
    genhtml coverage.info -o $coverage/html/
    cp coverage.info $coverage/coverage.info
  '';

  meta = with lib; {
    homepage = "https://github.com/jbboehr/php-mustache";
    license = licenses.bsd3;
    platforms = platforms.linux;
    outputsToInstall = outputs;
  };

  #TEST_PHP_DETAILED = 1;
})
.overrideAttrs (o:
    o
    // {
      checkPhase = o.theRealFuckingCheckPhase;
    })
