{
  description = "jbboehr/php-mustache";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.05";
    nixpkgs-unstable.url = "github:nixos/nixpkgs/nixos-unstable";
    systems.url = "github:nix-systems/default-linux";
    flake-utils = {
      url = "github:numtide/flake-utils";
      inputs.systems.follows = "systems";
    };
    gitignore = {
      url = "github:hercules-ci/gitignore.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    pre-commit-hooks = {
      url = "github:cachix/pre-commit-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.nixpkgs-stable.follows = "nixpkgs";
      inputs.flake-utils.follows = "flake-utils";
      inputs.gitignore.follows = "gitignore";
    };
    nix-github-actions = {
      url = "github:nix-community/nix-github-actions";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    libmustache = {
      url = "github:jbboehr/libmustache";
      inputs.nixpkgs.follows = "nixpkgs";
      inputs.systems.follows = "systems";
      inputs.flake-utils.follows = "flake-utils";
      inputs.gitignore.follows = "gitignore";
    };
    mustache_spec.url = "github:jbboehr/mustache-spec";
  };

  outputs = {
    self,
    nixpkgs,
    nixpkgs-unstable,
    flake-utils,
    gitignore,
    pre-commit-hooks,
    systems,
    nix-github-actions,
    libmustache,
    mustache_spec,
    ...
  } @ args:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
        pkgs-unstable = nixpkgs-unstable.legacyPackages.${system};
        lib = pkgs.lib;

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
            '';
          };
        };

        makePackage = {
          stdenv ? pkgs.stdenv,
          php ? pkgs.php,
          coverageSupport ? false,
        }:
          pkgs.callPackage ./nix/derivation.nix {
            inherit src;
            inherit stdenv php;
            inherit coverageSupport;
            libmustache = libmustache.packages.${system}.libmustache;
            mustache_spec = mustache_spec.packages.${system}.mustache-spec;
            buildPecl = pkgs.callPackage (nixpkgs + "/pkgs/build-support/php/build-pecl.nix") {
              inherit php stdenv;
            };
          };

        makeCheck = package:
          package.override {
            checkSupport = true;
          };

        pre-commit-check = pre-commit-hooks.lib.${system}.run {
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
            shellcheck.excludes = ["^.github\/scripts\/"];
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
            inherit php81 php82 php83;
            php84 = pkgs-unstable.php84;
          };
          stdenv = {
            gcc = stdenv;
            clang = clangStdenv;
            musl = pkgsMusl.stdenv;
          };
        };

        # @see https://github.com/NixOS/nixpkgs/pull/110787
        buildConfs =
          (lib.cartesianProductOfSets {
            php = ["php81" "php82" "php83" "php84"];
            stdenv = [
              "gcc"
              "clang"
              # totally broken
              # "musl"
            ];
            coverageSupport = [false];
          })
          ++ [
            {
              php = "php81";
              stdenv = "gcc";
            }
          ]
          ++ (lib.cartesianProductOfSets {
            php = ["php81" "php82" "php83" "php84"];
            stdenv = ["gcc"];
            coverageSupport = [true];
          });

        buildFn = {
          php,
          stdenv,
          coverageSupport ? false,
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
          ]))
          (
            makePackage {
              php = matrix.php.${php};
              stdenv = matrix.stdenv.${stdenv};
              inherit coverageSupport;
            }
          );

        packages' = builtins.listToAttrs (builtins.map buildFn buildConfs);
        packages =
          packages'
          // {
            # php81 = packages.php81-gcc;
            # php82 = packages.php82-gcc;
            # php83 = packages.php83-gcc;
            # php84 = packages.php84-gcc;
            default = packages.php81-gcc;
          };
      in {
        inherit packages;

        devShells = builtins.mapAttrs (name: package: makeDevShell package) packages;

        checks =
          {inherit pre-commit-check;}
          // (builtins.mapAttrs (name: package: makeCheck package) packages);

        formatter = pkgs.alejandra;
      }
    )
    // {
      # prolly gonna break at some point
      githubActions.matrix.include = let
        cleanFn = v: v // {name = builtins.replaceStrings ["githubActions." "checks." "x86_64-linux."] ["" "" ""] v.attr;};
      in
        builtins.map cleanFn
        (nix-github-actions.lib.mkGithubMatrix {
          attrPrefix = "checks";
          checks = nixpkgs.lib.getAttrs ["x86_64-linux"] self.checks;
        })
        .matrix
        .include;
    };
}
