let
    generateMustacheDrv = { pkgs, php, phpYaml }:
        let
            mustache_spec = pkgs.callPackage (import ((fetchTarball {
                url = https://github.com/jbboehr/mustache-spec/archive/5b85c1b58309e241a6f7c09fa57bd1c7b16fa9be.tar.gz;
                sha256 = "1h9zsnj4h8qdnzji5l9f9zmdy1nyxnf8by9869plyn7qlk71gdyv";
            }))) {};
        in
        pkgs.runCommand "pecl-mustache.tgz" {
            buildInputs = [ php ];
            src = builtins.filterSource
                (path: type: baseNameOf path != ".idea" && baseNameOf path != ".git" && baseNameOf path != "ci.nix")
                ./.;
        } ''
            cp -r $src/* .
            chmod -R +w tests
            php -d extension=${phpYaml}/lib/php/extensions/yaml.so generate-tests.php ${mustache_spec}/share/mustache-spec/specs
            pecl package | tee tmp.txt
            pecl_tgz=$(cat tmp.txt | grep -v Warning | awk '{print $2}')
            echo $pecl_tgz
            cp $pecl_tgz $out
        '';

    generateTestsForPlatform = { pkgs, path, phpAttr, dist }:
        pkgs.recurseIntoAttrs {
            mustache = let
                php = pkgs.${phpAttr};
            in pkgs.callPackage ./default.nix {
                inherit php;
                phpMustacheSrc = dist;
                buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            };
            # not working due to debuginfo issue
            # "error adding symbols: file in wrong format"
            # probably due to cross compiling
            # mustache-32bit = let
            #     php = pkgs.pkgsi686Linux.${phpAttr};
            # in pkgs.pkgsi686Linux.callPackage ./default.nix {
            #     inherit php;
            #     phpMustacheSrc = dist;
            #     buildPecl = pkgs.callPackage "${path}/pkgs/build-support/build-pecl.nix" { inherit php; };
            # };
        };
in
builtins.mapAttrs (k: _v:
  let
    path = builtins.fetchTarball {
       url = https://github.com/NixOS/nixpkgs/archive/release-20.03.tar.gz;
       name = "nixpkgs-20.03";
    };
    pkgs = import (path) { system = k; };

    dist = generateMustacheDrv {
        inherit pkgs;
        inherit (pkgs) php;
        phpYaml = pkgs.phpPackages.yaml;
    };
  in
  pkgs.recurseIntoAttrs {
    inherit dist;

    php72 = generateTestsForPlatform {
        inherit pkgs path dist;
        phpAttr = "php72";
    };

    php73 = let
        php = pkgs.php73;
    in generateTestsForPlatform {
        inherit pkgs path dist;
        phpAttr = "php73";
    };

    php74 = let
        php = pkgs.php74;
    in generateTestsForPlatform {
        inherit pkgs path dist;
        phpAttr = "php74";
    };
  }
) {
  x86_64-linux = {};
  # Uncomment to test build on macOS too
  # x86_64-darwin = {};
}
