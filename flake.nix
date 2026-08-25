{
  description = "jbboehr/php-mustache";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-26.05";
    systems.url = "github:nix-systems/default-linux";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
    gitignore = {
      url = "github:hercules-ci/gitignore.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    git-hooks = {
      url = "github:cachix/git-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    nix-github-actions = {
      url = "github:nix-community/nix-github-actions";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    nix-phps.url = "github:fossar/nix-phps";
    libmustache = {
      url = "github:jbboehr/libmustache/develop";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.systems.follows = "systems";
      inputs.flake-utils.follows = "flake-utils";
      inputs.gitignore.follows = "gitignore";
    };
    mustache_spec = {
      url = "github:jbboehr/mustache-spec";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    gitignore,
    git-hooks,
    nix-github-actions,
    nix-phps,
    libmustache,
    mustache_spec,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        phpSanitizerFlags = "-O1 -g -fno-omit-frame-pointer -fsanitize=address";
        php83Sanitized =
          (pkgs.php83.overrideAttrs (finalAttrs: previousAttrs: {
            CFLAGS = "${previousAttrs.CFLAGS or ""} ${phpSanitizerFlags}";
            CXXFLAGS = "${previousAttrs.CXXFLAGS or ""} ${phpSanitizerFlags}";
            # PHP itself is ASan-only, but the process must provide the UBSan runtime
            # used by the extension and libmustache.
            LDFLAGS = "${previousAttrs.LDFLAGS or ""} -fsanitize=address,undefined";
            dontStrip = true;
          })).buildEnv {
            extensions = _: [];
          };
        php83FuzzerUnwrapped = ((pkgs.php83.override {
            stdenv = pkgs.llvmPackages.stdenv;
            cgiSupport = false;
            fpmSupport = false;
            pearSupport = false;
            pharSupport = false;
            phpdbgSupport = false;
            argon2Support = false;
            systemdSupport = false;
            valgrindSupport = false;
          })
              .unwrapped)
          .overrideAttrs (finalAttrs: previousAttrs: {
          CFLAGS = "${previousAttrs.CFLAGS or ""} -O1 -g -fno-omit-frame-pointer";
          CXXFLAGS = "${previousAttrs.CXXFLAGS or ""} -O1 -g -fno-omit-frame-pointer";
          configureFlags =
            (previousAttrs.configureFlags or [])
            ++ [
              "--enable-fuzzer"
              "--with-pic"
              "--enable-debug-assertions"
              "--enable-address-sanitizer"
              "--enable-undefined-sanitizer"
            ];
          patches = (previousAttrs.patches or []) ++ [./nix/php-fuzzer-target.patch];
          postPatch =
            (previousAttrs.postPatch or "")
            + ''
              cp ${./fuzz/php_mustache_fuzzer.c} sapi/fuzzer/fuzzer-mustache.c
            '';
          postInstall =
            (previousAttrs.postInstall or "")
            + ''
              install -Dm755 sapi/fuzzer/php-fuzz-mustache "$out/bin/php-fuzz-mustache"
            '';
          dontStrip = true;
        });
        src' = gitignore.lib.gitignoreSource ./.;

        src = pkgs.lib.cleanSourceWith {
          name = "php-mustache-source";
          src = src';
          filter = gitignore.lib.gitignoreFilterWith {
            basePath = ./.;
            extraRules = ''
              .clang-format
              composer.json
              composer.lock
              .editorconfig
              .envrc
              .gitattributes
              .github
              .gitignore
              *.md
              *.nix
              flake.*
              fuzz/
              nix/php-fuzzer-target.patch
            '';
          };
        };

        libmustachePackage = libmustache.packages.${system}.libmustache;
        libmustacheCmakePackage = libmustache.packages.${system}.libmustache-cmake;
        # Keep the sanitizer job on the same Autotools package as the normal jobs.
        libmustacheSanitizedPackage = libmustachePackage.override {
          debugSupport = true;
          sanitizerSupport = true;
        };
        # The Autotools build rejects the Clang toolchain's assembler-only flag
        # under -Werror, so use the upstream CMake package for fuzz instrumentation.
        libmustacheFuzzPackage =
          (libmustacheCmakePackage.override {
            stdenv = pkgs.llvmPackages.stdenv;
            debugSupport = true;
            sanitizerSupport = true;
          }).overrideAttrs (finalAttrs: previousAttrs: {
            CXXFLAGS = "${previousAttrs.CXXFLAGS or ""} -fsanitize=fuzzer-no-link";
          });

        makePackage = {
          stdenv ? pkgs.stdenv,
          php ? pkgs.php,
          coverageSupport ? false,
          sanitizerSupport ? false,
          libmustacheOverride ? null,
        }:
          pkgs.callPackage ./nix/derivation.nix {
            inherit src;
            inherit stdenv php;
            inherit coverageSupport sanitizerSupport;
            valgrindSupport = !sanitizerSupport;
            libmustache =
              if libmustacheOverride != null
              then libmustacheOverride
              else if sanitizerSupport
              then libmustacheSanitizedPackage
              else libmustachePackage;
            mustache_spec = mustache_spec.packages.${system}.mustache-spec;
            buildPecl = pkgs.callPackage (nixpkgs + "/pkgs/build-support/php/build-pecl.nix") {
              inherit php stdenv;
            };
          };

        makeCheck = package:
          package.override {
            checkSupport = true;
          };

        phpMustacheFuzzPackage =
          (makePackage {
            stdenv = pkgs.llvmPackages.stdenv;
            php = php83FuzzerUnwrapped;
            sanitizerSupport = true;
            libmustacheOverride = libmustacheFuzzPackage;
          }).overrideAttrs (finalAttrs: previousAttrs: {
            CXXFLAGS = "${previousAttrs.CXXFLAGS or ""} -fsanitize=fuzzer-no-link";
          });

        phpMustacheFuzzer = pkgs.writeShellApplication {
          name = "php-mustache-fuzz";
          text = ''
            if (( $# == 0 )); then
              echo "usage: php-mustache-fuzz [libFuzzer options] CORPUS_DIRECTORY" >&2
              exit 2
            fi

            export MUSTACHE_FUZZ_EXTENSION=${phpMustacheFuzzPackage}/lib/php/extensions/mustache.so
            export ASAN_OPTIONS="abort_on_error=1:detect_leaks=1:halt_on_error=1"
            export UBSAN_OPTIONS="abort_on_error=1:halt_on_error=1:print_stacktrace=1"
            exec ${php83FuzzerUnwrapped}/bin/php-fuzz-mustache \
              -max_len=4096 \
              -timeout=5 \
              -dict=${./fuzz/mustache.dict} \
              "$@"
          '';
        };

        pre-commit-check = git-hooks.lib.${system}.run {
          src = src';
          hooks = {
            actionlint.enable = true;
            alejandra.enable = true;
            alejandra.excludes = ["\/vendor\/"];
            #clang-format.enable = true;
            #clang-format.types_or = ["c" "c++"];
            #clang-format.files = "\\.(c|h)$";
            markdownlint.enable = true;
            markdownlint.excludes = ["LICENSE\.md"];
            markdownlint.settings.configuration = {
              MD013 = {
                line_length = 1488;
                # this doesn't seem to work
                table = false;
              };
              MD024 = false;
            };
            shellcheck.enable = true;
            shellcheck.excludes = ["^.github/scripts/(docker|fold|linux|osx|suite).sh$"];
          };
        };

        makeDevShell = package:
          (pkgs.mkShell.override {
            stdenv = package.stdenv;
          }) {
            inputsFrom = [package];
            buildInputs = with pkgs; [
              actionlint
              autoconf-archive
              lcov
              gdb
              package.php.packages.composer
              pre-commit
              valgrind
            ];
            shellHook = ''
              ${pre-commit-check.shellHook}
              mkdir -p .direnv/include
              unlink .direnv/include/php
              ln -sf ${package.php.unwrapped.dev}/include/php/ .direnv/include/php
              export REPORT_EXIT_STATUS=1
              export NO_INTERACTION=1
              export PATH="$PWD/vendor/bin:$PATH"
              # opcache isn't getting loaded for tests because tests are run with '-n' and nixos doesn't compile
              # in opcache and relies on mkWrapper to load extensions
              export TEST_PHP_ARGS="-c ${package.php.phpIni} -j$(nproc --all)"
              # php.unwrapped from the buildDeps is overwriting php
              export PATH="${package.php}/bin:./vendor/bin:$PATH"
            '';
          };

        matrix = with pkgs; {
          php = {
            php80 = nix-phps.packages.${system}.php80;
            php81 = nix-phps.packages.${system}.php81;
            php82 = nix-phps.packages.${system}.php82;
            inherit php83 php84 php85;
          };
          stdenv = {
            gcc = stdenv;
            clang = clangStdenv;
            musl = pkgsMusl.stdenv;
          };
        };

        # @see https://github.com/NixOS/nixpkgs/pull/110787
        buildConfs =
          (lib.cartesianProduct {
            php = ["php81" "php82" "php83" "php84" "php85"];
            stdenv = [
              "gcc"
              "clang"
              # totally broken
              # "musl"
            ];
            coverageSupport = [false];
            sanitizerSupport = [false];
          })
          ++ (lib.cartesianProduct {
            php = ["php81" "php82" "php83" "php84" "php85"];
            stdenv = ["gcc"];
            coverageSupport = [true];
            sanitizerSupport = [false];
          })
          ++ [
            {
              php = "php80";
              stdenv = "gcc";
              coverageSupport = false;
              sanitizerSupport = false;
            }
            {
              php = "php83";
              stdenv = "gcc";
              coverageSupport = false;
              sanitizerSupport = true;
            }
          ];

        buildFn = {
          php,
          stdenv,
          coverageSupport ? false,
          sanitizerSupport ? false,
        }:
          lib.nameValuePair
          (lib.concatStringsSep "-" (lib.filter (v: v != "") [
            "${php}"
            "${stdenv}"
            #(if stdenv == "gcc" then "" else "${stdenv}")
            (
              if coverageSupport
              then "coverage"
              else ""
            )
            (
              if sanitizerSupport
              then "sanitized"
              else ""
            )
          ]))
          (
            makePackage {
              php =
                if sanitizerSupport
                then php83Sanitized
                else matrix.php.${php};
              stdenv = matrix.stdenv.${stdenv};
              inherit coverageSupport sanitizerSupport;
            }
          );

        packages' = builtins.listToAttrs (builtins.map buildFn buildConfs);
        packages =
          packages'
          // {
            default = packages.php83-gcc;
          };
      in {
        inherit packages;

        devShells = builtins.mapAttrs (name: package: makeDevShell package) packages;

        checks =
          {inherit pre-commit-check;}
          // (builtins.mapAttrs (name: package: makeCheck package) packages');

        apps.php83-clang-fuzzer = flake-utils.lib.mkApp {
          drv = phpMustacheFuzzer;
          exePath = "/bin/php-mustache-fuzz";
        };

        formatter = pkgs.alejandra;
      }
    )
    // {
      githubActions.matrix =
        (nix-github-actions.lib.mkGithubMatrix {
          attrPrefix = "checks";
          checks = nixpkgs.lib.getAttrs ["x86_64-linux"] self.checks;
        })
        .matrix;
    };
}
